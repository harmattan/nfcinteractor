/****************************************************************************
 **
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Andreas Jakl (andreas.jakl@nokia.com)
 **
 ** The Java ME-based Nfc Interactor can write many different 
 ** NFC Forum NDEF message formats to all supported tag formats, 
 ** delete the existing message from a tag (by overwriting it with 
 ** an empty message) and read the contents of an NDEF-formatted tag.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 **     the names of its contributors may be used to endorse or promote
 **     products derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/
package com.nokia.examples;

// Packages for contactless Communcation
import java.io.ByteArrayOutputStream;
import java.io.UnsupportedEncodingException;
import javax.microedition.contactless.ContactlessException;
import javax.microedition.contactless.DiscoveryManager;
import javax.microedition.contactless.TargetProperties;
import javax.microedition.contactless.TargetListener;
import javax.microedition.contactless.TargetType;
import java.io.IOException;

// Packages for NDEF
import java.io.InputStream;
import javax.microedition.contactless.ndef.NDEFRecord;
import javax.microedition.contactless.ndef.NDEFMessage;
import javax.microedition.contactless.ndef.NDEFRecordType;
import javax.microedition.contactless.ndef.NDEFTagConnection;
import javax.microedition.io.Connector;

// Packages for GUI
import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.ItemStateListener;
import javax.microedition.lcdui.TextField;

import javax.microedition.midlet.*;

public class NFCinteractor extends MIDlet implements TargetListener, CommandListener, ItemStateListener, Runnable {

    /** To determine the first app start. */
    private boolean initialized = false;
    /** Manager for discovering targets. */
    private DiscoveryManager dm = null;
    /** Connection to the tag is saved here, as the actual processing is happening in a thread. */
    private NDEFTagConnection ndconn = null;
    /** Command to exit the app. */
    private Command exitCommand;
    /** Main UI form. */
    private Form form;
    
    // Operation mode UI
    /** Selection UI element to choose the current interaction mode. */
    private ChoiceGroup operationModeSelector;
    /** Names of the operating modes, to be used in the UI element. */
    private static final String operatingModeNames[] = {
        "Read",
        "Write Smart Poster",
        "Write URI",
        "Write Text",
        "Write SMS",
        "Write Image",
        "Write Custom",
        "Delete"
    };
    /** Current operation mode. Can be either READ_TAG, WRITE_TAG or DELETE_TAG. */
    private int operationMode = READ_TAG;
    /** When touching an NFC tag: the tag will be read. */
    private static final int READ_TAG = 0;
    /** When touching an NFC tag: the Smart Poster info currently visible in the form is written to the tag. */
    private static final int WRITE_SP_TAG = 1;
    /** When touching an NFC tag: write a uri to the tag. */
    private static final int WRITE_URI_TAG = 2;
    /** When touching an NFC tag: write text to the tag. */
    private static final int WRITE_TEXT_TAG = 3;
    /** When touching an NFC tag: write an sms link to the tag. */
    private static final int WRITE_SMS_TAG = 4;
    /** When touching an NFC tag: write an image to the tag. */
    private static final int WRITE_IMAGE_TAG = 5;
    /** When touching an NFC tag: write a custom tag format. */
    private static final int WRITE_CUSTOM_TAG = 6;
    /** When touching an NFC tag: the record currently present on the tag is overwritten with an empty record. */
    private static final int DELETE_TAG = 7;
    
    // Settings for the writing modes
    /** UI element to choose which messages to write to the tag. */
    private ChoiceGroup posterEnabledMessages;
    /** UI element to choose the action associated with the Smart Poster. Only visible when in WRITE_TAG operation mode. */
    private ChoiceGroup posterAction;
    /** ID of the action element in the form. */
    private TextField tagUrl;
    /** UI element to enter the text of a record. */
    private TextField tagText;
    /** UI element to enter the URI of a record. */
    private TextField tagTypeUri;
    /** UI element to enter the URI of a record. */
    private TextField tagCustomPayload;
    /** UI element to choose which parts to write for an sms tag. */
    private ChoiceGroup tagSmsEnabledMessages;
    /** UI element to enter the SMS recipient number. */
    private TextField tagSmsNumber;
    /** UI element to enter the SMS body. */
    private TextField tagSmsBody;
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

    /**
     * Constructor of the MIDlet. Initializes the UI.
     */
    public NFCinteractor() {
        // Construct the UI
        exitCommand = new Command("Exit", Command.EXIT, 1);
        form = new Form("NFCinteractor");

        // Choice group to choose the operation mode
        form.addCommand(exitCommand);
        operationModeSelector = new ChoiceGroup("Choose Mode", ChoiceGroup.EXCLUSIVE, operatingModeNames, null);
        operationModeSelector.setSelectedIndex(READ_TAG, true);
        form.append(operationModeSelector);

        // Prepare the UI elements that are only visible and relevant when writing a tag
        // URL field (URI tag, Smart Poster)
        tagUrl = new TextField("URL", "http://nokia.com/", 255, TextField.URL);
        //tagUrl = new TextField("URL", "file:///C/sys/bin/journey2.exe", 255, TextField.URL);
        
        // Text field (Text only tag, Smart Poster)
        tagText = new TextField("Text", "Nokia", 255, TextField.ANY);
        
        // Action (Smart Poster)
        posterAction = new ChoiceGroup("Action", ChoiceGroup.EXCLUSIVE);
        posterAction.append("Do the action", null);
        posterAction.append("Save for later", null);
        posterAction.append("Open for editing", null);
        posterAction.setSelectedIndex(0, true);
        
        // Selection which messages to write (Smart Poster)
        posterEnabledMessages = new ChoiceGroup("Messages", ChoiceGroup.MULTIPLE);
        posterEnabledMessages.append("URL", null);
        posterEnabledMessages.append("Title", null);
        posterEnabledMessages.append("Action", null);
        posterEnabledMessages.append("Icon", null);
        boolean[] posterEnabledFlags = {true, true, false, false};
        posterEnabledMessages.setSelectedFlags(posterEnabledFlags);
        
        // Custom tag
        tagTypeUri = new TextField("Tag URI", "urn:nfc:ext:nokia:custom", 255, TextField.URL);
        tagCustomPayload = new TextField("Payload", "Nokia", 255, TextField.ANY);

        // SMS
        tagSmsEnabledMessages = new ChoiceGroup("SMS Options", ChoiceGroup.MULTIPLE);
        tagSmsEnabledMessages.append("Title text (-> Sp)", null);
        tagSmsEnabledMessages.append("Action (-> Sp)", null);
        boolean[] smsEnabledFlags = {false, false};
        tagSmsEnabledMessages.setSelectedFlags(smsEnabledFlags);
        tagSmsNumber = new TextField("SMS Recipient", "+1234", 255, TextField.PHONENUMBER);
        tagSmsBody = new TextField("SMS Body", "Hello", 255, TextField.ANY);
    }

    /**
     * Called when the midlet is entering the active state.
     * On the very first start-up, this registers listening for NDEF tags.
     */
    public void startApp() {
        Display.getDisplay(this).setCurrent(form);
        if (!initialized) {
            form.setItemStateListener(this);
            form.setCommandListener(this);
            createNfcDiscoveryManager();
        }
    }

    public void pauseApp() {
    }

    public void destroyApp(boolean unconditional) {
        deleteNfcInstances(true);
    }

    /**
     * Call-back from the choice group that selects the operation mode.
     */
    public void itemStateChanged(Item item) {
        if (item == operationModeSelector) {
            final int newOperationMode = operationModeSelector.getSelectedIndex();
            // Activate the new operation mode.
            activateOperationMode(newOperationMode);
        }
    }

    /**
     * Switches the internal operation mode between reading, writing and deleting tags.
     * Also adjusts the UI in the form accordingly
     * @param newOperationMode one of the defined operation modes 
     * (READ_TAG, WRITE_TAG or DELETE_TAG) that should be activated.
     */
    private void activateOperationMode(final int newOperationMode) {
        if (operationMode != newOperationMode) {
            // Clean up UI elements - everything except the first element
            // (-> which is the operation mode choice group)
            final int numChoiceElements = form.size();
            for (int i = numChoiceElements - 1; i > 0; i--)
            {
                form.delete(i);
            }
            // The new operation mode is writing tags (previously, something
            // different was active): make the input items visible for
            // entering the tag type specific info.
            if (newOperationMode == WRITE_SP_TAG) {
                form.append(posterEnabledMessages);
                form.append(tagText);
                form.append(tagUrl);
                form.append(posterAction);
            } else if (newOperationMode == WRITE_URI_TAG) {
                form.append(tagUrl);
            } else if (newOperationMode == WRITE_TEXT_TAG) {
                form.append(tagText);
            } else if (newOperationMode == WRITE_SMS_TAG) {
                form.append(tagSmsEnabledMessages);
                form.append(tagSmsNumber);
                form.append(tagSmsBody);
                form.append(tagText);
                form.append(posterAction);
            } else if (newOperationMode == WRITE_CUSTOM_TAG) {
                form.append(tagTypeUri);
                form.append(tagCustomPayload);
            }
            operationMode = newOperationMode;
        }
    }

    // ---------------------------------------------------------------------------------------------------------
    // NFC releated control code
    
    private void deleteNfcInstances(boolean alsoRemoveDiscoveryManager) {
        if (ndconn != null) {
            try {
                ndconn.close();
                ndconn = null;
            } catch (IOException ex) {
                displayAlert("IOException during close", ex.toString(), AlertType.ERROR);
            }
        }
        if (alsoRemoveDiscoveryManager && dm != null) {
            dm.removeTargetListener(this, TargetType.NDEF_TAG);
            dm = null;
        }
    }

    private void createNfcDiscoveryManager() {
        deleteNfcInstances(true);
        // Registration of the TargetListener for external contactless
        // Targets (in this RFID_TAG).
        try {
            dm = DiscoveryManager.getInstance();
            dm.addTargetListener(this, TargetType.NDEF_TAG);

        } catch (ContactlessException ce) {
            displayAlert("ContactlessException", "Unable to register TargetListener: " + ce.toString(), AlertType.ERROR);
        }
    }
    
    /**
     * Implementation of the call-back function of the TargetListener.
     * @param targetProperties array of targets found by the phone
     */
    public void targetDetected(TargetProperties[] targetProperties) {

        // In case no targets were found, exit the method
        if (targetProperties.length == 0) {
            displayAlert("Target detected", "No targets found", AlertType.WARNING);
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
                        displayAlert("Exception: NDEF Tag Connection", e.toString(), AlertType.ERROR);
                    }
                }
            }
        }
        return null;
    }

    /**
     * Thread for processing the tag, once the connection to it has been established.
     */
    public void run() {
        if (ndconn != null) {
            try {
                /*if (operationMode != READ_TAG)
                {
                    // Especially in case there was an issue with writing before, reading the current
                    // contents once before writing makes the successive writing successful again.
                    // Disadvantage: the example doesn't attempt to write to the target in case reading
                    // doesn't succeed at all.
                    ndconn.readNDEF();
                }*/
                // Call the handling method depending on the current operation mode.
                switch (operationMode) {
                    case READ_TAG:
                        readNDEFMessage(ndconn);
                        break;
                    case WRITE_SP_TAG:
                        writeSmartPoster(ndconn);
                        break;
                    case WRITE_IMAGE_TAG:
                        writeImage(ndconn);
                        break;
                    case WRITE_URI_TAG:
                        writeUri(ndconn);
                        break;
                    case WRITE_TEXT_TAG:
                        writeText(ndconn);
                        break;
                    case WRITE_SMS_TAG:
                        writeSms(ndconn);
                        break;
                    case WRITE_CUSTOM_TAG:
                        writeCustom(ndconn);
                        break;
                    case DELETE_TAG:
                        deleteNDEFMessage(ndconn);
                        break;
                    default:
                        displayAlert("Illegal Action", "", AlertType.ERROR);
                        break;
                }

            } catch (IOException e) {
                if (e.toString().indexOf("-36") > -1) {
                    // KErrDisconnected == -36
                    displayAlert("IOException", "-36: Communication problem / " + e.getMessage(), AlertType.ERROR);
                } else {
                    displayAlert("IOException", e.toString(), AlertType.ERROR);
                }
            } catch (ContactlessException e) {
                if (e.toString().indexOf("-9") > -1) {
                    // KErrOverflow == -9
                    displayAlert("ContactlessException", "-9: Not enough space on the tag / " + e.getMessage(), AlertType.ERROR);
                } else if (e.toString().indexOf("-2") > -1) {
                    // KErrGeneral == -2
                    // Often for unable to read tag - format not supported
                    displayAlert("ContactlessException", "-2: General error / " + e.getMessage(), AlertType.ERROR);
                } else {
                    displayAlert("ContactlessException", e.toString(), AlertType.ERROR);
                }
            } catch (Exception e) {
                displayAlert("Exception", e.toString(), AlertType.ERROR);
            } finally {
                // In case of an exception, close the connection properly
                deleteNfcInstances(false);
            }
        } else {
            displayAlert("Connection not found", "Lost NDEF Connection", AlertType.ERROR);
        }
    }

    // ---------------------------------------------------------------------------------------------------------
    // Read messages
    
    /**
     * Processing method to read an NDEF message from a pre-established tag connection.
     * @param ndconn established connection to the NDEF tag.
     * @throws IOException
     * @throws ContactlessException 
     */
    private void readNDEFMessage(NDEFTagConnection ndconn) throws IOException, ContactlessException {
        // Get the message out of the connection
        NDEFMessage ndefMessage = ndconn.readNDEF();
        if (ndefMessage == null || ndefMessage.getNumberOfRecords() <= 0) {
            // No records are found, or no message contained in the connection
            displayAlert("Read NDEF", "No records in the message.", AlertType.ERROR);
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
                        displayAlert("Smart Poster", new String(payload), AlertType.CONFIRMATION);
                        recordHandled = true;

                    } else if (typeName.equals("U")) {
                        // Parse URL
                        // Doesn't do actual parsing of the contents
                        byte[] payload = rec.getPayload();
                        displayAlert("Url", new String(payload), AlertType.CONFIRMATION);
                        recordHandled = true;
                    } else if (typeName.equals("T")) {
                        // Parse URL
                        // Doesn't do actual parsing of the contents
                        byte[] payload = rec.getPayload();
                        displayAlert("Text", new String(payload), AlertType.CONFIRMATION);
                        recordHandled = true;
                    }
                }
                if (!recordHandled) {
                    // Didn't do special parsing of the record - so just show general info about it
                    displayAlert("Record " + (i + 1) + "/" + ndefMessage.getNumberOfRecords(), "format = " + type.getFormat() + ", name = " + type.getName() + "\n", AlertType.CONFIRMATION);
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
     * @param ndconn established connection to the NDEF tag.
     * @throws ContactlessException
     * @throws IOException 
     */
    private void deleteNDEFMessage(NDEFTagConnection ndconn) throws ContactlessException, IOException {
        NDEFMessage ndefMessage = ndconn.readNDEF();
        if (ndefMessage == null || ndefMessage.getNumberOfRecords() <= 0) {
            displayAlert("Delete NDEF", "Tag already empty.", AlertType.ERROR);
        } else {
            NDEFRecord emptyRecord = new NDEFRecord(new NDEFRecordType(NDEFRecordType.EMPTY, null), null, null);
            NDEFRecord[] emptyRecordArray = {emptyRecord};
            NDEFMessage emptyMessage = new NDEFMessage(emptyRecordArray);
            ndconn.writeNDEF(emptyMessage);
            displayAlert("Delete NDEF", "Wrote empty message.", AlertType.CONFIRMATION);
        }

    }

    // ---------------------------------------------------------------------------------------------------------
    // Write messages to tags

    private void writeUri(NDEFTagConnection ndconn) throws IOException, ContactlessException {
        // Create NDEFMessage
        NDEFMessage message = new NDEFMessage();

        String fullUrl = tagUrl.getString();

        // Append the record to the message
        message.appendRecord(createUriRecord(fullUrl));

        // Write message to the tag
        ndconn.writeNDEF(message);

        displayAlert(form.getTitle(), "URI written", AlertType.CONFIRMATION);
    }
    
    private void writeText(NDEFTagConnection ndconn) throws IOException, ContactlessException {
        // Create NDEFMessage
        NDEFMessage message = new NDEFMessage();

        String fullText = tagText.getString();

        // Append the record to the message
        message.appendRecord(createTextRecord(fullText, "en"));

        // Write message to the tag
        ndconn.writeNDEF(message);

        displayAlert(form.getTitle(), "Text written", AlertType.CONFIRMATION);
    }
    
    /**
     * Processing method to write a Smart Poster NDEF message to a pre-established tag connection.
     * Will take the title and the URL from the UI elements in the form.
     * @param ndconn established connection to the NDEF tag.
     * @throws IOException
     * @throws ContactlessException 
     */
    private void writeSmartPoster(NDEFTagConnection ndconn) throws IOException, ContactlessException {

        boolean writeMessages[] = new boolean[posterEnabledMessages.size()];
        posterEnabledMessages.getSelectedFlags(writeMessages);

        // Create the final Smart Poster meta-message
        NDEFMessage messageSmartPoster = new NDEFMessage();

        // Create the record containing all selected smart poster details
        NDEFRecord spRecord = createSpRecord(writeMessages, tagUrl.getString(), tagText.getString(), (byte) posterAction.getSelectedIndex(), "/minimal.png");

        // Append the smart poster record to the meta-message
        messageSmartPoster.appendRecord(spRecord);

        // Write message to the tag
        ndconn.writeNDEF(messageSmartPoster);

        displayAlert(form.getTitle(), "Smart Poster written", AlertType.CONFIRMATION);
    }
    
    private void writeSms(NDEFTagConnection ndconn) throws IOException, ContactlessException {

        // Assemble SMS URL (contains phone number and body text)
        String smsUrl = "sms:" + tagSmsNumber.getString() + "?body=" + tagSmsBody.getString();

        NDEFRecord smsRecord;
        // Check if to write a smart poster or a URL tag
        boolean writeMessages[] = new boolean[tagSmsEnabledMessages.size()];
        tagSmsEnabledMessages.getSelectedFlags(writeMessages);

        if (writeMessages[0] || writeMessages[1]) {
            // Write a smart poster
            boolean writeSpMessages[] = new boolean[4];
            writeSpMessages[0] = true;              // Write URL & body -> always true
            writeSpMessages[1] = writeMessages[0];  // Write title text?
            writeSpMessages[2] = writeMessages[1];  // Write action?
            writeSpMessages[3] = false;             // No image
            smsRecord = createSpRecord(writeSpMessages, smsUrl, tagText.getString(), (byte) posterAction.getSelectedIndex(), null);
        } else {
            // No title or action set -> write a URI tag
            smsRecord = createUriRecord(smsUrl);
        }

        // Create the final SMS message
        NDEFMessage messageSms = new NDEFMessage();
        messageSms.appendRecord(smsRecord);
        // Write message to the tag
        ndconn.writeNDEF(messageSms);

        displayAlert(form.getTitle(), "Sms tag written", AlertType.CONFIRMATION);
    }
    
    private void writeImage(NDEFTagConnection ndconn) throws IOException, ContactlessException {

        // Create NDEFMessage
        NDEFMessage message = new NDEFMessage();

        message.appendRecord(createImageRecord("/minimal.png"));

        // Write message to the tag
        ndconn.writeNDEF(message);

        displayAlert(form.getTitle(), "Image written", AlertType.CONFIRMATION);
    }
    
    private void writeCustom(NDEFTagConnection ndconn) throws IOException, ContactlessException {
        // Create NDEFMessage
        NDEFMessage message = new NDEFMessage();

        String tagUri = tagTypeUri.getString();

        // Append the record to the message
        message.appendRecord(createCustomRecord(tagUri, tagCustomPayload.getString().getBytes("utf-8")));

        // Write message to the tag
        ndconn.writeNDEF(message);

        displayAlert(form.getTitle(), "Custom tag written", AlertType.CONFIRMATION);
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
            displayAlert("Image", "Unrecognized file type", AlertType.WARNING);
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


    // ---------------------------------------------------------------------------------------------------------
    // UI Handling code

    /**
     * Implementation of the call-back function of the CommandListener
     * @param command: command key pressed
     * @param displayable: associated displayable Object
     */
    public void commandAction(Command command, Displayable displayable) {
        if (command == exitCommand) {
            // Exit the application
            destroyApp(false);
            notifyDestroyed();
        }
    }

    /**
     * Utility function to show a Java ME alert, as used for informing the user
     * about events in this demo app.
     * @param title title text to use for the message box.
     * @param text text to show as the main message in the box.
     * @param type one of the available alert types, defining the icon, sound
     * and display length.
     */
    private void displayAlert(final String title, final String text, final AlertType type) {
        Alert al = new Alert(title, text, null, type);
        Display.getDisplay(this).setCurrent(al, form);
    }
}
