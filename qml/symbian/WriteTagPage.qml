import QtQuick 1.1
import com.nokia.symbian 1.1 // Symbian Qt Quick components

Page {
    tools: ToolBarLayout {
        ToolButton {
            flat: true
            iconSource: "toolbar-back";
            onClicked: {
                nfcInfo.nfcStopWritingTags();
                pageStack.depth <= 1 ? Qt.quit() : pageStack.pop();
            }
        }
    }

    function resetPage() {
        writeStatusAnimation.stop()
        writeStatusBgFlash.stop()
        writeStatusContainer.visible = false;
        writeStatusBg.visible = false;
        separator.visible = false;
    }

    function startAnimation() {
        writeStatusContainer.visible = true;
        writeStatusAnimation.restart();
        writeStatusBg.visible = true;
        writeStatusBgFlash.restart();
        separator.visible = true;
    }

    function tagWritten() {
        writeStatusText.text = "Success";
        writeStatusText.color = "palegreen";
        writeStatusDetails.text = "Message written to the tag.\nTouch another tag to write again, or go back to the edit page.";
        writeStatusImage.source = "okLarge.svg";
        writeStatusBg.gradient = writeSuccessGradient;
        separator.gradient = writeSuccessGradient;
        startAnimation();
    }

    function tagWriteError(errorMsg) {
        writeStatusText.text = "Failed to write tag";
        writeStatusText.color = "lightcoral";
        writeStatusDetails.text = errorMsg;
        writeStatusImage.source = "errorLarge.svg";
        writeStatusBg.gradient = writeFailureGradient;
        separator.gradient = writeFailureGradient;
        startAnimation();
    }

    Gradient {
        id: writeSuccessGradient
        GradientStop {
            position: 0.00;
            color: "#1b3e11";
        }
        GradientStop {
            position: 1.00;
            color: "#4baa29";
        }
    }
    Gradient {
        id: writeFailureGradient
        GradientStop {
            position: 0.00;
            color: "#3e1111";
        }
        GradientStop {
            position: 1.00;
            color: "#aa2828";
        }
    }

    Rectangle {
        id: writeStatusBg
        anchors.fill: parent
        opacity: 0
        SequentialAnimation {
            id: writeStatusBgFlash
            PropertyAnimation {
                target: writeStatusBg
                property: "opacity"
                from: 0
                to: 100
                duration: 500
                easing.type: Easing.InQuad
            }
            PropertyAnimation {
                target: writeStatusBg
                property: "opacity"
                from: 100
                to: 0
                duration: 1000
                easing.type: Easing.OutQuad
            }
        }
    }


    // -------------------------------------------------------------------------
    // Instructions

    Image {
        id: writeInstructionsImage
        source: "nfcTouchTag.svg"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: platformStyle.paddingMedium
    }
    Text {
        id: writeInstructions
        text: "Please touch a tag to write the NDEF message"
        //horizontalAlignment: Text.AlignHCenter
        font.pixelSize: platformStyle.fontSizeLarge
        color: platformStyle.colorNormalLight
        anchors.top: parent.top
        anchors.topMargin: platformStyle.paddingLarge
        anchors.left: writeInstructionsImage.right
        anchors.leftMargin: platformStyle.paddingLarge
        anchors.right: parent.right
        anchors.rightMargin: platformStyle.paddingMedium
        wrapMode: Text.WordWrap
    }


    Rectangle {
        id: separator
        width: parent.width; height: 10;
        gradient: writeSuccessGradient
        opacity: 0.3
        anchors.top: writeInstructionsImage.bottom
        visible: false
    }
    // -------------------------------------------------------------------------
    // Status

    Item {
        id: writeStatusContainer
        anchors.top: separator.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: platformStyle.paddingLarge
        anchors.rightMargin: platformStyle.paddingLarge
        visible: false

        Flickable {
            id: writeStatusFlickable
            width: parent.width
            anchors.fill: parent
            contentWidth: parent.width
            contentHeight: writeStatusGroup.height
            flickableDirection: Flickable.VerticalFlick
            clip: true

            Column {
                id: writeStatusGroup
                width: parent.width
                spacing: platformStyle.paddingSmall

                PropertyAnimation {
                    id: writeStatusAnimation
                    target: writeStatusGroup
                    property: "opacity"
                    from: 0
                    to: 100
                    duration: 2000
                    easing.type: Easing.InOutQuad
                }
                Text {
                    id: writeStatusText
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: platformStyle.fontSizeLarge
                    color: platformStyle.colorNormalLight
                    height: platformStyle.fontSizeLarge + platformStyle.paddingLarge
                    verticalAlignment: Text.AlignBottom
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    id: writeStatusDetails
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: platformStyle.fontSizeMedium
                    color: platformStyle.colorNormalLight
                    width: parent.width
                    wrapMode: Text.WordWrap
                }

                Image {
                    id: writeStatusImage
                    source: "okLarge.svg"
                    anchors.horizontalCenter: parent.horizontalCenter
                    fillMode: Image.PreserveAspectFit
                }


            }

        }
    }


}
