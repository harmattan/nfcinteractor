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
import com.nokia.symbian 1.1 // Symbian Qt Quick components

Page {
    tools: ToolBarLayout {
        ToolButton {
            flat: true
            iconSource: "toolbar-back";
            onClicked: {
                pageStack.depth <= 1 ? Qt.quit() : pageStack.pop()
            }
        }
    }

    Item {
        id: instructionsItem
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: customPlatformStyle.paddingLarge
        anchors.right: parent.right
        anchors.rightMargin: customPlatformStyle.paddingLarge
        anchors.bottom: splitViewInput.top

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
                    text: qsTr("<strong>Nfc Interactor</strong>") + (window.isAdvTagsPurchased() ? "\nUnlimited" : "");
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
                    text: qsTr("v4.0.0 beta\n2011 - 2012 Andreas Jakl")
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
                    onClicked: Qt.openUrlExternally("http://www.nfcinteractor.com/r?ni");
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
            }
        }

        ScrollBar {
            id: scrollBar
            height: parent.height
            anchors { top: instructionsFlickable.top; right: instructionsFlickable.right }
            flickableItem: instructionsFlickable
            interactive: false
            orientation: Qt.Vertical
        }

    }

    // --------------------------------------------------------------------------------
    // Virtual Keyboard handling (VKB)
    // Resizes the Listview to avoid overlapping the edited item with the VKB.
    // Only needed on Symbian, MeeGo does that by itself.
    Item {
        // This element has the same size as the virtual keyboard and is used to
        // reduce the size of the recordView whenever the VKB becomes visible.
        // This is needed to ensure the edited item is visible and not hidden
        // by the VKB (through overlap).
        id: splitViewInput
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
    }
    Connections {
        target: inputContext;
        onVisibleChanged: adjustVkbHeight();
        onHeightChanged: adjustVkbHeight();
    }
    function adjustVkbHeight() {
        if (platform !== 2) {
            var prevSplitViewInputHeight = splitViewInput.height;
            // Only do this when not working with the Simulator, which doesn't draw the
            // VKB but still reserves the size for it.
            splitViewInput.height = (inputContext.visible) ? inputContext.height - tools.height : 0;
            if (prevSplitViewInputHeight !== splitViewInput.height) {
                // Note that the recordView.currentIndex requires the item to set itself
                // as the current index when the user starts editing an item.
                // (in onActiveFocusChanged in the TextField element of the delegate)
                //recordView.positionViewAtIndex(recordView.currentIndex, ListView.Visible);
                flickSettings.contentY = focusedItem.y + focusedItem.height
                flickSettings.returnToBounds();
            }
        }
    }
}
