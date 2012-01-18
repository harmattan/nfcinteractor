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
#ifndef NFCMODELTONDEF_H
#define NFCMODELTONDEF_H

#include <QObject>
#include <QDebug>
#include "nfcrecordmodel.h"
#include "nfcrecorditem.h"
#include <QNdefMessage>
#include <QNdefRecord>
#include <QNdefNfcTextRecord>
#include <QNdefNfcUriRecord>
#include "ndefnfcsprecord.h"
#include "ndefnfcmimeimagerecord.h"
#include "ndefnfcsmarturirecord.h"
#include "ndefnfcgeorecord.h"
#include "ndefnfcsmsrecord.h"
#include "ndefnfcsocialrecord.h"
#include "ndefnfcstorelinkrecord.h"

// Contact handling
#include "ndefnfcmimevcardrecord.h"
#include <QContact>
#include <QContactDetail>
#include <QContactName>
#include <QContactNickname>
#include <QContactPhoneNumber>
#include <QContactEmailAddress>
#include <QContactUrl>
#include <QContactOrganization>
#include <QContactAddress>
#include <QContactNote>
#include <QContactBirthday>

/*!
  \brief Builds an NDEF message based on the data stored in the NfcRecordModel.
  */
class NfcModelToNdef : public QObject
{
    Q_OBJECT
public:
    /*! Construct a new converter, based on the list of NfcRecordItems, as managed
      by the NfcRecordModel. */
    explicit NfcModelToNdef(QList<NfcRecordItem*> &nfcRecordItems, QObject *parent = 0);
    /*! Convert the current data in the record model to an NDEF message. */
    QNdefMessage * convertToNdefMessage();

private:
    NdefNfcSpRecord *convertSpFromModel(const int startIndex, int &endIndex);
    QNdefNfcUriRecord *convertUriFromModel(const int startIndex, int &endIndex, bool expectHeader = true);
    QNdefNfcTextRecord *convertTextFromModel(const int startIndex, int &endIndex, bool expectHeader = true);
    NdefNfcSmsRecord *convertSmsFromModel(const int startIndex, int& endIndex);
    NdefNfcMimeVcardRecord *convertBusinessCardFromModel(const int startIndex, int &endIndex);
    template<class T> bool contactSetDetail(QContact &contact, const NfcTypes::RecordContent contentType, const QString &value);
    NdefNfcSocialRecord *convertSocialNetworkFromModel(const int startIndex, int &endIndex);
    NdefNfcGeoRecord *convertGeoFromModel(const int startIndex, int& endIndex);
    NdefNfcStoreLinkRecord *convertStoreFromModel(const int startIndex, int &endIndex);
    NdefNfcStoreLinkRecord::AppStore appStoreFromRecordContentType(const NfcTypes::RecordContent contentType);
    QNdefRecord *convertCustomFromModel(const int startIndex, int &endIndex);

signals:

public slots:
private:
    QList<NfcRecordItem*> &m_recordItems;    // Not owned by this class

};

#endif // NFCMODELTONDEF_H
