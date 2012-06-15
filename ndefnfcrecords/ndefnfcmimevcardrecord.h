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
