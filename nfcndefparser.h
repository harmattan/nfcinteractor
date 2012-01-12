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

// NDEF
#include <QNdefMessage>
#include <QNdefRecord>
#include <QNdefNfcUriRecord>
#include <QNdefNfcTextRecord>
#include "ndefnfcmimeimagerecord.h"
#include "ndefnfcsprecord.h"
#include "ndefnfcmimevcardrecord.h"

// Image handling
#include <QImage>
#include "tagimagecache.h"

// VCard reading
#include <QContact>
#include <QContactDetail>
#include <QVariantMap>
#include <QContactThumbnail>


QTM_USE_NAMESPACE

class NfcNdefParser : public QObject
{
    Q_OBJECT
public:
    explicit NfcNdefParser(QObject *parent = 0);
    void setImageCache(TagImageCache* tagImageCache);

signals:
    /*! The tag contained an image. The parameter contains
      the image id that can be used to fetch it from the
      tag image cache class. */
    void nfcTagImage(const int nfcImgId);

public:
    QString parseNdefMessage(const QNdefMessage &message);

private:
    QString parseUriRecord(const QNdefNfcUriRecord &record);
    QString parseTextRecord(const QNdefNfcTextRecord &record);
    QString textRecordToString(const QNdefNfcTextRecord &textRecord);
    QString parseSpRecord(const NdefNfcSpRecord &record);
    QString parseImageRecord(const NdefNfcMimeImageRecord &record);
    QString parseVcardRecord(NdefNfcMimeVcardRecord &record);

    QString convertRecordTypeNameToString(const QNdefRecord::TypeNameFormat typeName);

private:
    /*! Used for storing images found on the tags. */
    TagImageCache* m_imgCache;    // Not owned by this class
    
};

#endif // NFCNDEFPARSER_H
