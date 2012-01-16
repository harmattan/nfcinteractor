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

#include "nfcinfo.h"

NfcInfo::NfcInfo(QObject *parent) :
    QObject(parent),
    m_reportingLevel(OnlyImportantReporting),
    m_pendingWriteNdef(false),
    m_currentActivity(NfcUninitialized),
    m_writeOneTagOnly(true),
    m_cachedNdefMessage(NULL),
    m_cachedNdefMessageSize(0),
    m_cachedRequestType(NfcIdle)
{
    m_nfcRecordModel = new NfcRecordModel(this);
    connect(m_nfcRecordModel, SIGNAL(recordItemsModified()), this, SLOT(nfcRecordModelChanged()));
    m_nfcTargetAnalyzer = new NfcTargetAnalyzer(this);
    m_nfcNdefParser = new NfcNdefParser(this);
    // Relay the signal when the private ndef parser found an image,
    // so that the QML UI can react to this.
    // Images are stored in the m_imgCache variable; both this and
    // the m_nfcNdefParser have a reference to it.
    connect(m_nfcNdefParser, SIGNAL(nfcTagImage(int)), this, SIGNAL(nfcTagImage(int)));
}

NfcInfo::~NfcInfo() {
    delete m_cachedNdefMessage;
}

/*!
  \brief Initialize the NFC access for NDEF targets and start target detection.

  This method emits nfcStatusUpdate signals containing the textual results of
  the operation.

  \return true if starting target detection was successful.
  */
bool NfcInfo::initAndStartNfc()
{
    if (m_nfcRecordModel->size() == 0) {
        nfcRecordModelChanged();
        // Populate write view with items (for development, TODO: comment out for release)
//        nfcRecordModel->addRecordItem(new NfcRecordItem("Smart Poster", NfcTypes::MsgSmartPoster, NfcTypes::RecordHeader, "", true, true, 1));
//        nfcRecordModel->addRecordItem(new NfcRecordItem("URI", NfcTypes::MsgSmartPoster, NfcTypes::RecordUri, "http://www.nokia.com/", false, false, 1));
    }

    // NdefManager (this) is the parent; will automaically delete nfcManager
    if (!m_nfcManager) {
        m_nfcManager = new QNearFieldManager(this);
    }

    const bool nfcAvailable = m_nfcManager->isAvailable();
    if (nfcAvailable) {
        emit nfcStatusUpdate("QtM reports: NFC is available");
    } else {
        emit nfcStatusError("QtM reports: NFC is not available");
    }

    // MeeGo Harmattan PR 1.0 only allows one target access mode to be active at the same time
//#ifdef MEEGO_EDITION_HARMATTAN
//    nfcManager->setTargetAccessModes(QNearFieldManager::NdefReadTargetAccess);
//#else
    m_nfcManager->setTargetAccessModes(QNearFieldManager::NdefReadTargetAccess | QNearFieldManager::NdefWriteTargetAccess | QNearFieldManager::TagTypeSpecificTargetAccess);
//#endif

    // Required for autostart tags
    m_nfcManager->registerNdefMessageHandler(this, SLOT(targetMessageDetected(QNdefMessage,QNearFieldTarget*)));

    // Get notified when the tag gets out of range
    connect(m_nfcManager, SIGNAL(targetLost(QNearFieldTarget*)),
            this, SLOT(targetLost(QNearFieldTarget*)));
    connect(m_nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)),
            this, SLOT(targetDetected(QNearFieldTarget*)));

    // Start detecting targets
    bool activationSuccessful = m_nfcManager->startTargetDetection();
    if (activationSuccessful) {
        emit nfcStatusUpdate("Successfully started target detection");
        return true;
    } else {
        emit nfcStatusError("Error starting NFC target detection");
        return false;
    }

    m_currentActivity = NfcIdle;
}


NfcRecordModel * NfcInfo::recordModel() const
{
    return m_nfcRecordModel;
}


void NfcInfo::nfcRecordModelChanged()
{
    // Calculate new message size
    QNdefMessage* message = recordModel()->convertToNdefMessage();
    QByteArray rawMessage = message->toByteArray();
    const int bytesize = rawMessage.size();
    delete message;
    emit storedMessageSizeChanged(bytesize);
}

/*!
  \brief Check if NFC is supported by the device and if it
  is activated.

  Note: currently, this is only implemented for Symbian using
  nfcsettings component (which uses native Symbian code to query
  the NFC status).

  \return true if and only if NFC is available and activated on Symbian.
  The nfcStatusError / nfcStatusUpdate signals contain more information
  about the actual status (e.g., if the device would support NFC but
  the user needs to activate it).
  */
bool NfcInfo::checkNfcStatus()
{
#ifdef Q_OS_SYMBIAN
    // Construct a new instance.
    nfcSettings = new NfcSettings(this);

    // Retrieve the NFC feature support information.
    NfcSettings::NfcFeature nfcFeature = nfcSettings->nfcFeature();

    if (nfcFeature == NfcSettings::NfcFeatureSupported) {
        // Connect signals for receiving mode change and error notifications.
        connect(nfcSettings, SIGNAL(nfcModeChanged(NfcSettings::NfcMode)), SLOT(handleNfcModeChange(NfcSettings::NfcMode)));
        connect(nfcSettings, SIGNAL(nfcErrorOccurred(NfcSettings::NfcError, int)), SLOT(handleNfcError(NfcSettings::NfcError, int)));

        // Retrieve the initial value of the NFC mode setting.
        NfcSettings::NfcMode nfcMode = nfcSettings->nfcMode();

        if (nfcMode != NfcSettings::NfcModeOn) {
            // NFC is supported but not switched on, prompt the user to enable it.
            emit nfcStatusError(tr("NFC hardware is available but currently switched off."));
            return false;
        } else {
            emit nfcStatusUpdate(tr("NFC is supported and switched on."));
            return true;
        }
    }
    else if (nfcFeature == NfcSettings::NfcFeatureSupportedViaFirmwareUpdate) {
        // Display message to user to update device firmware
        emit nfcStatusError(tr("Update device firmware to enable NFC support."));
        return false;
    } else {
        // Display message informing the user that NFC is not supported by this device.
        emit nfcStatusError(tr("NFC not supported by this device."));
        return false;
    }
#endif
    return false;
}

/*!
  Set the image cache to use for storing images retrieved
  from tags.
  \a tagImageCache instance of the image cache. This
  class will not take ownership of the instance!
  */
void NfcInfo::setImageCache(TagImageCache *tagImageCache)
{
    m_nfcNdefParser->setImageCache(tagImageCache);
}

/*!
  \brief Called by the NearFieldManager whenever it finds a target.

  This method will create connections between the target and this class
  to be informed about its status. It also attempts to analyze the target
  and emits information through nfcStatusUpdate signals. If a write operation
  is pending, it will be written to the tag. Otherwise, the tag contents will be
  read (if possible).
  */
void NfcInfo::targetDetected(QNearFieldTarget *target)
{
    // Handle potential errors emitted by the target
    connect(target, SIGNAL(error(QNearFieldTarget::Error,QNearFieldTarget::RequestId)),
            this, SLOT(targetError(QNearFieldTarget::Error,QNearFieldTarget::RequestId)));
    connect(target, SIGNAL(requestCompleted (const QNearFieldTarget::RequestId)),
            this, SLOT(requestCompleted(QNearFieldTarget::RequestId)));
    connect(target, SIGNAL(ndefMessagesWritten()),
            this, SLOT(ndefMessageWritten()));

    m_currentActivity = NfcTargetAnalysis;
    // Cache the target in any case for future writing
    // (so that we can also write on tags that are empty as of now)
    m_cachedTarget = target;

    startedTagInteraction();
    // Check if the target includes a NDEF message
    const bool targetHasNdefMessage = target->hasNdefMessage();
    if (targetHasNdefMessage)
    {
        emit nfcStatusUpdate("NDEF target detected");
    } else {
        emit nfcStatusUpdate("Target detected");
    }

    // Analyze the target and send the info to the UI
    emit nfcInfoUpdate(m_nfcTargetAnalyzer->analyzeTarget(target));
    m_currentActivity = NfcIdle;

    // Check if we have NDEF access and can read or write to the tag
    QNearFieldTarget::AccessMethods accessMethods = target->accessMethods();

    if (accessMethods.testFlag(QNearFieldTarget::NdefAccess))
    {
        // Is a write operation pending?
        if (!m_pendingWriteNdef)
        {
            // NO write operation pending, so read the tag if possible
            // If target has an NDEF message...
            if (targetHasNdefMessage)
            {
                m_currentActivity = NfcNdefReading;
                m_cachedRequestType = NfcNdefReading;
                // Target has NDEF messages: read them (asynchronous)
                connect(target, SIGNAL(ndefMessageRead(QNdefMessage)),
                        this, SLOT(ndefMessageRead(QNdefMessage)));
                m_cachedRequestId = target->readNdefMessages();
            }
        }
        else
        {
            // Write operation is pending, so attempt writing the message.
            //#ifdef MEEGO_EDITION_HARMATTAN
            //            nfcManager->setTargetAccessModes(QNearFieldManager::NdefWriteTargetAccess);
            //#endif
            // Write a cached NDEF message to the tag
            writeCachedNdefMessage();
        }
    } else {
        // No NDEF access - no further interaction
        qDebug() << "No NDEF access to the target";
        stoppedTagInteraction();
    }

}


void NfcInfo::targetMessageDetected(const QNdefMessage &message, QNearFieldTarget* target)
{
    // TODO: When this method is called through autostart, it seems to crash the app
    // Check if target is set!
    // If it isn't set -> bug report
    qDebug() << "NDEF Message detected!";
    emit nfcStatusUpdate("NDEF Message detected");
    if (target) {
        // Analyze the target and send the info to the UI
        emit nfcInfoUpdate(m_nfcTargetAnalyzer->analyzeTarget(target));
    }
    // Go through all records in the message
    ndefMessageRead(message);
#ifdef MEEGO_EDITION_HARMATTAN
    // MeeGo: raise the app to the foreground in case it was autostarted by touching the tag
    // AND it was already running in the background.
    // If we wouldn't do it, the app would receive the tag, but remain in the background.

    // TODO: pass declarative view to this class, like in nfc corkboard app.

    //if (declarativeView) {
    //    declarativeView->raise();
    //}
#endif
}


/*!
  \brief Emits the nfcTagContents containing a textual description of the
  contents of the NDEF message.

  In case pictures are found, these are added to the image cache and the
  nfcTagImage signal is emitted together with the image ID.
  \param message the NDEF message to analyze.
  */
void NfcInfo::ndefMessageRead(const QNdefMessage &message)
{
    emit nfcTagContents(m_nfcNdefParser->parseNdefMessage(message));
    stoppedTagInteraction();
}

/*!
  \brief Create the message for writing to the tag and attempt
  to write it.

  This method will be extended in future releases of this app.

  \return if it was already possible to write to the tag. If
  false is returned, the message is cached and will be written
  when a writable target is available. Only one message is cached;
  if this method is called a second time before the first message
  is actually written to a tag, the old message will be discarded
  and only the later one written to the tag.
  */
bool NfcInfo::nfcWriteTag(const bool writeOneTagOnly)
{
    qDebug() << "Write tag ... ";
    emit nfcModeChanged(NfcTypes::nfcWriting);

    // Convert the model into a NDEF message
    QNdefMessage* message = recordModel()->convertToNdefMessage();

    QByteArray rawMessage = message->toByteArray();
    emit nfcStatusUpdate("Created message (size: " + QString::number(rawMessage.size()) + " bytes)");

    // Write the message (containing either a URL or plain text) to the target.
    if (m_cachedNdefMessage) { delete m_cachedNdefMessage; }
    m_cachedNdefMessage = message;
    m_cachedNdefMessageSize = m_cachedNdefMessage->toByteArray().size();
    m_pendingWriteNdef = true;
    m_writeOneTagOnly = writeOneTagOnly;
    return writeCachedNdefMessage();
}

void NfcInfo::nfcStopWritingTags()
{
    m_pendingWriteNdef = false;
    m_writeOneTagOnly = false;
    emit nfcModeChanged(NfcTypes::nfcReading);
}

/*!
  \brief Attempt to write the currently cached message to the tag.

  \return true if it was possible to send the request to the tag.
  */
bool NfcInfo::writeCachedNdefMessage()
{
    bool success = false;
    if (m_pendingWriteNdef && m_cachedNdefMessage)
    {
        if (m_cachedTarget)
        {
            // Check target access mode
            QNearFieldManager::TargetAccessModes accessModes = m_nfcManager->targetAccessModes();
            // Writing access is active - we should be able to write
            if (accessModes.testFlag(QNearFieldManager::NdefWriteTargetAccess))
            {
                qDebug() << "Writing message: " << m_cachedNdefMessage->toByteArray();
                m_currentActivity = NfcNdefWriting;
                m_cachedRequestType = NfcNdefWriting;
                m_cachedRequestId = m_cachedTarget->writeNdefMessages(QList<QNdefMessage>() << (*m_cachedNdefMessage));
                if (!m_writeOneTagOnly) {
                    // If writing only one tag, deactivate the writing mode again.
                    m_pendingWriteNdef = false;
                    emit nfcModeChanged(NfcTypes::nfcReading);
                    //#ifdef MEEGO_EDITION_HARMATTAN
                    //                nfcManager->setTargetAccessModes(QNearFieldManager::NdefReadTargetAccess);
                    //#endif
                }
                emit nfcStatusUpdate("Writing message to the tag.");
                success = true;
            } else {
                // Device is not in writing mode
                emit nfcStatusUpdate("Please touch the tag again to write the message.");
            }
        } else {
            // Can't write - no cached target available
            emit nfcStatusUpdate("Please touch a tag to write the message.");
        }
    }
    if (!success) {
        // Didn't start a request to write a message - finished interacting
        // with the tag
        stoppedTagInteraction();
    }
    return success;
}

void NfcInfo::startedTagInteraction() {
    if (!m_nfcTagInteractionActive) {
        m_nfcTagInteractionActive = true;
        emit nfcStartingTagInteraction();
        qDebug() << "*** Starting tag interaction...";
    }
}

void NfcInfo::stoppedTagInteraction() {
    if (m_nfcTagInteractionActive) {
        m_nfcTagInteractionActive = false;
        emit nfcStoppedTagInteraction();
        qDebug() << "*** Stopped tag interaction...";
    }
}

/*!
  \brief Slot for handling when the target was lost (usually when
  it gets out of range.
  */
void NfcInfo::targetLost(QNearFieldTarget *target)
{
    m_cachedTarget = NULL;
    target->deleteLater();
    stoppedTagInteraction();
    emit nfcStatusUpdate("Target lost");
}

/*!
  \brief Slot for handling an error with a request to the target.

  Emits the nfcTagError signal containing a description of the error.
  */
void NfcInfo::targetError(QNearFieldTarget::Error error, const QNearFieldTarget::RequestId &id)
{
    QString errorText("Error: " + convertTargetErrorToString(error));
    qDebug() << errorText;
    if (id == m_cachedRequestId && m_cachedRequestType == NfcNdefWriting) {
        m_cachedRequestType = NfcIdle;
        if (!m_pendingWriteNdef) {
            m_currentActivity = NfcIdle;
        }
        errorText.append("\n\nUnable to write message.");

        if (m_nfcTargetAnalyzer->m_tagInfo.combinedWriteAccess() == NearFieldTargetInfo::NfcAccessForbidden) {
            errorText.append("\nTag is write-protected.");
        } else if (m_nfcTargetAnalyzer->m_tagInfo.combinedWriteAccess() == NearFieldTargetInfo::NfcAccessUnknown) {
            errorText.append("\nTag write access unknown.");
        }
        // Compare tag size to message size
        const int tagWritableSize = m_nfcTargetAnalyzer->m_tagInfo.tagWritableSize;
        // Check if the app was successful in determining the tag size
        if (tagWritableSize > 0 && m_cachedNdefMessageSize > 0) {
            // Known tag size - we can do a proper check
            if (m_cachedNdefMessageSize > tagWritableSize) {
                // Message was too large for the target.
                errorText.append("\n\nMessage (" + QString::number(m_cachedNdefMessageSize) + " bytes) and control data were probably too large for the available tag size (" + QString::number(tagWritableSize) + " bytes).");
            }
        } else if (tagWritableSize == 0 && m_cachedNdefMessageSize > 0 && m_cachedTarget) {
            // Don't know the tag size - print a warning for typical tag sizes
            // that we have have to guess
            // This happens if the tag has issues, if Qt Mobility APIs
            // didn't allow reading the size (on Symbian Anna or Harmattan),
            // or if this app doesn't support reading the tag size.
            int memorySizeWarning;
            switch (m_cachedTarget->type()) {
            case QNearFieldTarget::NfcTagType2:
                memorySizeWarning = GUESS_TYPE2_SIZE;
                break;
            case QNearFieldTarget::NfcTagType3:
                memorySizeWarning = GUESS_TYPE3_SIZE;
                break;
            case QNearFieldTarget::NfcTagType4:
                memorySizeWarning = GUESS_TYPE4_SIZE;
                break;
            case QNearFieldTarget::MifareTag:
                memorySizeWarning = GUESS_MIFARE_SIZE;
                break;
            case QNearFieldTarget::NfcTagType1:
            default:
                memorySizeWarning = GUESS_TYPE1_SIZE;
                break;
            }
            if (m_cachedNdefMessageSize > memorySizeWarning) {
                errorText.append("\n\nMessage (" + QString::number(m_cachedNdefMessageSize) + " bytes) plus control data might be too large for the " + m_nfcTargetAnalyzer->convertTagTypeToString(m_cachedTarget->type()) + " target?");
            }
        }
        emit nfcTagWriteError(errorText);
        stoppedTagInteraction();
    } if (id == m_cachedRequestId && m_cachedRequestType == NfcNdefReading) {
        emit nfcTagError(errorText);
        stoppedTagInteraction();
    } else {
        if (m_reportingLevel == FullReporting ||
                error != QNearFieldTarget::InvalidParametersError) {
            emit nfcTagError(errorText);
        }
    }
}

/*!
  \brief Slot called by Qt Mobility when a request has been completed.

  This method emits the nfcStatusUpdate signal to log the event in the
  user interface. In case the request was of a kind that contains a
  response, information about the response will also be emitted through
  another signal of the same type. In case the response is a byte array,
  it will be printed as a string of hex characters.
  */
void NfcInfo::requestCompleted(const QNearFieldTarget::RequestId &id)
{
    QString message;
    bool showAnotherTagWriteMsg = false;
    if (id == m_cachedRequestId) {
        switch (m_cachedRequestType) {
        case NfcIdle:
            message = "Active request completed.";
            m_currentActivity = NfcIdle;
            break;
        case NfcNdefReading:
            message = "Read request completed.";
            m_currentActivity = NfcIdle;
            break;
        case NfcNdefWriting: {
            message = "Write request completed.";
            if (m_pendingWriteNdef) {
                // Writing tags is still pending - means the user can write another
                // tag with the same contents.
                showAnotherTagWriteMsg = true;
            } else {
                m_currentActivity = NfcIdle;  // Read or write request finished
            }
            break;
        }
        default:
            message = "Request completed.";
            break;
        }
        m_cachedRequestType = NfcIdle;
        emit nfcStatusUpdate(message);
        stoppedTagInteraction();
    } else {
        message = "Request completed.";
    }
    qDebug() << message;
    if (showAnotherTagWriteMsg) {
        // Emit the message that the user can write another tag.
        // (after we emitted the message that the previous write request completed).
        emit nfcStatusUpdate("Touch another tag to write again.");
    }

    // Request the response
    if (m_reportingLevel == DebugReporting) {
        // Print the response of the tag to the debug output
        // in case debug reporting is active.
        if (m_cachedTarget)
        {
            QVariant response = m_cachedTarget->requestResponse(id);
            if (response.isValid()) {
                if (response.type() == QVariant::ByteArray) {
                    //emit nfcStatusUpdate("Response (" + QString(response.typeName()) + ")");
                    qDebug() << "Response (" << QString(response.typeName()) << ")";
                } else {
                    //emit nfcStatusUpdate("Response (" + QString(response.typeName()) + "): " + response.toString());
                    qDebug() << "Response (" << QString(response.typeName()) << "): " << response.toString();
                }
                if (response.type() == QVariant::ByteArray) {
                    QByteArray p = response.toByteArray();
                    QString arrayContents = "";
                    for (int i = 0; i < p.size(); ++i) {
                        arrayContents.append(QString("0x") + QString::number(p.at(i), 16) + " ");
                    }
                    //emit nfcStatusUpdate("Raw contents of payload:\n" + arrayContents + "\n");
                    qDebug() << "Raw contents of response:\n" << arrayContents;
                }
            }
        }
    }
}

/*!
  \brief Slot called by Qt Mobility when an NDEF message was successfully
  written to a tag.

  Emits an nfcStatusUpdate signal to log this in the user interface.
  On MeeGo, both the requestCompleted() method and this method will be called
  when writing a tag.
  */
void NfcInfo::ndefMessageWritten()
{
    emit nfcTagWritten();
    stoppedTagInteraction();
    qDebug() << "ndefMessageWritten";

    if (m_pendingWriteNdef) {
        // Writing tags is still pending - means the user can write another
        // tag with the same contents.
        emit nfcStatusUpdate("Touch another tag to write again.");
    } else {
        m_currentActivity = NfcIdle;
    }
}

/*!
  \brief Return a textual description of the \a error.
  */
QString NfcInfo::convertTargetErrorToString(QNearFieldTarget::Error error)
{
    QString errorString = "Unknown";
    switch (error)
    {
    case QNearFieldTarget::NoError:
        errorString = "No error has occurred.";
        break;
    case QNearFieldTarget::UnsupportedError:
        errorString = "The requested operation is unsupported by this near field target.";
        break;
    case QNearFieldTarget::TargetOutOfRangeError:
        errorString = "The target is no longer within range.";
        break;
    case QNearFieldTarget::NoResponseError:
        errorString = "The target did not respond.";
        break;
    case QNearFieldTarget::ChecksumMismatchError:
        errorString = "The checksum has detected a corrupted response.";
        break;
    case QNearFieldTarget::InvalidParametersError:
        errorString = "Generic communication error"; //"Invalid parameters were passed to a tag type specific function.";
        break;
    case QNearFieldTarget::NdefReadError:
        errorString = "Failed to read NDEF messages from the target.";
        break;
    case QNearFieldTarget::NdefWriteError:
        errorString = "Failed to write NDEF messages to the target.";
        break;
//#ifdef MEEGO_EDITION_HARMATTAN
    case QNearFieldTarget::UnknownError:
        errorString = "Unknown error.";
        break;
//#endif
    }
    return errorString;
}

#ifdef Q_OS_SYMBIAN
/*!
  \brief Slot will be executed by the nfcsettings component whenever
  the NFC availability changes in the phone.

  This can also happen at runtime, for example when the user switches
  on NFC through the settings app. Emits an nfcStatusError or nfcStatusUpdate
  signal with the new information.
  */
void NfcInfo::handleNfcModeChange(NfcSettings::NfcMode nfcMode)
{
    switch (nfcMode) {
    case NfcSettings::NfcModeNotSupported:
        // NFC is not currently supported. It is not possible to distinguish
        // whether a firmware update could enable NFC features based solely
        // on the value of the nfcMode parameter. The return value of
        // NfcSettings::nfcFeature() indicates whether a firmware update is
        // applicable to this device.
        emit nfcStatusError(tr("NFC is not currently supported."));
        break;
    case NfcSettings::NfcModeUnknown:
        // NFC is supported, but the current mode is unknown at this time.
        emit nfcStatusError(tr("NFC is supported, but the current mode is unknown at this time."));
        break;
    case NfcSettings::NfcModeOn:
        // NFC is supported and switched on.
        emit nfcStatusUpdate(tr("NFC is supported and switched on."));
        break;
    case NfcSettings::NfcModeCardOnly:
        // NFC hardware is available and currently in card emulation mode.
        emit nfcStatusError(tr("NFC hardware is available and currently in card emulation mode."));
        break;
    case NfcSettings::NfcModeOff:
        // NFC hardware is available but currently switched off.
        emit nfcStatusError(tr("NFC hardware is available but currently switched off."));
        break;
    default:
        break;
    }
}

/*!
  \brief Handle any error that might have occured when checking
  the NFC support on the phone.
  */
void NfcInfo::handleNfcError(NfcSettings::NfcError nfcError, int error)
{
    // The platform specific error code is ignored here.
    Q_UNUSED(error)

    switch (nfcError) {
    case NfcSettings::NfcErrorFeatureSupportQuery:
        // Unable to query NFC feature support.
        emit nfcStatusError(tr("Unable to query NFC feature support."));
        break;
    case NfcSettings::NfcErrorSoftwareVersionQuery:
        // Unable to query device software version.
        emit nfcStatusError(tr("Unable to query device software version."));
        break;
    case NfcSettings::NfcErrorModeChangeNotificationRequest:
        // Unable to request NFC mode change notifications.
        emit nfcStatusError(tr("Unable to request NFC mode change notifications."));
        break;
    case NfcSettings::NfcErrorModeChangeNotification:
        // NFC mode change notification was received, but caused an error.
        emit nfcStatusError(tr("NFC mode change notification was received, but caused an error."));
        break;
    case NfcSettings::NfcErrorModeRetrieval:
        // Unable to retrieve current NFC mode.
        emit nfcStatusError(tr("Unable to retrieve current NFC mode."));
        break;
    default:
        break;
    }
}



#endif
