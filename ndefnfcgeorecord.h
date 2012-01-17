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
#ifndef NDEFNFCGEORECORD_H
#define NDEFNFCGEORECORD_H

#include <QNdefMessage>
#include <QNdefRecord>
#include <QString>
#include <QUrl>
#include <QGeoCoordinate>
#include <QNdefNfcUriRecord>
#include "ndefnfcsprecord.h"
#include "ndefnfcsmarturirecord.h"

QTM_USE_NAMESPACE

/*!
  \brief Store longitude and latitude on a tag, to allow the user
  to view a map when touching the tag.

  Geo tags are not standardized by the NFC forum, therefore,
  this class supports three different types of writing the location
  to a tag.

  1. GeoUri: write URI based on the "geo:" URI scheme, as specified
  by RFC5870, available at: http://geouri.org/
  Currently supported by MeeGo Harmattan and Android, not recognized
  by Symbian at the moment.

  2. NokiaMaps: write URI based on a Nokia Maps link, following the
  "http://m.ovi.me/?c=..." scheme of the Nokia/Ovi Maps Rendering API.
  Depending on the target device, the phone / web service should then
  redirect to the best maps representation.
  On Symbian, the phone will launch the Nokia Maps client. On a
  desktop computer, the full Nokia Maps web experience will open.
  On other phones, the HTML 5 client may be available.
  On MeeGo Harmattan, currently only a static Maps image is shown,
  making this alternative not suitable for the Nokia N9.

  3. WebRedirect: uses the web service at NfcInteractor.com to
  check the OS of the phone, and then redirect to the best way
  of showing maps to the user. On MeeGo Harmattan, this would be
  the GeoUri (1) scheme; on Symbian, the NokiaMaps-link (2).
  Essentially, this allows opening the Maps client on all currently
  available NFC capable Nokia phones.
  Note the limitations and terms of use of the web service. For
  real world deployment, outside of development and testing, it's
  recommended to host the script on your own web server.
  Find more information at nfcinteractor.com.

  As this class is based on the Smart URI base class, the
  payload is formatted as a URI record initially. When first
  adding Smart Poster information (like a title), the payload
  instantly transforms into a Smart Poster.

  \version 1.0.0
  */
class NdefNfcGeoRecord : public NdefNfcSmartUriRecord
{
public:
    // "urn:nfc:wkt:U" / "urn:nfc:wkt:Sp"
    NdefNfcGeoRecord();
    NdefNfcGeoRecord(const QGeoCoordinate& geoCoordinate);

public:
    enum NfcGeoType {
        GeoUri = 0,
        NokiaMaps,
        WebRedirect
    };
public:
    QGeoCoordinate location() const;
    void setLocation(const QGeoCoordinate& geoCoordinate);
    void setLatitude(const double latitude);
    void setLongitude(const double longitude);

    NfcGeoType geoType() const;
    void setGeoType(const NfcGeoType geoType);

private:
    void updatePayload();

private:
    QGeoCoordinate m_geoCoordinate;
    NfcGeoType m_geoType;
};

#endif // NDEFNFCGEORECORD_H
