/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Andreas Jakl (andreas.jakl@nokia.com)
**
** This file is part of an NFC example for Qt Mobility.
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

import QtQuick 1.1
import com.nokia.meego 1.0

import Nfc 1.0
import com.nfcinfo.types 1.0

PageStackWindow {
    id: window
    initialPage: nfcInfoPage
    showStatusBar: true
    showToolBar: true
    //platformSoftwareInputPanelEnabled: true

    NfcInfoPage {
        id: nfcInfoPage
    }

    ComposeTagPage {
        id: composeTagPage
    }

    WriteTagPage {
        id: writeTagPage
    }

    InstructionsPage {
        id: instructionsPage
    }

    NfcInfo {
        id: nfcInfo
        objectName: "nfcInfoObj"

        // Initialize and start NFC target detection
        function initNfc()
        {
            // Check if NFC is available
            nfcInfo.checkNfcStatus();
            nfcInfo.initAndStartNfc();
        }

        Component.onCompleted: initNfc()

        // Add a message to the model
        function logMessage(text, color, img)
        {
            nfcInfoPage.logMessage(text, color, img)
        }

        // Add an image to the model (message with empty text but custom image)
        function logImage(imgId)
        {
            nfcInfoPage.logMessage("", "white", ("image://nfcimageprovider/"+imgId))
            //nfcInfoPage.messageModel.append( {"infoMsg": "", "textColor": "white", "image": ("image://nfcimageprovider/"+imgId)} )
        }

        onNfcStatusUpdate:
            logMessage(nfcStatusText, "aliceblue", "nfcSymbolInfo.png");
        onNfcStatusSuccess:
            logMessage(nfcStatusSuccessText, "aliceblue", "nfcSymbolSuccess.png");
        onNfcStatusError:
            logMessage(nfcStatusErrorText, "coral", "nfcSymbolError.png");
        onNfcInfoUpdate:
            logMessage(nfcInfoText, "aquamarine", "nfcSymbolTag.png");
        onNfcTagContents:
            logMessage(nfcTagContents, "lightskyblue", "nfcSymbolTag.png");
        onNfcTagImage:
            logImage(nfcImgId);
        onNfcTagError:
            logMessage(nfcTagError, "coral", "nfcSymbolError.png");

        onNfcStartingTagInteraction: {
            if (writeTagPage.status === PageStatus.Active) {
                // Active page is writing page - start busy animation
                writeTagPage.startWriting();
            } else if (nfcInfoPage.status === PageStatus.Active) {
                nfcInfoPage.showHideBusy(true);
            }
        }

        onNfcStoppedTagInteraction: {
            // Hide the busy animation in any case - the user might have
            // switched away from the page while tag reading was active.
            nfcInfoPage.showHideBusy(false);
            writeTagPage.cancelWriting();
        }

        onNfcTagWritten: {
            logMessage("Message written to the tag.", "aliceblue", "nfcSymbolSuccess.png");
            writeTagPage.tagWritten();
        }
        onNfcTagWriteError: {
            logMessage(nfcTagError, "coral", "nfcSymbolError.png");
            writeTagPage.tagWriteError(nfcTagError);
        }
        onNfcModeChanged: {
            if (nfcNewMode === 0) {
                // Reading; 1 would be writing, but the NfcInfo class will
                // send out more detailed textual signals about the write status,
                // so we won't print a message here to keep the amount of on-screen text down.
                logMessage("Switched back to tag reading mode.", "aliceblue", "nfcSymbolInfo.png");
            }
        }

        onStoredMessageSizeChanged:
            composeTagPage.updateHeader(ndefMessageSize)
    }

    // MeeGo: set black theme
    Component.onCompleted: {
        theme.inverted = true
    }

//    Component.onCompleted: {
////        if (platform !== 0) {
////            // Only show the toolbar with the back/quit button on Symbian
////            toolBar.visible = false;
////        }
//        //pageStack.push(Qt.resolvedUrl("NfcInfoPage.qml"))
//        pageStack.push(nfcInfoPage)
//    }
}
