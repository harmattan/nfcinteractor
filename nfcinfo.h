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
#ifndef NFCINFO_H
#define NFCINFO_H

#include <QObject>
#include <QDebug>
#include <QByteArray>
#include <QVariant>
#include <QUrl>
#include <QDeclarativeView>

#include <QNearFieldManager>
#include <QNdefFilter>
#include <QNearFieldTarget>

// NDEF
#include <QNdefMessage>
#include <QNdefRecord>
#include <QNdefNfcUriRecord>
#include <QNdefNfcTextRecord>
#include "ndefnfcmimeimagerecord.h"
#include "ndefnfcsprecord.h"
#include "ndefnfcmimevcardrecord.h"
#include "ndefnfcsmarturirecord.h"
#include "ndefnfcgeorecord.h"

// Analyze and parse targets
#include "nfctargetanalyzer.h"
#include "nfcndefparser.h"

#include "tagimagecache.h"

// Record model for writing
#include "nfcrecordmodel.h"
#include "nfcrecorditem.h"

#ifdef Q_OS_SYMBIAN
#include "nfcsettings.h"
#endif

QTM_USE_NAMESPACE

/*!
  \brief The NfcInfo class manages the interaction with the Nfc hardware,
  parses the tags and emits information signals for the UI.

  Following the use case of this application, the emitted signals
  contain mostly textual descriptions of the events or the information.
  Therefore, it is a good example to understand how the APIs are working
  and what kind of information you can get through the Qt Mobility APIs.

  Additionally, this class demonstrates how to parse tags and how to
  extract the relevant information - e.g., the parameters of a Smart Poster
  record, the image of a Mime record or the QContact coming from the versit
  document stored on a mime / text-vCard record.

  Furthermore, the class can also write NDEF messages to tags. The contents
  are edited by the user through the UI and stored in the model called
  NfcRecordModel. When writing, the model is converted to an actual NDEF message
  and written to the next available tag.

  The class will also handles errors that can occur and sends them out as
  signals for the UI to handle.
  */
class NfcInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(NfcRecordModel* recordModel READ recordModel NOTIFY recordModelChanged)

    enum NfcRequestStatus {
        NfcUninitialized,
        NfcIdle,
        NfcNdefReading,
        NfcNdefWriting,
        NfcTargetAnalysis
    };
    enum ReportingLevel {
        DebugReporting,
        FullReporting,
        OnlyImportantReporting
    };

public:
    explicit NfcInfo(QObject *parent = 0);
    ~NfcInfo();

    void setImageCache(TagImageCache* tagImageCache);
    void setDeclarativeView(QDeclarativeView& view);

signals:
    /*! Update on the NFC status (starting target detection, etc.) */
    void nfcStatusUpdate(const QString& nfcStatusText);
    /*! An important operation finished successfully (reading or writing tags) */
    void nfcStatusSuccess(const QString& nfcStatusSuccessText);
    /*! Error with NFC. */
    void nfcStatusError(const QString& nfcStatusErrorText);
    /*! Generic information about a new tag that was detected */
    void nfcInfoUpdate(const QString& nfcInfoText);
    /*! Information when the app starts interacting with a tag. */
    void nfcStartingTagInteraction();
    /*! Finished interacting with the tag. */
    void nfcStoppedTagInteraction();
    /*! Contents of the NDEF message found on the tag. */
    void nfcTagContents(const QString& nfcTagContents);
    /*! The tag contained an image. The parameter contains
      the image id that can be used to fetch it from the
      tag image cache class. */
    void nfcTagImage(const int nfcImgId);
    /*! An error occured while interacting with the tag. */
    void nfcTagError(const QString& nfcTagError);
    /*! Written the message to the tag. */
    void nfcTagWritten();
    /*! An error occured while writing the tag. */
    void nfcTagWriteError(const QString& nfcTagError);
    /*! Switched to a different operation mode. Using int for better compatibility to QML. */
    void nfcModeChanged(const int nfcNewMode);

    void storedMessageSizeChanged(const int ndefMessageSize);

    void recordModelChanged();
public slots:
    bool checkNfcStatus();
    bool initAndStartNfc();
    bool nfcWriteTag(const bool writeOneTagOnly);
    void nfcStopWritingTags();
    NfcRecordModel* recordModel() const;

private slots:
    void nfcRecordModelChanged();
    void targetDetected(QNearFieldTarget *target);
    void targetMessageDetected(const QNdefMessage &message, QNearFieldTarget *target);
    void ndefMessageRead(const QNdefMessage &message);
    void ndefMessageWritten();
    void requestCompleted(const QNearFieldTarget::RequestId & id);
    void targetError(QNearFieldTarget::Error error, const QNearFieldTarget::RequestId &id);
    void targetLost(QNearFieldTarget *target);

private:
    bool writeCachedNdefMessage();

    QString convertTargetErrorToString(QNearFieldTarget::Error error);

    void startedTagInteraction();
    void stoppedTagInteraction();

#ifdef Q_OS_SYMBIAN
private slots:
    // Check for NFC Support
    void handleNfcError(NfcSettings::NfcError nfcError, int error);
    void handleNfcModeChange(NfcSettings::NfcMode nfcMode);
private:
    NfcSettings* nfcSettings;
#endif

private:
    /*! Responsible for finding tags and interacting with them. */
    QNearFieldManager *m_nfcManager;
    /*! Stores the editable records of the compose tag view. */
    NfcRecordModel* m_nfcRecordModel;
    /*! Current NFC target in proximity. */
    QNearFieldTarget *m_cachedTarget;

    /*! Analyze the tag type platform targets, getting low level information about
      the tags themselves. */
    NfcTargetAnalyzer *m_nfcTargetAnalyzer;
    /*! Analyze the NDEF contents and return the record contents in text format. */
    NfcNdefParser *m_nfcNdefParser;

    /*! Configures mainly how many messages are shown on the screen in the
      Nfc Info view. Also partly affects output to qDebug(). */
    ReportingLevel m_reportingLevel;

    /*! Set to true if the application requested to write an NDEF message
      to the tag on the next opportunity. */
    bool m_pendingWriteNdef;
    /*! Set to true if the app is currently interacting with a tag. */
    bool m_nfcTagInteractionActive;
    /*! Current activity / status of the class, e.g., reading or analyzing
      a tag. The activity can consist of multiple individual requests. */
    NfcRequestStatus m_currentActivity;
    /*! If set to true, the message will only be written to the next tag,
      then the class will read tags again.
      If set to false, the message will be written to every future tag
      touched, until nfcStopWriteTag() is called. */
    bool m_writeOneTagOnly;
    /*! The cached NDEF message that is to be written to the tag. */
    QNdefMessage* m_cachedNdefMessage;
    /*! Save the size of the message that is queued to write, to make
      it easier to compare it to the tag size if writing fails. */
    int m_cachedNdefMessageSize;
    /*! Currently active request ID for tracking the requests
      to the NFC interface. Only used for main read & write requests.
      Finishing them will stop NFC interactivity. */
    QNearFieldTarget::RequestId m_cachedRequestId;

    /*! Status of the concrete currently active request.
      In contrast to the m_currentActivity, this is related to only
      one request and not the whole class status, where for example the
      analysis phase can include multiple individual requests. */
    NfcRequestStatus m_cachedRequestType;

    /*! Needed on MeeGo Harmattan to raise the app to the foreground when
      it's autostarted. */
    QDeclarativeView* m_declarativeView;
};

#endif // NFCINFO_H
