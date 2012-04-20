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
#ifndef NDEFNFCMIMEVCARDRECORD_H
#define NDEFNFCMIMEVCARDRECORD_H

#include <QNdefRecord>
#include <qndefmessage.h>
#include <qndefrecord.h>
#include <QDebug>

// VCard reading
#include <QVersitDocument>
#include <QVersitReader>
#include <QVersitWriter>
#include <QVersitContactImporter>
#include <QVersitContactExporter>
#include <QContact>
#include <QBuffer>
#include <QContactName>

QTM_USE_NAMESPACE

/*!
  \brief Handles the vCard record of the MIME record type.

  The contents of the vCard are parsed using Qt Mobility APIs.
  You can then further use the returned QContact(s) to store them
  in the address book of the user.

  In case there is an issue parsing the vCard or serializing
  a QContact to a vCard, you can retrieve the error message through
  the error() method.

  \version 1.0.0
  */
class NdefNfcMimeVcardRecord : public QNdefRecord
{
public:
    // "urn:nfc:mime:text/x-vCard"
    Q_DECLARE_NDEF_RECORD(NdefNfcMimeVcardRecord, QNdefRecord::Mime, "text/x-vCard",
                                  QByteArray(0, char(0)));

public:
    QList<QContact> contacts();
    bool setContact(const QContact& contacts, QVersitDocument::VersitType versitType = QVersitDocument::VCard30Type);
    bool setContact(const QList<QContact> contacts, QVersitDocument::VersitType versitType = QVersitDocument::VCard30Type);
    QString error();

private:
    QString cachedErrorText;

};


Q_DECLARE_ISRECORDTYPE_FOR_NDEF_RECORD(NdefNfcMimeVcardRecord, QNdefRecord::Mime, "text/x-vCard")


#endif // NDEFNFCMIMEVCARDRECORD_H
