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
#ifndef NDEFNFCSMARTURIRECORD_H
#define NDEFNFCSMARTURIRECORD_H

#include <QNdefMessage>
#include <QNdefRecord>
#include "ndefnfcsprecord.h"
#include <QNdefNfcUriRecord>

QTM_USE_NAMESPACE

/**
  At construction and when only setting the URI, this class will stay a
  URI record. As soon as you set a Smart Poster feature, the payload and
  type will transform into a Smart Poster. This transformation will not
  be reversed if you remove Smart Poster information again.

  If you use the type() and payload() methods provided by this class, it
  will look like a URI record again. However, as the methods of the base
  class are not virtual, accessing the payload() through a variable of type
  QNdefRecord would return a Smart Poster payload after such information
  has been added.

  Note: the QNdefRecord base class doesn't define its methods as virtual.
  Therefore, make sure you only modify the data in this class if your
  object is of the correct NdefNfcSmartUriRecord type.
  */
class NdefNfcSmartUriRecord : public NdefNfcSpRecord
{
public:
    // "urn:nfc:wkt:U" / "urn:nfc:wkt:Sp"
    NdefNfcSmartUriRecord();
    NdefNfcSmartUriRecord(const QNdefRecord &other);

    QByteArray type() const;
    QByteArray payload() const;

    /*!
      \brief Returns true if the record contains any information that
      would mandate a Smart Poster record.
      If this record only contains a URI, retrieving its type or payload
      will return a URI record instead of a Smart Poster.
      */
    bool isSp() const;


    void setUri(const QUrl &uri);
    void setUri(const QNdefNfcUriRecord& newUri);

    void addTitle(const QNdefNfcTextRecord& newTitle);
    void setTitleList(QList<QNdefNfcTextRecord> newTitleList);
    void setAction(const NfcAction &action);
    void setSize(const quint32 size);
    void setMimeType(const QString& mimeType);
    void setImage(const NdefNfcMimeImageRecord& imageRecord);

private:
    void changeTypeToSp();
};

#endif // NDEFNFCSMARTURIRECORD_H
