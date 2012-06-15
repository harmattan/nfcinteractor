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
//        message: qsTr("Please enable network connectivity and restart the app.\nIf you prefer to use the app in offline mode, purchase the 'Remove Ads' item from within the application.")
//        acceptButtonText: "Quit"
//        onAccepted: Qt.quit()
//    }
}
