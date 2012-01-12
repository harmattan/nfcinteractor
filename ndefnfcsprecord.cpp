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

#include "ndefnfcsprecord.h"

/*!
  \brief Create an empty Smart Poster (Sp) record.

  Note that in order to write a Smart Poster to a tag,
  you have to at least add the URL. An empty Smart Poster
  record is not valid according to the specs.
  */
NdefNfcSpRecord::NdefNfcSpRecord()
    : QNdefRecord(QNdefRecord::NfcRtd, "Sp"),
      recordUri(NULL),
      recordAction(NULL),
      recordSize(NULL),
      recordMimeType(NULL),
      recordImage(NULL)
{
    setPayload(QByteArray(0, char(0)));
}

/*!
  \brief Create a Smart Poster record based on the record passed
  through the argument.

  Internalizes and parses the payload of the original record.
  */
NdefNfcSpRecord::NdefNfcSpRecord(const QNdefRecord &other)
    : QNdefRecord(QNdefRecord::NfcRtd, "Sp"),
      recordUri(NULL),
      recordAction(NULL),
      recordSize(NULL),
      recordMimeType(NULL),
      recordImage(NULL)
{
    setPayload(other.payload());
}

NdefNfcSpRecord::~NdefNfcSpRecord()
{
    delete recordUri;
    recordTitleList.clear();
    delete recordAction;
    delete recordSize;
    delete recordMimeType;
    delete recordImage;
}

/*!
  \brief (Re)set all the stored sub records of the Smart Poster.
  */
void NdefNfcSpRecord::initializeData()
{
    delete recordUri;
    recordUri = NULL;
    recordTitleList.clear();
    delete recordAction;
    recordAction = NULL;
    delete recordSize;
    recordSize = NULL;
    delete recordMimeType;
    recordMimeType = NULL;
    delete recordImage;
    recordImage = NULL;
}

/*!
  \brief Sets the payload of the base class to the byte array
  from the parameter and parses its contents.
  */
void NdefNfcSpRecord::setPayload(const QByteArray &payload)
{
    setPayloadAndParse(payload, true);
}

/*!
  \brief Sets the payload of the base class to the byte array
  from the parameter and optionally parses its contents.

  The parsing needs to be done when the Smart Poster is read from
  a tag. If a detail of an existing Smart Poster class is modified,
  it will just update its internal payload, but doesn't need to
  parse it again (as the details are already stored in instances
  of the various record classes).
  */
void NdefNfcSpRecord::setPayloadAndParse(const QByteArray &payload, const bool parseNewPayload)
{
    QNdefRecord::setPayload(payload);
    if (parseNewPayload)
    {
        //qDebug() << "Sp parsing and internalizing payload.";
        parseRecords();
    }
}

/*!
  \brief Deletes any details currently stored in the Smart Poster
  and re-initializes them by parsing the contents of the payload.
  */
void NdefNfcSpRecord::parseRecords()
{
    initializeData();
    if (payload().count() == 0) {
        //qDebug() << "Empty payload";
        return;
    }

    QNdefMessage message = QNdefMessage::fromByteArray(payload());
    //qDebug() << "Sp Record Count: " << message.count();

    foreach (const QNdefRecord &record, message) {
        //qDebug() << "Sp Record type: " << QString(record.type());
        //qDebug() << "Sp Type name: " << record.typeNameFormat();

        // URI
        if (record.isRecordType<QNdefNfcUriRecord>()) {
            if (recordUri) { delete recordUri; recordUri = NULL; }
            recordUri = new QNdefNfcUriRecord(record);
            //qDebug() << "Sp URI: " << recordUri->uri().toString();
        }
        // Title
        else if (record.isRecordType<QNdefNfcTextRecord>()) {
            QNdefNfcTextRecord* recordTitle = new QNdefNfcTextRecord(record);
            addTitle(*recordTitle);
            //if (!recordTitleList.isEmpty()) {
            //    qDebug() << "Sp Title: " << recordTitleList.last().text();
            //}
        }
        // Image
        else if (record.typeNameFormat() == QNdefRecord::Mime &&
                   record.type().startsWith("image/")) {
            if (recordImage) { delete recordImage; recordImage = NULL; }
            recordImage = new NdefNfcMimeImageRecord(record);
            //qDebug() << "Sp Image: " << recordImage->format();
        }
        // Action
        else if (record.typeNameFormat() == QNdefRecord::NfcRtd &&
                 QString(record.type()) == "act") {
            if (recordAction) { delete recordAction; recordAction = NULL; }
            recordAction = new NdefNfcActRecord(record);
            //qDebug() << "Sp Action: " << action();
        }
        // Size
        else if (record.typeNameFormat() == QNdefRecord::NfcRtd &&
                 QString(record.type()) == "s") {
            if (recordSize) { delete recordSize; recordSize = NULL; }
            recordSize = new NdefNfcSizeRecord(record);
            //qDebug() << "Sp Size: " << size();
        }
        // Type
        else if (record.typeNameFormat() == QNdefRecord::NfcRtd &&
                 QString(record.type()) == "t") {
            if (recordMimeType) { delete recordMimeType; recordMimeType = NULL; }
            recordMimeType = new NdefNfcTypeRecord(record);
            //qDebug() << "Sp Type: " << type();
        }
        else
        {
            // This class handles all records defined in the Smart Poster
            // specification, so this case should never happen for a valid
            // Smart Poster record in the current version.
            qDebug() << "Sp: Don't know how to handle this record";
        }
    }
}

/*!
  \brief Reverse function to parseRecords() - this one takes
  the information stored in the individual record instances and assembles
  it into the payload of the base class.

  Note: as the URI is mandatory, the payload will not be assembled
  if no URI is defined.
  */
bool NdefNfcSpRecord::assemblePayload()
{
    // Uri is mandatory - don't assemble the payload if it's not set
    if (!recordUri) {
        return false;
    }

    QNdefMessage message;

    // URI (mandatory)
    message.append(*recordUri);

    // Title(s) (optional)
    if (titleCount() > 0) {
        foreach (QNdefNfcTextRecord curTitle, recordTitleList) {
            message.append(curTitle);
        }
    }

    // Action (optional)
    if (recordAction) {
        message.append(*recordAction);
    }

    // Size (optional)
    if (recordSize) {
        message.append(*recordSize);
    }

    // Type (optional)
    if (recordMimeType) {
        message.append(*recordMimeType);
    }

    // Image (optional)
    if (recordImage) {
        message.append(*recordImage);
    }

    setPayloadAndParse(message.toByteArray(), false);
    //qDebug() << "Sp Assembling: Payload set.";
    return true;
}

/*!
  \brief Returns the contents of the text record as a string.
  To be used for debug purposes.

  Also does an output of the contents to qDebug().
*/
QString NdefNfcSpRecord::rawContents() const
{
    const QByteArray p = payload();

    if (p.isEmpty())
        return QString();

    QString temp = "";
    for (int i = 0; i < p.size(); ++i) {
        temp.append(QString("0x") + QString::number(p.at(i), 16) + ", ");
    }

    qDebug() << "Raw contents of payload (" << p.size() << "chars)";
    qDebug() << temp;
    qDebug() << "Raw contents of payload (bytes)";
    qDebug() << QString(p);

    return temp;
}

// -----------------------------------------------------------------------------
// URI
/*!
  \brief Set the mandatory URI of the Smart Poster.

  \param newUri Uri to be stored in the Smart Poster,
  passed as a QUrl for convenience.
  */
void NdefNfcSpRecord::setUri(const QUrl& newUri)
{
    if (!recordUri) {
        recordUri = new QNdefNfcUriRecord();
    }
    recordUri->setUri(newUri);
    assemblePayload();
}

/*!
  \brief Set the mandatory URI of the Smart Poster.

  \param newUri Uri record to be stored in the Smart Poster.
  */
void NdefNfcSpRecord::setUri(const QNdefNfcUriRecord& newUri)
{
    if (recordUri) {
        delete recordUri;
    }
    recordUri = new QNdefNfcUriRecord(newUri);
    assemblePayload();
}

/*!
  \brief Return the URI stored in this Smart Poster,
  converted to a QUrl for convenience.

  If no URI has been set in the smart poster, an empty
  QUri is returned.
  */
QUrl NdefNfcSpRecord::uri() const
{
    if (recordUri) {
        return recordUri->uri();
    } else {
        return QUrl();
    }
}

/*!
  \brief Return the URI record stored in this Smart Poster.

  If no URI has been set in the smart poster, an empty
  record is returned.
  */
QNdefNfcUriRecord NdefNfcSpRecord::uriRecord() const
{
    if (recordUri) {
        return *recordUri;
    } else {
        return QNdefNfcUriRecord();
    }
}


// -----------------------------------------------------------------------------
// Title(s)
/*!
  \brief Add an (optional) title to the Smart Poster.

  It is possible to add more than one title as each title text
  record can have a different language. The phone is then recommended
  to choose the text record with the language that makes most sense
  to the user.

  \param newTitle (additional) title to be stored in the Smart Poster.
  */
void NdefNfcSpRecord::addTitle(const QNdefNfcTextRecord &newTitle)
{
    recordTitleList.append(newTitle);
    assemblePayload();
}

/*!
  \brief Set all the titles of the Smart Poster.

  This replaces any titles that might have been stored in the Smart Poster
  with the new list.

  It is possible to add more than one title as each title text
  record can have a different language. The phone is then recommended
  to choose the text record with the language that makes most sense
  to the user.

  \param newTitleList titles to be stored in the Smart Poster.
  */
void NdefNfcSpRecord::setTitleList(QList<QNdefNfcTextRecord> newTitleList)
{
    recordTitleList.clear();
    recordTitleList.append(newTitleList);
    assemblePayload();
}

/*!
  \brief Returns how many title text records are stored in the Smart Poster.
  */
int NdefNfcSpRecord::titleCount() const
{
    return recordTitleList.count();
}

/*!
  \brief Retrieve the full list containing all title text
  records that are stored in the Smart Poster.
  */
QList<QNdefNfcTextRecord> NdefNfcSpRecord::titles() const
{
    return recordTitleList;
}

/*!
  \brief Retrieve a specific title text record from the
  list.

  Returns an empty record in case the index is invalid.
  */
QNdefNfcTextRecord NdefNfcSpRecord::title(const int index) const
{
    if (index >= 0 && index < titleCount()) {
        return recordTitleList[index];
    } else {
        return QNdefNfcTextRecord();
    }
}


// -----------------------------------------------------------------------------
// Action
/*!
  \brief Returns if the action has been defined for this Smart Poster instance.
  */
bool NdefNfcSpRecord::actionInUse() const
{
    return (recordAction);
}

/*!
  \brief Return the action defined for this Smart Poster.

  Make sure to check actionInUse() before calling this method;
  if the Smart Poster doesn't have an action defined, this method
  will return the default NdefNfcSpRecord::DoAction.
  */
NdefNfcSpRecord::NfcAction NdefNfcSpRecord::action() const
{
    if (recordAction) {
        return recordAction->action();
    }
    return NdefNfcSpRecord::DoAction; // Default
}

/*!
  \brief Set the action of the Smart Poster to the specified value.

  After this method has been called, actionInUse() will return true.
  (As it will do when a Smart Poster containing an action has been
  read and parsed from a tag).
  */
void NdefNfcSpRecord::setAction(const NdefNfcSpRecord::NfcAction &action)
{
    if (!recordAction) {
        recordAction = new NdefNfcActRecord();
    }
    recordAction->setAction(action);
    assemblePayload();
}

// -----------------------------------------------------------------------------
// Size
/*!
  \brief Returns if the size has been defined for this Smart Poster instance.
  */
bool NdefNfcSpRecord::sizeInUse() const
{
    return (recordSize);
}

/*!
  \brief Return the size defined for this Smart Poster.

  Make sure to check sizeInUse() before calling this method;
  if the Smart Poster doesn't have a size defined, this method
  will return 0.
  */
quint32 NdefNfcSpRecord::size() const
{
    if (recordSize) {
        return recordSize->size();
    }
    return 0; // Default
}

/*!
  \brief Set the size of the Smart Poster to the specified value.

  After this method has been called, sizeInUse() will return true.
  (As it will do when a Smart Poster containing a size has been
  read and parsed from a tag).
  */
void NdefNfcSpRecord::setSize(const quint32 size)
{
    if (!recordSize) {
        recordSize = new NdefNfcSizeRecord();
    }
    recordSize->setSize(size);
    assemblePayload();
}

// -----------------------------------------------------------------------------
// Type
/*!
  \brief Returns if the type has been defined for this Smart Poster instance.
  */
bool NdefNfcSpRecord::mimeTypeInUse() const
{
    return (recordMimeType);
}

/*!
  \brief Return the type defined for this Smart Poster.

  Make sure to check typeInUse() before calling this method;
  if the Smart Poster doesn't have a type defined, this method
  will return 0.
  */
QString NdefNfcSpRecord::mimeType() const
{
    if (recordMimeType) {
        return recordMimeType->mimeType();
    }
    return QString(); // Default
}

/*!
  \brief Set the type of the Smart Poster to the specified value.

  After this method has been called, typeInUse() will return true.
  (As it will do when a Smart Poster containing a type has been
  read and parsed from a tag).
  */
void NdefNfcSpRecord::setMimeType(const QString &type)
{
    if (!recordMimeType) {
        recordMimeType = new NdefNfcTypeRecord();
    }
    recordMimeType->setMimeType(type);
    assemblePayload();
}

// -----------------------------------------------------------------------------
// Image
/*!
  \brief Returns if the Smart Poster contains an image.
  */
bool NdefNfcSpRecord::imageInUse() const
{
    return (recordImage);
}

/*!
  \brief Return the image stored in the Smart Poster.

  Make sure to check imageInUse() before calling this method;
  if the Smart Poster doesn't contain an image, this method
  will return an empty image record.
  */
NdefNfcMimeImageRecord NdefNfcSpRecord::image() const
{
    if (recordImage) {
        return *recordImage;
    }
    return NdefNfcMimeImageRecord(); // Default
}

/*!
  \brief Add an image to the Smart Poster.

  After this method has been called, imageInUse() will return true.
  (As it will do when a Smart Poster containing an image has been
  read and parsed from a tag).
  */
void NdefNfcSpRecord::setImage(const NdefNfcMimeImageRecord& imageRecord)
{
    if (recordImage) { delete recordImage; recordImage = NULL; }
    recordImage = new NdefNfcMimeImageRecord(imageRecord);
    assemblePayload();
}

/*!
  \brief Returns true if this record contains data that requires a Smart Poster
  and could not be stored in a simple URI record.

  \return true if the record contains one or more titles, an image, action, size
  or type information. It is false if the record only contains a URI.
  */
bool NdefNfcSpRecord::hasSpData() const
{
    return ((titleCount() > 0) || actionInUse() || sizeInUse() || mimeTypeInUse() || imageInUse());
}


// -----------------------------------------------------------------------------
// Smart Poster Action record
/*!
  \brief Retrieve the Smart Poster action from the record.

  The action is interpreted using an enum defined in the Smart Poster
  record class.

  In the current version of the Smart Poster specification, the values
  0, 1 and 2 have been assigned a meaning. 3+ are reserved for future
  use (RFU). If the byte in the record is therefore 3+, the RFU enum
  will be returned.

  \return the action stored in the record if specified according to the
  current Smart Poster specification, or RFU for all other cases.
  */
NdefNfcSpRecord::NfcAction NdefNfcSpRecord::NdefNfcActRecord::action() const
{
    const QByteArray p = payload();

    if (p.count() == 1)
    {
        char spAction = p.at(0);
        switch (spAction)
        {
        case 0x00:
            return NdefNfcSpRecord::DoAction;
            break;
        case 0x01:
            return NdefNfcSpRecord::SaveForLater;
            break;
        case 0x02:
            return NdefNfcSpRecord::OpenForEditing;
            break;
        default:
            return NdefNfcSpRecord::RFU;
        }
    }
    // Invalid action record
    return DoAction;
}

/*!
  \brief Set the action of the Smart Poster to the specified value.
  */
void NdefNfcSpRecord::NdefNfcActRecord::setAction(const NfcAction &action)
{
    char spAction;
    switch (action)
    {
    case NdefNfcSpRecord::DoAction:
        spAction = 0x00;
        break;
    case NdefNfcSpRecord::SaveForLater:
        spAction = 0x01;
        break;
    case NdefNfcSpRecord::OpenForEditing:
        spAction = 0x02;
        break;
    default:
    case NdefNfcSpRecord::RFU:
        spAction = 0x03;
        break;
    }
    setPayload(QByteArray(1, char(spAction)));
}

// -----------------------------------------------------------------------------
// Smart Poster Size record
/*!
  \brief Fetch the size of the content linked to by the Smart Poster.

  \return the value as stored in the Smart Poster record.
  */
quint32 NdefNfcSpRecord::NdefNfcSizeRecord::size() const
{
    const QByteArray p = payload();
    if (p.count() == 4)
    {
        return p.toUInt();
    } else {
        return 0;
    }
}

/*!
  \brief Set the size record of the Smart Poster.

  See the class documentation for more details.
  */
void NdefNfcSpRecord::NdefNfcSizeRecord::setSize(quint32 size)
{
    QByteArray p(4, char(0));
    QDataStream ds(p);
    ds.setByteOrder(QDataStream::BigEndian);
    ds << size;
    setPayload(p);
}

// -----------------------------------------------------------------------------
// Smart Poster Type record
/*!
  \brief Fetch the mime type of the content linked to by the Smart Poster.

  \return the mime type as stored in the Smart Poster record.
  */
QString NdefNfcSpRecord::NdefNfcTypeRecord::mimeType() const
{
    const QByteArray p = payload();
    if (p.isEmpty())
        return QString();
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    return codec->toUnicode(p);
}

/*!
  \brief Set the mime type of the content linked to by the Smart Poster.

  See the class documentation for more details.
  */
void NdefNfcSpRecord::NdefNfcTypeRecord::setMimeType(const QString& mimeType)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QByteArray p = codec->fromUnicode(mimeType);
    setPayload(p);
}



