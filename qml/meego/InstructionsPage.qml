import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: {
                saveSettings();
                pageStack.depth <= 1 ? Qt.quit() : pageStack.pop()
            }
        }
    }

    // Internal use - for virtual keyboard handling
    property Item focusedItem

    // Settings
    property alias logNdefToFile: logNdefToFileEdit.checked
    property alias logNdefDir: logNdefDirEdit.text

    onStatusChanged: {
        if (status === PageStatus.Activating) {
            console.log("Activating page...");
            applySettingsToPage();
        }
    }

    function applySettingsToPage() {
        logNdefToFile = settings.logNdefToFile;
        logNdefDir = settings.logNdefDir;
    }

    function saveSettings() {
        console.log("Saving settings...");
        // Apply the new settings to the nfcPeerToPeer object
        settings.setLogNdefToFile(logNdefToFile);
        settings.setLogNdefDir(logNdefDir);
        settings.saveSettings();
    }


    Item {
        id: instructionsItem
        anchors.fill: parent
        anchors.leftMargin: customPlatformStyle.paddingLarge
        anchors.rightMargin: customPlatformStyle.paddingLarge

        Flickable {
            id: instructionsFlickable
            width: parent.width
            height: parent.height
            anchors { left: parent.left; top: parent.top }
            contentHeight: instructionsContentItem.height
            flickableDirection: Flickable.VerticalFlick

            Column {
                id: instructionsContentItem
                width: parent.width

                spacing: 7

                Text {
                    id: instructionsText1
                    text: qsTr("<strong>Nfc Interactor</strong>") + (useIap ? "" : "\nUnlimited");
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    wrapMode: Text.WordWrap
                    //color: customPlatformStyle.colorNormalLight
                    color: "aliceblue";
                    font.family: customPlatformStyle.fontFamilyRegular;
                    font.pixelSize: customPlatformStyle.fontSizeLarge
                }
                Image {
                    id: instructionsImage
                    anchors.horizontalCenter: parent.horizontalCenter
                    fillMode: Image.PreserveAspectFit
                    source: "icon.png"
                    asynchronous: true
                }

                Text {
                    id: instructionsText2
                    text: qsTr("v3.0.0 beta\n2011 - 2012 Andreas Jakl")
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    wrapMode: Text.WordWrap
                    color: customPlatformStyle.colorNormalLight
                    font.family: customPlatformStyle.fontFamilyRegular;
                    font.pixelSize: customPlatformStyle.fontSizeMedium
                }
                Button {
                    id: instructionsLinkButton
                    text: "NfcInteractor.com"
                    onClicked: Qt.openUrlExternally(((useIap) ? "http://www.nfcinteractor.com/r?ni" : "http://www.nfcinteractor.com/r?niu"));
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    id: instructionsText3
                    text: qsTr("Touch NFC tags to analyze their contents.\nTap the 'write' toolbar button to compose your own NDEF message (consisting of one or more records), then tap the finish flag to finalize the contents. Now touch a tag to write the message to the tag.\nTap the read tag contents (blue text) to clone or edit a tag.")
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    wrapMode: Text.WordWrap
                    color: customPlatformStyle.colorNormalLight
                    font.family: customPlatformStyle.fontFamilyRegular;
                    font.pixelSize: customPlatformStyle.fontSizeMedium
                }

                Text {
                    id: instructionsText4
                    text: qsTr("Upgrade to the Nfc Interactor Unlimited to write more than 10 advanced tags and enjoy usage without ads (no network connection required)! Click the button above for more information.")
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    visible: useIap
                    wrapMode: Text.WordWrap
                    color: customPlatformStyle.colorNormalLight
                    font.family: customPlatformStyle.fontFamilyRegular;
                    font.pixelSize: customPlatformStyle.fontSizeMedium
                }

                // ----------------------------------------------------------
                // Settings

                Rectangle {
                    id: separator
                    width: parent.width; height: 1; color: "gray"
                }
                Row {
                    id: settingsHeader
                    spacing: customPlatformStyle.paddingMedium;

                    Image {
                        id: settingsImage
                        source: "settings.svg"
                        fillMode: Image.PreserveAspectFit
                        asynchronous: true
                    }

                    Text {
                        id: settingsHeaderText
                        text: qsTr("Settings")
                        verticalAlignment: Text.AlignVCenter
                        height: settingsImage.height
                        font.family: customPlatformStyle.fontFamilyRegular;
                        color: customPlatformStyle.colorNormalLight
                        font.pixelSize: customPlatformStyle.fontSizeLarge
                    }
                }

                // - Log NDEF messages to files
                CheckBox {
                    id: logNdefToFileEdit
                    checked: true
                    text: "Save NDEF messages to files\n(required for cloning \& editing)"
                    onClicked: {
                        logNdefDirEdit.enabled = logNdefToFileEdit.checked
                        logNdefDirTitle.color = (logNdefDirEdit.enabled) ? customPlatformStyle.colorNormalLight : customPlatformStyle.colorNormalMid;
                    }
                }

                // - Log directory
                Text {
                    id: logNdefDirTitle
                    text: qsTr("Saved NDEF directory")
                    font.family: customPlatformStyle.fontFamilyRegular;
                    color: customPlatformStyle.colorNormalLight
                    font.pixelSize: customPlatformStyle.fontSizeMedium
                }

                TextField {
                    id: logNdefDirEdit
                    width: parent.width
                    text: ""
                    maximumLength: 255
                    onActiveFocusChanged: if (activeFocus) focusedItem = logNdefDirEdit
                }
            }
        }

        ScrollDecorator {
            flickableItem: instructionsFlickable
        }
    }
}
