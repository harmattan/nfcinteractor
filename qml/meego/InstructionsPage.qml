import QtQuick 1.1
//import com.nokia.symbian 1.1 // Symbian Qt Quick components
import com.nokia.meego 1.0

Page {
    tools: ToolBarLayout {
        ToolIcon {
            //iconSource: "toolbar-back";
            iconId: "toolbar-back"
            onClicked: pageStack.depth <= 1 ? Qt.quit() : pageStack.pop()
        }
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
                    text: qsTr("<strong>Nfc Interactor Professional</strong>")
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    wrapMode: Text.WordWrap
                    //color: customPlatformStyle.colorNormalLight
                    color: "aliceblue";
                    font.pixelSize: customPlatformStyle.fontSizeLarge
                }
                Image {
                    id: instructionsImage
                    anchors.horizontalCenter: parent.horizontalCenter
                    fillMode: Image.PreserveAspectFit
                    source: "icon.png"
                }

                Text {
                    id: instructionsText2
                    text: qsTr("v2.0.0 beta\n2011 - 2012 Andreas Jakl\nNfcInteractor.com\n\nTouch NFC tags to analyze their contents.\nTap the 'write' toolbar button to compose your own NDEF message (consisting of one or more records), then tap the finish flag to finalize the contents. Now touch a tag to write the message to the tag.")
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    wrapMode: Text.WordWrap
                    color: customPlatformStyle.colorNormalLight
                    font.pixelSize: customPlatformStyle.fontSizeMedium
                }
            }

        }

        // TODO: MeeGo
//        ScrollBar {
//            id: scrollBar
//            height: parent.height
//            anchors { top: instructionsFlickable.top; right: instructionsFlickable.right }
//            flickableItem: instructionsFlickable
//            interactive: false
//            orientation: Qt.Vertical
//        }

    }
}