/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.nokia.examples;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import javax.microedition.contactless.ContactlessException;
import javax.microedition.contactless.DiscoveryManager;
import javax.microedition.contactless.TargetListener;
import javax.microedition.contactless.TargetProperties;
import javax.microedition.contactless.TargetType;
import javax.microedition.contactless.ndef.NDEFMessage;
import javax.microedition.contactless.ndef.NDEFRecord;
import javax.microedition.contactless.ndef.NDEFRecordType;
import javax.microedition.contactless.ndef.NDEFTagConnection;
import javax.microedition.io.Connector;
import javax.microedition.lcdui.AlertType;

/**
 *
 * @author ajakl
 */
public class NfcManager implements TargetListener, Runnable {

    private NFCinteractor callback;
    /** Manager for discovering targets. */
    private DiscoveryManager dm = null;
    /** Connection to the tag is saved here, as the actual processing is happening in a thread. */
    private NDEFTagConnection ndconn = null;
    private static final String uriAbbreviations[] = {
        "",
        "http://www.",
        "https://www.",
        "http://",
        "https://",
        "tel:",
        "mailto:",
        "ftp://anonymous:anonymous@",
        "ftp://ftp.",
        "ftps://",
        "sftp://",
        "smb://",
        "nfs://",
        "ftp://",
        "dav://",
        "news:",
        "telnet://",
        "imap:",
        "rtsp://",
        "urn:",
        "pop:",
        "sip:",
        "sips:",
        "tftp:",
        "btspp://",
        "btl2cap://",
        "btgoep://",
        "tcpobex://",
        "irdaobex://",
        "file://",
        "urn:epc:id:",
        "urn:epc:tag:",
        "urn:epc:pat:",
        "urn:epc:raw:",
        "urn:epc:",
        "urn:nfc:"};

    public NfcManager(NFCinteractor callback) {
        this.callback = callback;
    }

    public void deleteNfcInstances(boolean alsoRemoveDiscoveryManager) {
        if (ndconn != null) {
            try {
                ndconn.close();
                ndconn = null;
            } catch (IOException ex) {
                callback.displayAlert("IOException during close", ex.toString(), AlertType.ERROR);
            }
        }
        if (alsoRemoveDiscoveryManager && dm != null) {
            dm.removeTargetListener(this, TargetType.NDEF_TAG);
            dm = null;
        }
    }

    // ---------------------------------------------------------------------------------------------------------
    // NFC releated control code
    public void createNfcDiscoveryManager() {
        deleteNfcInstances(true);
        // Registration of the TargetListener for external contactless
        // Targets (in this RFID_TAG).
        try {
            dm = DiscoveryManager.getInstance();
            dm.addTargetListener(this, TargetType.NDEF_TAG);
        } catch (ContactlessException ce) {
            callback.displayAlert("ContactlessException", "Unable to register TargetListener: " + ce.toString(), AlertType.ERROR);
        }
    }

    /**
     * Implementation of the call-back function of the TargetListener.
     * @param targetProperties array of targets found by the phone
     */
    public void targetDetected(TargetProperties[] targetProperties) {

        // In case no targets were found, exit the method
        if (targetProperties.length == 0) {
            callback.displayAlert("Target detected", "No targets found", AlertType.WARNING);
            return;
        }

        // Make sure no connection is already open
        deleteNfcInstances(false);

        // NDEF Connection for write Operation
        ndconn = getNDEFTAGConnection(targetProperties);

        // Start a new thread for processing the tag, as recommended by the API specs.
        Thread t = new Thread(this);
        t.start();
    }

    /**
     * Open the connection to the NDEF tag when a target was found.
     * Shows an alert if there is an issue opening the connection.
     * @param tProp array containing all target properties that will be
     * searched for an NDEF connection.
     * @return a NDEF tag connection if one was found, or null otherwise.
     */
    private NDEFTagConnection getNDEFTAGConnection(TargetProperties[] tProp) {
        for (int j = 0; j < tProp.length; j++) {
            if (tProp[j].hasTargetType(TargetType.NDEF_TAG)) {
                String url = tProp[j].getUrl();
                if (url != null) {
                    try {
                        return (NDEFTagConnection) Connector.open(url);
                    } catch (Exception e) {
                        callback.displayAlert("Exception: NDEF Tag Connection", e.toString(), AlertType.ERROR);
                    }
                }
            }
        }
        return null;
    }

    /**
     * Check if the NDEF connection is still set. Will do a callback
     * to the listener class in case no connection is currently active.
     * (in addition to the callback in case of a failure).
     * @return true if a connection is active, false if otherwise.
     */
    private boolean checkNdefConnection() {
        if (ndconn == null) {
            callback.tagError("No active NDEF connection.");
            return false;
        }
        return true;
    }
    
    // ---------------------------------------------------------------------------------------------------------
    // Methods to interact with the physical tag (read / write). The methods also handle the potential errors.
    
    private NDEFMessage readMessageFromTag()
    {
        try {
            return ndconn.readNDEF();
        } catch (Exception ex) {
            handleException(ex);
        }
        return null;
    }

    private boolean writeMessageToTag(NDEFMessage ndefMessage) {
        if (!checkNdefConnection()) {
            return false;
        }

        boolean success = false;
        try {
            ndconn.writeNDEF(ndefMessage);
            success = true;
        } catch (Exception ex) {
            handleException(ex);
        } finally {
            // In case of an exception, close the connection properly
            deleteNfcInstances(false);
        }
        return success;
    }
    
    private void handleException (Exception ex)
    {
        if (ex instanceof IOException)
        {
            if (ex.toString().indexOf("-36") > -1) {
                // KErrDisconnected == -36
                callback.displayAlert("IOException", "-36: Communication problem / " + ex.getMessage(), AlertType.ERROR);
            } else {
                callback.displayAlert("IOException", ex.toString(), AlertType.ERROR);
            }  
        } else if (ex instanceof ContactlessException)
        {
            if (ex.toString().indexOf("-9") > -1) {
                // KErrOverflow == -9
                callback.displayAlert("ContactlessException", "-9: Not enough space on the tag / " + ex.getMessage(), AlertType.ERROR);
            } else if (ex.toString().indexOf("-2") > -1) {
                // KErrGeneral == -2
                // Often for unable to read tag - format not supported
                callback.displayAlert("ContactlessException", "-2: General error / " + ex.getMessage(), AlertType.ERROR);
            } else {
                callback.displayAlert("ContactlessException", ex.toString(), AlertType.ERROR);
            }
        } else {
            callback.displayAlert("Exception", ex.toString(), AlertType.ERROR);
        }
    }

    // ---------------------------------------------------------------------------------------------------------
    // Read messages
    /**
     * Processing method to read an NDEF message from a pre-established tag connection.
     */
    public void readNDEFMessage() {
        if (!checkNdefConnection()) {
            return;
        }
        // Get the message out of the connection
        NDEFMessage ndefMessage = readMessageFromTag();
        if (ndefMessage == null || ndefMessage.getNumberOfRecords() <= 0) {
            // No records are found, or no message contained in the connection
            callback.displayAlert("Read NDEF", "No records in the message.", AlertType.ERROR);
        } else {
            boolean recordHandled;
            // Go through all the records present in the message
            for (int i = 0; i < ndefMessage.getNumberOfRecords(); i++) {
                recordHandled = false;
                NDEFRecord rec = ndefMessage.getRecord(i);
                NDEFRecordType type = rec.getRecordType();
                String typeName = type.getName();
                if (typeName != null) {
                    // Enable specific tag handling
                    if (typeName.equals("Sp")) {
                        // Show Smart Poster
                        // Doesn't do actual parsing of the contents
                        byte[] payload = rec.getPayload();
                        callback.displayAlert("Smart Poster", new String(payload), AlertType.CONFIRMATION);
                        recordHandled = true;
                    } else if (typeName.equals("U")) {
                        // Parse URL
                        // Doesn't do actual parsing of the contents
                        byte[] payload = rec.getPayload();
                        callback.displayAlert("Url", new String(payload), AlertType.CONFIRMATION);
                        recordHandled = true;
                    } else if (typeName.equals("T")) {
                        // Parse URL
                        // Doesn't do actual parsing of the contents
                        byte[] payload = rec.getPayload();
                        callback.displayAlert("Text", new String(payload), AlertType.CONFIRMATION);
                        recordHandled = true;
                    }
                }
                if (!recordHandled) {
                    // Didn't do special parsing of the record - so just show general info about it
                    callback.displayAlert("Record " + (i + 1) + "/" + ndefMessage.getNumberOfRecords(), "format = " + type.getFormat() + ", name = " + type.getName() + "\n", AlertType.CONFIRMATION);
                }
            }
        }
    }
    

    // ---------------------------------------------------------------------------------------------------------
    // Delete messages (-> write empty message)
    /**
     * Processing method to delete an NDEF message from a pre-established tag connection.
     * Writes an empty message if a message already exists on the tag. Doesn't 
     * do anything if the tag is already empty.
     */
    public void deleteNDEFMessage() {
        if (!checkNdefConnection()) {
            return;
        }
        NDEFMessage ndefMessage = readMessageFromTag();
        if (ndefMessage == null || ndefMessage.getNumberOfRecords() <= 0) {
            callback.tagSuccess("Tag already empty");
        } else {
            NDEFRecord emptyRecord = new NDEFRecord(new NDEFRecordType(NDEFRecordType.EMPTY, null), null, null);
            NDEFRecord[] emptyRecordArray = {emptyRecord};
            NDEFMessage emptyMessage = new NDEFMessage(emptyRecordArray);
            if (writeMessageToTag(emptyMessage)) {
                callback.tagSuccess("Wrote empty message.");
            }
        }

    }

    // ---------------------------------------------------------------------------------------------------------
    // Write messages to tags
    
    public void writeUri(String fullUrl) throws UnsupportedEncodingException {
        if (!checkNdefConnection()) {
            return;
        }
        // Create NDEFMessage
        NDEFMessage message = new NDEFMessage();

        // Append the record to the message
        message.appendRecord(createUriRecord(fullUrl));

        // Write message to the tag
        if (writeMessageToTag(message)) {
            callback.tagSuccess("URI written");
        }
    }

    public void writeText(String fullText) throws UnsupportedEncodingException {
        if (!checkNdefConnection()) {
            return;
        }
        // Create NDEFMessage
        NDEFMessage message = new NDEFMessage();

        // Append the record to the message
        message.appendRecord(createTextRecord(fullText, "en"));

        // Write message to the tag
        if (writeMessageToTag(message)) {
            callback.tagSuccess("Text written");
        }
    }

    /**
     * Processing method to write a Smart Poster NDEF message to a pre-established tag connection.
     * Will take the title and the URL from the UI elements in the form.
     * @throws IOException
     * @throws ContactlessException 
     */
    public void writeSmartPoster(boolean writeMessages[], String fullUrl, String fullText, byte posterAction, String imageUri) throws UnsupportedEncodingException, IOException {

        if (!checkNdefConnection()) {
            return;
        }

        // Create the final Smart Poster meta-message
        NDEFMessage messageSmartPoster = new NDEFMessage();

        // Create the record containing all selected smart poster details
        NDEFRecord spRecord = createSpRecord(writeMessages, fullUrl, fullText, posterAction, imageUri);

        // Append the smart poster record to the meta-message
        messageSmartPoster.appendRecord(spRecord);

        // Write message to the tag
        if (writeMessageToTag(messageSmartPoster)) {
            callback.tagSuccess("Smart Poster written");
        }
    }

    public void writeSms(boolean writeMessages[], String smsUrl, String smsBody, String titleText, byte posterAction) throws UnsupportedEncodingException, IOException {
        if (!checkNdefConnection()) {
            return;
        }

        // Assemble SMS URL (contains phone number and body text)
        String tagSmsText = "sms:" + smsUrl + "?body=" + smsBody;

        NDEFRecord smsRecord;
        // Check if to write a smart poster or a URL tag

        if (writeMessages[0] || writeMessages[1]) {
            // Write a smart poster
            boolean writeSpMessages[] = new boolean[4];
            writeSpMessages[0] = true;              // Write URL & body -> always true
            writeSpMessages[1] = writeMessages[0];  // Write title text?
            writeSpMessages[2] = writeMessages[1];  // Write action?
            writeSpMessages[3] = false;             // No image
            smsRecord = createSpRecord(writeSpMessages, tagSmsText, titleText, posterAction, null);
        } else {
            // No title or action set -> write a URI tag
            smsRecord = createUriRecord(tagSmsText);
        }

        // Create the final SMS message
        NDEFMessage messageSms = new NDEFMessage();
        messageSms.appendRecord(smsRecord);
        // Write message to the tag
        if (writeMessageToTag(messageSms)) {
            callback.tagSuccess("Sms tag written");
        }
    }

    public void writeImage(String imageUri) throws IOException {
        if (!checkNdefConnection()) {
            return;
        }

        // Create NDEFMessage
        NDEFMessage message = new NDEFMessage();

        message.appendRecord(createImageRecord(imageUri));

        // Write message to the tag
        if (writeMessageToTag(message)) {
            callback.tagSuccess("Image written");
        }
    }

    public void writeCustom(String tagTypeUri, byte[] tagPayload) throws UnsupportedEncodingException {
        if (!checkNdefConnection()) {
            return;
        }
        // Create NDEFMessage
        NDEFMessage message = new NDEFMessage();

        // Append the record to the message
        message.appendRecord(createCustomRecord(tagTypeUri, tagPayload));

        // Write message to the tag
        if (writeMessageToTag(message)) {
            callback.tagSuccess("Custom tag written");
        }
    }
    
    
    public void writeCombination(String tagUrl, String tagTypeUri, byte[] tagPayload) throws UnsupportedEncodingException {
        if (!checkNdefConnection()) {
            return;
        }
        // Create NDEFMessage
        NDEFMessage message = new NDEFMessage();

        // Append the record to the message
        message.appendRecord(createCustomRecord(tagTypeUri, tagPayload));
        message.appendRecord(createUriRecord(tagUrl));

        // Write message to the tag
        if (writeMessageToTag(message)) {
            callback.tagSuccess("Combination tag written");
        }
    }

    // ---------------------------------------------------------------------------------------------------------
    // Create individual records
    private NDEFRecord createUriRecord(final String fullUri) throws UnsupportedEncodingException {
        byte[] urlPrefix = new byte[1];
        byte[] url = null;

        boolean foundAbbreviation = false;
        for (int i = 1; i < uriAbbreviations.length; i++) {
            if (fullUri.startsWith(uriAbbreviations[i])) {
                urlPrefix[0] = (byte) i;
                url = fullUri.substring(uriAbbreviations[i].length(), fullUri.length()).getBytes("utf-8");
                foundAbbreviation = true;
                break;
            }
        }
        if (!foundAbbreviation) {
            // Store the full URL to the tag - always using UTF-8
            urlPrefix[0] = (byte) 0x00;
            url = fullUri.getBytes("utf-8");
        }

        // Create the record for the URL
        NDEFRecord recordUrl = new NDEFRecord(new NDEFRecordType(NDEFRecordType.NFC_FORUM_RTD, "urn:nfc:wkt:U"), null, null);
        recordUrl.appendPayload(urlPrefix);
        recordUrl.appendPayload(url);

        // Create NDEF Record to be added to NDEF Message
        return recordUrl;
    }

    private NDEFRecord createTextRecord(final String text, final String lang) throws UnsupportedEncodingException {
        byte[] lang_bytes = lang.getBytes("US-ASCII");
        byte[] status_lang_len = {(byte) (lang_bytes.length & 0x3f)};
        // Bit at 0x80 of status_lang_len would need to be set to 1 for UTF-16 text.
        // We always use UTF-8 here in this example.
        byte[] textContents = text.getBytes("utf-8");

        NDEFRecord recordText = new NDEFRecord(new NDEFRecordType(NDEFRecordType.NFC_FORUM_RTD, "urn:nfc:wkt:T"), null, null);
        recordText.appendPayload(status_lang_len);
        recordText.appendPayload(lang_bytes);
        recordText.appendPayload(textContents);

        return recordText;
    }

    private NDEFRecord createSpRecord(final boolean[] writeDetails, final String fullUrl, final String title, final byte action, final String imageFilename) throws UnsupportedEncodingException, IOException {
        if (writeDetails.length != 4) {
            return null;
        }

        // Create NDEFMessage
        NDEFMessage message = new NDEFMessage();

        if (writeDetails[0]) {
            // Url
            message.appendRecord(createUriRecord(fullUrl));
        }
        if (writeDetails[1]) {
            // Title
            message.appendRecord(createTextRecord(title, "en"));
        }
        if (writeDetails[2]) {
            // Action
            message.appendRecord(createActionRecord(action));
        }
        if (writeDetails[3]) {
            // Image
            message.appendRecord(createImageRecord(imageFilename));
        }

        // Create the Smart Poster record
        NDEFRecord recordSmartPoster = new NDEFRecord(new NDEFRecordType(NDEFRecordType.NFC_FORUM_RTD, "urn:nfc:wkt:Sp"), null, null);
        // Add the URL and the title as a payload to the Smart Poster record.
        recordSmartPoster.appendPayload(message.toByteArray()); // add content of previously created NDEFMessage

        return recordSmartPoster;
    }

    private NDEFRecord createActionRecord(final byte action) {
        byte[] actionRecord = {0x11, 0x03, 0x01, 'a', 'c', 't', action};

        return new NDEFRecord(actionRecord, 0);
    }

    private NDEFRecord createImageRecord(final String filename) throws IOException {
        // Read image from phone to ByteArrayOutputStream
        ByteArrayOutputStream baos = getImage(filename);

        final String fileExt = filename.substring(filename.lastIndexOf('.') + 1, filename.length());
        String mimeType = "";
        if (fileExt.equalsIgnoreCase("png")) {
            mimeType = "image/png";
        } else if (fileExt.equalsIgnoreCase("jpg") || fileExt.equalsIgnoreCase("jpeg")) {
            mimeType = "image/jpeg";
        } else {
            callback.displayAlert("Image", "Unrecognized file type", AlertType.WARNING);
        }

        // Create NDEF Record to be added to NDEF Message
        return new NDEFRecord(new NDEFRecordType(
                NDEFRecordType.MIME, mimeType), null, baos.toByteArray());
    }

    /**
     * Read image from phone to a ByteArrayOutputStream
     */
    private ByteArrayOutputStream getImage(final String filename) throws IOException {
        InputStream is = getClass().getResourceAsStream(filename);
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        int c;
        while ((c = is.read()) != -1) {
            baos.write(c);
        }
        return baos;
    }

    private NDEFRecord createCustomRecord(final String tagUri, final byte[] payload) throws UnsupportedEncodingException {
        // Create NDEF Record to be added to NDEF Message
        NDEFRecord recordCustom = new NDEFRecord(new NDEFRecordType(NDEFRecordType.EXTERNAL_RTD, tagUri), null, null);
        recordCustom.appendPayload(payload);

        return recordCustom;
    }

    public void run() {
        callback.tagReady();
    }
}
