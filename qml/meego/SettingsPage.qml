/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Andreas Jakl (andreas.jakl@nokia.com)
**
** Released under Nokia Example Code License.
** See license.txt in the main project folder.
**
****************************************************************************/
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
    property alias deleteTagBeforeWriting: deleteTagBeforeWritingEdit.checked
    property alias useSnep: useSnepEdit.checked


    onStatusChanged: {
        if (status === PageStatus.Activating) {
            applySettingsToPage();
        }
    }

    function applySettingsToPage() {
        logNdefToFile = settings.logNdefToFile;
        logNdefDir = settings.logNdefDir;
        deleteTagBeforeWriting = settings.deleteTagBeforeWriting;
    }

    function saveSettings() {
        console.log("Saving settings...");
        // Apply the new settings to the nfcPeerToPeer object
        settings.setLogNdefToFile(logNdefToFile);
        settings.setLogNdefDir(logNdefDir);
        settings.setDeleteTagBeforeWriting(deleteTagBeforeWriting);
        settings.saveSettings();
    }


    // ----------------------------------------------------------
    // Settings


    Row {
        id: settingsHeader
        spacing: customPlatformStyle.paddingMedium;
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

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

    Rectangle {
        id: separator
        width: parent.width; height: 1; color: "gray"
        anchors.top: settingsHeader.bottom
    }


    Flickable {
        id: flickSettings

        clip: true
        contentHeight: settingsContentItem.height
        anchors.top: separator.bottom
        anchors.topMargin: customPlatformStyle.paddingMedium;
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Column {
            id: settingsContentItem
            width: parent.width

            spacing: 7

            // - Log NDEF messages to files
            Item {// 2-line text for checkbox not properly formated
                width: 1
                height: customPlatformStyle.paddingSmall;
            }
            CheckBox {
                id: logNdefToFileEdit
                checked: true
                text: "Save NDEF messages to files\n(required for cloning \& editing)"
                onClicked: {
                    logNdefDirEdit.enabled = logNdefToFileEdit.checked
                    logNdefDirTitle.color = (logNdefDirEdit.enabled) ? customPlatformStyle.colorNormalLight : customPlatformStyle.colorNormalMid;
                }
            }
            Item {// 2-line text for checkbox not properly formated
                width: 1
                height: customPlatformStyle.paddingSmall;
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

            // - Delete tag before writing
            Item {// 2-line text for checkbox not properly formated
                width: 1
                height: customPlatformStyle.paddingSmall;
            }
            CheckBox {
                id: deleteTagBeforeWritingEdit
                checked: true
                text: "Delete/format tag before writing\n(use for factory empty tags)"
            }
            Item {// 2-line text for checkbox not properly formated
                width: 1
                height: customPlatformStyle.paddingSmall;
            }

            // - SNEP
            CheckBox {
                id: useSnepEdit
                checked: true
                text: "Use SNEP for peer-to-peer"
            }
        }
    }
}
