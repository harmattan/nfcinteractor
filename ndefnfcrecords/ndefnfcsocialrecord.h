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
#ifndef NDEFNFCSOCIALRECORD_H
#define NDEFNFCSOCIALRECORD_H

#include <QNdefMessage>
#include <QNdefRecord>
#include <QString>
#include <QUrl>
#include <QNdefNfcUriRecord>
#include "ndefnfcsprecord.h"
#include "ndefnfcsmarturirecord.h"

QTM_USE_NAMESPACE

/*!
  \brief Link to one of the supported social networks by
  simply selecting the network and specifying the username.

  Touching a tag written with this record type will take
  the user to the social network web site, where he can then
  for example start following you on Twitter.

  As this class is based on the Smart URI base class, the
  payload is formatted as a URI record initially. When first
  adding Smart Poster information (like a title), the payload
  instantly transforms into a Smart Poster.

  \version 1.2.0
  */
class NdefNfcSocialRecord : public NdefNfcSmartUriRecord
{
public:
    NdefNfcSocialRecord();

public:
    /*! Social network this class will create a link to. */
    enum NfcSocialType {
        Twitter = 0,
        LinkedIn,
        Facebook,
        Xing,
        VKontakte,
        Foursquare
    };

public:
    QString socialUserName() const;
    void setSocialUserName(const QString& socialUserName);

    NfcSocialType socialType() const;
    void setSocialType(const NfcSocialType socialType);

private:
    void updatePayload();

private:
    QString m_socialUserName;
    NfcSocialType m_socialType;
};

#endif // NDEFNFCSOCIALRECORD_H
