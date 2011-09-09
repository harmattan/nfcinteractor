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
import java.io.IOException;

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
import javax.microedition.lcdui.StringItem;
import javax.microedition.lcdui.TextField;

import javax.microedition.midlet.*;

public class NFCinteractor extends MIDlet implements CommandListener, ItemStateListener {

    /** To determine the first app start. */
    private boolean initialized = false;
    /** Is NFC supported by this device? */
    private boolean nfcSupported = false;
    /** The nfcManager handles all interaction with the contactless communication API. */
    private NfcManager nfcManager;
    
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
        "Write Annotated URL",
        "Write Image",
        "Write Geo",
        "Write Custom",
        "Write Combination",
        "Delete (Empty Msg)"
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
    /** When touching an NFC tag: write an annotated url (url record and text record 
     * without Smart Poster meta-record) to the tag. Used by some Qt Mobility examples. */
    private static final int WRITE_ANNOTATED_URL_TAG = 5;
    /** When touching an NFC tag: write an image to the tag. */
    private static final int WRITE_IMAGE_TAG = 6;
    /** When touching an NFC tag: write a geo URI to the tag. Specs: http://tools.ietf.org/rfc/rfc5870 */
    private static final int WRITE_GEO_TAG = 7;
    /** When touching an NFC tag: write a custom tag format. */
    private static final int WRITE_CUSTOM_TAG = 8;
    /** When touching an NFC tag: write a combination tag format:
     * 1. Custom record (for handling with a custom content handler plug-in) & 
     * 2. URL (e.g., a link to the Nokia Store to download the app including
     * the content-handler plug-in). */
    private static final int WRITE_COMBINATION_TAG = 9;
    /** When touching an NFC tag: the record currently present on the tag is overwritten with an empty record. */
    private static final int DELETE_TAG = 10;
    // Settings for the writing modes
    /** UI element to choose which messages to write to the tag. */
    private ChoiceGroup posterEnabledMessages;
    /** UI element to choose the action associated with the Smart Poster. Only visible when in WRITE_TAG operation mode. */
    private ChoiceGroup posterAction;
    /** UI element to enter the URL of a record. */
    private TextField tagUrl;
    /** UI element to enter the text of a record. */
    private TextField tagText;
    /** UI element to enter the text language of a record. */
    private TextField tagTextLanguage;
    /** UI element to enter the Type Uri of a record. */
    private TextField tagTypeUri;
    /** UI element to enter the payload of a record. */
    private TextField tagCustomPayload;
    /** UI element to choose which image to store on the tag. */
    private ChoiceGroup tagChooseImage;
    /** UI element to choose which parts to write for an sms tag. */
    private ChoiceGroup tagSmsEnabledMessages;
    /** UI element to enter the SMS recipient number. */
    private TextField tagSmsNumber;
    /** UI element to enter the SMS body. */
    private TextField tagSmsBody;
    /** UI element to enter the latitude. */
    private TextField tagLatitude;
    /** UI element to enter the longitude. */
    private TextField tagLongitude;
    /** Choose mechanism to write geo tag. */
    private ChoiceGroup tagGeoType;

    /**
     * Constructor of the MIDlet. Initializes the UI.
     */
    public NFCinteractor() {
        // Construct the UI
        form = new Form("NFCinteractor");
        exitCommand = new Command("Exit", Command.EXIT, 1);
        form.addCommand(exitCommand);
        
        // Check NFC availability
        String nfcVersion = System.getProperty("microedition.contactless.version");
        if ((nfcVersion == null) || (nfcVersion.length () == 0) || nfcVersion.equalsIgnoreCase ("null"))
        {
            // NFC APIs not supported
            String deviceName = System.getProperty ("microedition.platform");
            if (deviceName.startsWith("NokiaC7-00"))
            {
                // If using a Nokia C7, updating the firmware will enable NFC support
                form.append("Please upgrade the device firmware to experience the required NFC functionality.");
            } 
            else 
            {
                // On all other devices, print that NFC is not supported.
                form.append("NFC is not supported by your device.");
            }
        } 
        else 
        {
            // NFC is supported - construct full UI and NFC manager class.
            nfcSupported = true;
            createMainUi();
        }

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
            if (nfcSupported) {
                nfcManager = new NfcManager(this);
                nfcSupported = nfcManager.createNfcDiscoveryManager();
            }
        }
    }

    public void pauseApp() {
    }

    public void destroyApp(boolean unconditional) {
        if (nfcManager != null)
        {
            nfcManager.deleteNfcInstances(true);
        }
    }
    
    private void createMainUi() {
        // Choice group to choose the operation mode
        setupFormHeader(WRITE_URI_TAG, READ_TAG);

        // Prepare the UI elements that are only visible and relevant when writing a tag
        // URL field (URI tag, Smart Poster, Annotated URL)
        tagUrl = new TextField("URL", "http://nokia.com/", 255, TextField.URL);

        // Text field (Text only tag, Smart Poster)
        tagText = new TextField("Text", "Nokia", 255, TextField.ANY);
        tagTextLanguage = new TextField("Language", "en", 5, TextField.ANY);

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
        tagTypeUri = new TextField("Tag URI", "urn:nfc:ext:nokia.com:custom", 255, TextField.URL);
        tagCustomPayload = new TextField("Payload", "Nokia", 255, TextField.ANY);

        // Image chooser
        tagChooseImage = new ChoiceGroup("Choose image", ChoiceGroup.EXCLUSIVE);
        tagChooseImage.append("Minimal GIF (48 bytes)", null);
        tagChooseImage.append("Minimal PNG (80 bytes)", null);
        tagChooseImage.append("Nokia PNG (225 bytes)", null);
        tagChooseImage.setSelectedIndex(1, true);

        // SMS
        tagSmsEnabledMessages = new ChoiceGroup("SMS Options", ChoiceGroup.MULTIPLE);
        tagSmsEnabledMessages.append("Title text (-> Sp)", null);
        tagSmsEnabledMessages.append("Action (-> Sp)", null);
        boolean[] smsEnabledFlags = {false, false};
        tagSmsEnabledMessages.setSelectedFlags(smsEnabledFlags);
        tagSmsNumber = new TextField("SMS Recipient", "+1234", 255, TextField.PHONENUMBER);
        tagSmsBody = new TextField("SMS Body", "Hello", 255, TextField.ANY);

        // Geo Uri
        tagLatitude = new TextField("Latitude (dec deg., WGS-84)", "60.17", 255, TextField.DECIMAL);
        tagLongitude = new TextField("Longitude (dec deg., WGS-84)", "24.829", 255, TextField.DECIMAL);
        tagGeoType = new ChoiceGroup("Choose Geo tag type", ChoiceGroup.EXCLUSIVE);
        tagGeoType.append("geo: URI scheme", null); // http://geouri.org/
        tagGeoType.append("Nokia Maps link", null);
        tagGeoType.setSelectedIndex(0, true);
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
            setupFormHeader(operationMode, newOperationMode);
            
            // The new operation mode is writing tags (previously, something
            // different was active): make the input items visible for
            // entering the tag type specific info.
            switch (newOperationMode) {
                case WRITE_SP_TAG:
                    form.append(posterEnabledMessages);
                    form.append(tagText);
                    form.append(tagUrl);
                    form.append(posterAction);
                    form.append(tagChooseImage);
                    break;
                case WRITE_URI_TAG:
                    form.append(tagUrl);
                    break;
                case WRITE_TEXT_TAG:
                    form.append(tagText);
                    form.append(tagTextLanguage);
                    break;
                case WRITE_SMS_TAG:
                    form.append(tagSmsEnabledMessages);
                    form.append(tagSmsNumber);
                    form.append(tagSmsBody);
                    form.append(tagText);
                    form.append(posterAction);
                    break;
                case WRITE_ANNOTATED_URL_TAG:
                    form.append(tagText);
                    form.append(tagUrl);
                    break;
                case WRITE_IMAGE_TAG:
                    form.append(tagChooseImage);
                    break;
                case WRITE_GEO_TAG:
                    form.append(tagLatitude);
                    form.append(tagLongitude);
                    form.append(tagGeoType);
                    break;
                case WRITE_CUSTOM_TAG:
                    form.append(tagTypeUri);
                    form.append(tagCustomPayload);
                    break;
                case WRITE_COMBINATION_TAG:
                    form.append("First Record (Custom)");
                    form.append(tagTypeUri);
                    form.append(tagCustomPayload);
                    form.append("Second Record (URI)");
                    form.append(tagUrl);
                    break;
            }
            operationMode = newOperationMode;
        }
    }

    /**
     * Callback from the NFC Manager when a tag was found. The NFC manager will then
     * establish a new thread and call this method, so that commands can be issued
     * to the tag.
     */
    public void tagReady() {
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
                    nfcManager.readNDEFMessage();
                    break;
                case WRITE_SP_TAG: {
                    boolean writeMessages[] = new boolean[posterEnabledMessages.size()];
                    posterEnabledMessages.getSelectedFlags(writeMessages);
                    nfcManager.writeSmartPoster(writeMessages, tagUrl.getString(), tagText.getString(), (byte) posterAction.getSelectedIndex(), getSelectedImageName());
                    break;
                }
                case WRITE_IMAGE_TAG:
                    nfcManager.writeImage(getSelectedImageName());
                    break;
                case WRITE_URI_TAG:
                    nfcManager.writeUri(tagUrl.getString());
                    break;
                case WRITE_TEXT_TAG:
                    nfcManager.writeText(tagText.getString(), tagTextLanguage.getString());
                    break;
                case WRITE_SMS_TAG: {
                    boolean writeMessages[] = new boolean[tagSmsEnabledMessages.size()];
                    tagSmsEnabledMessages.getSelectedFlags(writeMessages);
                    nfcManager.writeSms(writeMessages, tagSmsNumber.getString(), tagSmsBody.getString(), tagText.getString(), (byte) posterAction.getSelectedIndex());
                    break;
                }
                case WRITE_ANNOTATED_URL_TAG:
                    nfcManager.writeAnnotatedUrl(tagUrl.getString(), tagText.getString());
                    break;
                case WRITE_GEO_TAG:
                    nfcManager.writeGeo(Double.parseDouble(tagLatitude.getString()), Double.parseDouble(tagLongitude.getString()), tagGeoType.getSelectedIndex());
                    break;
                case WRITE_CUSTOM_TAG:
                    nfcManager.writeCustom(tagTypeUri.getString(), tagCustomPayload.getString().getBytes("utf-8"));
                    break;
                case WRITE_COMBINATION_TAG:
                    nfcManager.writeCombination(tagUrl.getString(), tagTypeUri.getString(), tagCustomPayload.getString().getBytes("utf-8"));
                    break;
                case DELETE_TAG:
                    nfcManager.deleteNDEFMessage();
                    break;
                default:
                    displayAlert("Illegal Action", "", AlertType.ERROR);
                    break;
            }

        } catch (IOException ex) {
            displayAlert("IOException", "Error loading image" + ex.toString(), AlertType.ERROR);
        } 

    }
    
    /**
     * Setup the UI for the new operation mode. Will show the correct selection
     * UI element and the according instructions. All other UI elements are cleared
     * from the screen.
     * @param oldOperationMode operation mode that was active before.
     * @param newOperationMode operation mode to be activated.
     */
    private void setupFormHeader(final int oldOperationMode, final int newOperationMode) {

        if (!isWriteOperationMode(oldOperationMode) && 
                isWriteOperationMode(newOperationMode)) {
            // Switched from a read/delete mode to a write mode:
            // Make operation mode selector small and add instructions
            form.deleteAll();
            operationModeSelector = new ChoiceGroup("Choose Mode", ChoiceGroup.POPUP, operatingModeNames, null);
            operationModeSelector.setSelectedIndex(newOperationMode, true);
            form.append(operationModeSelector);
            StringItem instructions = new StringItem("Touch a tag to write the currently visible settings", null);
            form.append(instructions);
        } else if (isWriteOperationMode(oldOperationMode) &&
                isWriteOperationMode(newOperationMode)) {
            // Was write before and is write now - leave first two elements
            // Clean up UI elements - everything except the first two elements
            // (-> which is the operation mode choice group and the instructions)
            resetFormExceptFirstX(2);
        } else if (isWriteOperationMode(oldOperationMode) &&
                !isWriteOperationMode(newOperationMode)) {
            // Switched from write operation mode to read/delete
            // Make selector big again
            form.deleteAll();
            operationModeSelector = new ChoiceGroup("Choose Mode", ChoiceGroup.EXCLUSIVE, operatingModeNames, null);
            operationModeSelector.setSelectedIndex(newOperationMode, true);
            form.append(operationModeSelector);
        }
        if (!isWriteOperationMode(newOperationMode)) {
            // Make sure the correct instructions are set for read / delete
            resetFormExceptFirstX(1);
            if (newOperationMode == READ_TAG) {
                StringItem instructions = new StringItem("Touch a tag to read its contents", null);
                form.append(instructions);
            } else if (newOperationMode == DELETE_TAG) {
                StringItem instructions = new StringItem("Touch a tag to delete its contents (-> write an empty NDEF message)", null);
                form.append(instructions);
            }
        }
    }
    
    /**
     * Clear all elements of the Form UI-element except the first few.
     * @param numRemainingElements how many UI elements should remain from the
     * beginning of the UI.
     */
    private void resetFormExceptFirstX(final int numRemainingElements) {
        final int numChoiceElements = form.size();
        if (numChoiceElements > numRemainingElements) {
            for (int i = numChoiceElements - 1; i > numRemainingElements - 1; i--) {
                form.delete(i);
            }
        }
    }
    
    /**
     * Check if the specified operating mode is one that would write a tag 
     * (-> true) or one that reads / deletes a tag (-> false).
     * @param operationMode operation mode to check
     * @return true if the operation mode will write user-specified data to the
     * tag, false if it reads / deletes the tag.
     */
    private boolean isWriteOperationMode(final int operationMode) {
        return !(operationMode == READ_TAG || operationMode == DELETE_TAG);
    }
    
    private String getSelectedImageName() {
        switch (tagChooseImage.getSelectedIndex()) {
            case 0:
                return "/minimal.gif";
            case 1:
                return "/minimal.png";
            default:
                return "/nokia.png";
        }
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
     * Callback from the NFC Manager when reading / writing to a text was
     * not successful.
     * @param text Text to show in a message box if desired.
     */
    public void tagError(final String text) {
        displayAlert(form.getTitle(), text, AlertType.ERROR);
    }

    /**
     * Callback from the NFC Manager when writing to a tag was successful.
     * @param text Text to show in a message box if desired.
     */
    public void tagSuccess(final String text) {
        displayAlert(form.getTitle(), text, AlertType.CONFIRMATION);
    }

    /**
     * Utility function to show a Java ME alert, as used for informing the user
     * about events in this demo app.
     * @param title title text to use for the message box.
     * @param text text to show as the main message in the box.
     * @param type one of the available alert types, defining the icon, sound
     * and display length.
     */
    public void displayAlert(final String title, final String text, final AlertType type) {
        Alert al = new Alert(title, text, null, type);
        Display.getDisplay(this).setCurrent(al, form);
    }
}
