import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    id: iaaAd

    height: iaaItem.height

    AdItem {
        id: iaaItem
        parameters: AdParameters {
            applicationId: "Mopius_NfcInteractor_OVI"
            usePositioning: false
        }
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        showText: false
    }

//    Connections {
//        target: adInterface
//        onNetworkNotAccessible:
//        {
//            quitDialog.open();
//        }
//        onNetworkAccessibilityChanged:
//        {
//            console.log("Network isOnline: " + adInterface.networkAccessible);
//        }
//    }

//    QueryDialog {
//        id: quitDialog
//        titleText: qsTr("No connectivity.")
//        message: qsTr("Please enable network connectivity and restart the app.\nIf you prefer to use the app in offline mode, purchase the Nfc Interactor Unlimited from the Nokia Store.")
//        acceptButtonText: "Quit"
//        onAccepted: Qt.quit()
//    }
}
