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
#ifndef RECORDTYPES_H
#define RECORDTYPES_H

#include <QObject>

/*!
  \brief Important enums that are shared between QML and C++.

  QML doesn't have direct enum support, therefore, the enums are
  exported from C++ to be able to reuse them in the QML world.
  This is the only possible approach currently, but works fine.
  One of the limitations is that when passing data between
  C++ and QML, it's generally passed as Integer and not as a
  real enum, loosing some type safety due to the need to cast
  it back to an enum.
  */
class NfcTypes : public QObject
{
    Q_OBJECT
    Q_ENUMS(MessageType RecordContent NfcModes)
public:
    /*!
      \brief The message type of record items stored in the model.

      Some of the message types represent real NDEF record types,
      others are convenience classes provided by the Nfc Interactor
      (e.g., MsgGeo). Some other types defined here are just
      templates used to pre-populate the editing UI with various
      individual records, and do not actually appear in the model
      (e.g., MsgAnnotatedUrl, which is added as a MsgUri and a MsgText).

      Usually, several consecutive NfcRecordItem instances have the
      same MessageType, but different RecordContent - as these define
      different aspects of the message.
      */
    enum MessageType {
        MsgSmartPoster,
        MsgUri,
        MsgText,
        MsgSms,
        MsgBusinessCard,
        MsgSocialNetwork,
        MsgGeo,
        MsgStore,
        MsgImage,
        MsgAnnotatedUrl,
        MsgCustom,
        MsgCombination,
        MsgNfcAutostart
    };

    /*!
      \brief Type of the content of an individual item in the model.

      The content defines what part of the whole message is defined by this
      record item. Usually, a complete NDEF record consists of multime
      NfcRecordItem instances, all of them having the same MessageType,
      but their individual content items defined by having a different
      RecordContent. For example, a Smart Poster MessageType always has a
      RecordHeader and a RecordUri. Optionally (= if added by the user),
      it can also have a RecordText, RecordTextLanguage, RecordSpAction, etc.

      Some of the RecordContent can occur in different MessageType s. A
      good example is the RecordText, which can be used as the title in all
      records that map to a Smart Poster, or also in an extra text NDEF record.
      Other RecordContent items are specific to a MessageType, for example the
      RecordSpAction is only used within a Smart Poster.

      The RecordContent tells the UI how to draw the item in the editing UI
      (e.g., as a text box, or a selection item). Additionally, it tells the
      NfcModelToNdef class what part of the NDEF record is defined by this
      NfcRecordItem.
      */
    enum RecordContent {
        RecordHeader,
        RecordUri,
        RecordText,
        RecordTextLanguage,
        RecordSmsBody,
        RecordSpAction,
        RecordSpType,
        RecordSpSize,
        RecordImageFilename,
        RecordTypeName,
        RecordTypeNameFormat,
        RecordId,
        RecordRawPayload,
        RecordSocialNetworkType,
        RecordSocialNetworkName,

        RecordNamePrefix,
        RecordFirstName,
        RecordMiddleName,
        RecordLastName,
        RecordNameSuffix,
        RecordNickname,

        RecordEmailAddress,
        RecordPhoneNumber,
        RecordContactUrl,

        RecordOrganizationName,
        RecordOrganizationDepartment,
        RecordOrganizationRole,
        RecordOrganizationTitle,

        RecordBirthday, // not implemented yet, would require a date editor field
        RecordNote,

        RecordCountry,
        RecordLocality,
        RecordPostOfficeBox,
        RecordPostcode,
        RecordRegion,
        RecordStreet,

        RecordGeoType,
        RecordGeoLatitude,
        RecordGeoLongitude,

        RecordStoreNokia,
        RecordStoreSymbian,
        RecordStoreMeeGoHarmattan,
        RecordStoreSeries40,
        RecordStoreWindowsPhone,
        RecordStoreAndroid,
        RecordStoreiOS,
        RecordStoreBlackberry,
        RecordStoreCustomName
    };

    /*!
      \brief Status information whether the NfcInfo class is currently in reading or writing mode.

      Used to send signals from C++ to QML, to let the UI react to status changes
      of the C++ engine classes.
      */
    enum NfcModes {
        nfcReading = 0,
        nfcWriting
    };
};

#endif // RECORDTYPES_H
