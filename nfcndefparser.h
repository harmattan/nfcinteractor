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
#ifndef NFCNDEFPARSER_H
#define NFCNDEFPARSER_H

#include <QObject>
#include <QDebug>
#include "nfcrecordmodel.h"

// Clipboard
#include <QApplication>
#include <QClipboard>

// NDEF
#include <QNdefMessage>
#include <QNdefRecord>
#include <QNdefNfcUriRecord>
#include <QNdefNfcTextRecord>
#include "ndefnfcrecords/ndefnfcmimeimagerecord.h"
#include "ndefnfcrecords/ndefnfcsprecord.h"
#include "ndefnfcrecords/ndefnfcmimevcardrecord.h"

// Image handling
#include <QImage>
#include "tagimagecache.h"

// VCard reading
#include <QContact>
#include <QContactDetail>
#include <QVariantMap>
#include <QContactThumbnail>


QTM_USE_NAMESPACE

/*!
  \brief Parse the contents of an NDEF message and return the contents
  of known records in human-readable textual form.

  The output contains information about all the records contained
  in the message. More details are reported for records of type:
  Uri, Text, Smart Poster, Image and Mime/vCard.

  The NfcTargetAnalyzer has a similar task, but returns the general
  tag information in textual form.
  */
class NfcNdefParser : public QObject
{
    Q_OBJECT
public:
    explicit NfcNdefParser(NfcRecordModel *nfcRecordModel, QObject *parent = 0);
    void setImageCache(TagImageCache* tagImageCache);

signals:
    /*! \brief The tag contained an image.
      The parameter contains the image id that can be used
      to fetch it from the tag image cache class. */
    void nfcTagImage(const int nfcImgId);

public:
    /*! \brief Parse the NDEF message and return its contents
      as human-readable text. */
    QString parseNdefMessage(const QNdefMessage &message);

    void setParseToModel(bool parseToModel);
private:
    QString parseUriRecord(const QNdefNfcUriRecord &record);
    QString parseTextRecord(const QNdefNfcTextRecord &record);
    QString textRecordToString(const QNdefNfcTextRecord &textRecord);
    QString parseSpRecord(const NdefNfcSpRecord &record);
    QString parseImageRecord(const NdefNfcMimeImageRecord &record);
    QString parseVcardRecord(NdefNfcMimeVcardRecord &record);

    QString convertRecordTypeNameToString(const QNdefRecord::TypeNameFormat typeName);

private:
    void storeClipboard(const QString &text, const QString &locale);
    void storeClipboard(const QUrl &uri);
    void setStoredClipboard();

private:
    /*! Used for storing images found on the tags. */
    TagImageCache* m_imgCache;    // Not owned by this class
    bool m_parseToModel;
    /*! Stores the editable records of the compose tag view.
      Not owned by this class.*/
    NfcRecordModel* m_nfcRecordModel;

    /*! Content that's currently stored in m_clipboardText.
      Used to prioritize what's going to be written to the clipboard,
      in the following priority order:
      Uri > Text (English) > Text (any other language). */
    enum ClipboardContents {
        ClipboardEmpty,
        ClipboardUri,
        ClipboardText,
        ClipboardTextEn // Give preference to English text for storing on the clipboard
    };
    ClipboardContents m_clipboardContents;
    QString m_clipboardText;

};

#endif // NFCNDEFPARSER_H
