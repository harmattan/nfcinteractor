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
#include "ndefnfcsmsrecord.h"

/*!
  \brief Create an empty Sms record.
  */
NdefNfcSmsRecord::NdefNfcSmsRecord()
    : NdefNfcSmartUriRecord()
{
    updatePayload();
}

/*!
  \brief Return the current body of the short message.
  */
QString NdefNfcSmsRecord::smsBody() const
{
    return m_smsBody;
}

/*!
  \brief Set the body of the short message.

  Note: the URI content has to be encoded in UTF-8, which should
  be done by the Qt Mobility URI class. No further transformation
  is necessary for the SMS use case (e.g., to 7-bit US-ASCII, as
  would be recommended for Internet URLs).
  */
void NdefNfcSmsRecord::setSmsBody(const QString &smsBody)
{
    m_smsBody = smsBody;
    updatePayload();
}

/*!
  \brief Retrieve the number  the reading phone is supposed to send
  the short message to.
  */
QString NdefNfcSmsRecord::smsNumber() const
{
    return m_smsNumber;
}

/*!
  \brief Set the number the reading phone is supposed to send
  the short message to.

  Recommended to store in international format, e.g., +431234...
  */
void NdefNfcSmsRecord::setSmsNumber(const QString &smsNumber)
{
    m_smsNumber = smsNumber;
    updatePayload();
}

/*!
  \brief Format the payload and set it through the Smart URI base class.
  */
void NdefNfcSmsRecord::updatePayload()
{
    // Create URI string
    QString tagSmsUri = "sms:" + m_smsNumber;
    if (!m_smsBody.isEmpty()) {
        tagSmsUri.append("?body=" + m_smsBody);
    }

    NdefNfcSmartUriRecord::setUri(tagSmsUri);
}