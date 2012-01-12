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

#include "ndefnfcgeorecord.h"

NdefNfcGeoRecord::NdefNfcGeoRecord()
    : NdefNfcSmartUriRecord(),
      m_geoType(GeoUri)
{
}

NdefNfcGeoRecord::NdefNfcGeoRecord(const QGeoCoordinate &geoCoordinate)
    : NdefNfcSmartUriRecord(),
      m_geoType(GeoUri)
{
    setLocation(geoCoordinate);
}

QGeoCoordinate NdefNfcGeoRecord::location() const
{
    return m_geoCoordinate;
}

void NdefNfcGeoRecord::setLocation(const QGeoCoordinate &geoCoordinate)
{
    m_geoCoordinate = geoCoordinate;
    updatePayload();
}

void NdefNfcGeoRecord::setLatitude(const double latitude)
{
    m_geoCoordinate.setLatitude(latitude);
    updatePayload();
}

void NdefNfcGeoRecord::setLongitude(const double longitude)
{
    m_geoCoordinate.setLongitude(longitude);
    updatePayload();
}

NdefNfcGeoRecord::NfcGeoType NdefNfcGeoRecord::geoType() const
{
    return m_geoType;
}

void NdefNfcGeoRecord::setGeoType(const NdefNfcGeoRecord::NfcGeoType geoType)
{
    if (m_geoType != geoType) {
        // Changing the type
        m_geoType = geoType;
        // Adapt the payload
        updatePayload();
    }
}

void NdefNfcGeoRecord::updatePayload()
{
    QUrl uri;
    // TODO: check if double -> string conversion needs to be locale-aware to get right comma
    const QString latString = QString::number(m_geoCoordinate.latitude());
    const QString longString = QString::number(m_geoCoordinate.longitude());
    switch(m_geoType) {
    case NokiaMaps:
        uri = "http://m.ovi.me/?c=" + latString + "," + longString;
        break;
    case WebRedirect:
        uri = "http://NfcInteractor.com/m.php?c=" + latString + "," + longString;
        break;
    case GeoUri:
    default:
        uri = "geo:" + latString + "," + longString;
        break;
    }

    NdefNfcSmartUriRecord::setUri(uri);
    qDebug() << "Geo coordinates (lat, long): " << latString << "," << longString;
}

