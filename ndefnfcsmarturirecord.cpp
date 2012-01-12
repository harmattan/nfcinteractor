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
#include "ndefnfcsmarturirecord.h"

NdefNfcSmartUriRecord::NdefNfcSmartUriRecord()
    : NdefNfcSpRecord()
{
    setType("U");
}

QByteArray NdefNfcSmartUriRecord::type() const
{
    if (NdefNfcSpRecord::hasSpData()) {
        return NdefNfcSpRecord::type();
    }
    // URI record
    return "U";
}

QByteArray NdefNfcSmartUriRecord::payload() const
{
    if (NdefNfcSpRecord::hasSpData()) {
        return NdefNfcSpRecord::payload();
    }
    // URI record
    return NdefNfcSpRecord::uriRecord().payload();
}

bool NdefNfcSmartUriRecord::isSp() const
{
    return type() == "Sp";
}

void NdefNfcSmartUriRecord::setUri ( const QUrl & uri )
{
    if (NdefNfcSpRecord::hasSpData()) {
        // It's already a Smart Poster, so use the methods
        // of the Sp base class
        NdefNfcSpRecord::setUri(uri);
    } else {
        NdefNfcSpRecord::setUri(uri);
        // It's still a URI record - so set the payload as a
        // URI record as well and override the Smart Poster.
        QNdefNfcUriRecord uriRecord;
        uriRecord.setUri(uri);
        QNdefRecord::setPayload(uriRecord.payload());
    }
}

void NdefNfcSmartUriRecord::setUri(const QNdefNfcUriRecord &newUri)
{
    if (NdefNfcSpRecord::hasSpData()) {
        // It's already a Smart Poster, so use the methods
        // of the Sp base class
        NdefNfcSpRecord::setUri(newUri);
    } else {
        NdefNfcSpRecord::setUri(newUri);
        // It's still a URI record - so set the payload as a
        // URI record as well and override the Smart Poster.
        QNdefRecord::setPayload(newUri.payload());
    }
}

void NdefNfcSmartUriRecord::addTitle(const QNdefNfcTextRecord &newTitle)
{
    changeTypeToSp();
    NdefNfcSpRecord::addTitle(newTitle);
}

void NdefNfcSmartUriRecord::setTitleList(QList<QNdefNfcTextRecord> newTitleList)
{
    changeTypeToSp();
    NdefNfcSpRecord::setTitleList(newTitleList);
}

void NdefNfcSmartUriRecord::setAction(const NdefNfcSpRecord::NfcAction &action)
{
    changeTypeToSp();
    NdefNfcSpRecord::setAction(action);
}

void NdefNfcSmartUriRecord::setSize(const quint32 size)
{
    changeTypeToSp();
    NdefNfcSpRecord::setSize(size);
}

void NdefNfcSmartUriRecord::setMimeType(const QString &mimeType)
{
    changeTypeToSp();
    NdefNfcSpRecord::setMimeType(mimeType);
}

void NdefNfcSmartUriRecord::setImage(const NdefNfcMimeImageRecord &imageRecord)
{
    changeTypeToSp();
    NdefNfcSpRecord::setImage(imageRecord);
}

void NdefNfcSmartUriRecord::changeTypeToSp()
{
    setType("Sp");
}

