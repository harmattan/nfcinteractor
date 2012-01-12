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

#include "nfcndefparser.h"

NfcNdefParser::NfcNdefParser(QObject *parent) :
    QObject(parent)
{
}


/*!
  Set the image cache to use for storing images retrieved
  from tags.
  \a tagImageCache instance of the image cache. This
  class will not take ownership of the instance!
  */
void NfcNdefParser::setImageCache(TagImageCache *tagImageCache)
{
    // Not owned by this class
    m_imgCache = tagImageCache;
}

QString NfcNdefParser::parseNdefMessage(const QNdefMessage &message)
{
    if (message.isEmpty()) {
        return "No records in the Ndef message";
    }

    QString tagContents;

    QByteArray rawMessage = message.toByteArray();
    const int msgSize = rawMessage.size();
    if (msgSize > 0) {
        tagContents.append("Message size: " + QString::number(msgSize) + " bytes\n");
    }

    const int recordCount = message.size();
    int numRecord = 1;
    // Go through all records in the message
    foreach (const QNdefRecord &record, message)
    {
        if (recordCount > 1)
        {
            tagContents.append("Record " + QString::number(numRecord) + "/" + QString::number(recordCount) + "\n");
        }
        // Print generic information about the record
        tagContents.append("Type name: " + convertRecordTypeNameToString(record.typeNameFormat()) + "\n");
        tagContents.append("Record type: " + QString(record.type()) + " ");

        // Parse tag contents
        if (record.isRecordType<QNdefNfcUriRecord>())
        {
            // ------------------------------------------------
            // URI
            tagContents.append(parseUriRecord(QNdefNfcUriRecord(record)));
        }
        else if (record.isRecordType<QNdefNfcTextRecord>())
        {
            // ------------------------------------------------
            // Text
            tagContents.append(parseTextRecord(QNdefNfcTextRecord(record)));
        }
        else if (record.isRecordType<NdefNfcSpRecord>())
        {
            // ------------------------------------------------
            // Smart Poster (urn:nfc:wkt:Sp)
            tagContents.append(parseSpRecord(NdefNfcSpRecord(record)));
        }
        else if (record.typeNameFormat() == QNdefRecord::Mime &&
                   record.type().startsWith("image/"))
        {
            // The NdefNfcMimeImageRecord class handles many different
            // record types. To simplify the use, there is no extra
            // class for each individual image type (image/png, gif,
            // jpg, jpeg, etc.)
            // Therefore, the isRecordType<>() method can't be used
            // with the generic image handler class.
            // ------------------------------------------------
            // Image (any supported type)
            tagContents.append(parseImageRecord(NdefNfcMimeImageRecord(record)));
        }
        else if (record.isRecordType<NdefNfcMimeVcardRecord>())
        {
            // ------------------------------------------------
            // Mime type: vCard
            NdefNfcMimeVcardRecord vCardRecord(record);
            tagContents.append(parseVcardRecord(vCardRecord));
        }
        else if (record.isEmpty())
        {
            // ------------------------------------------------
            // Empty
            tagContents.append("[Empty record]\n");
        }
        else
        {
            // ------------------------------------------------
            // Record type not handled by this application
            tagContents.append("[Unknown record: " + record.type() + "]\n");
        }
        numRecord++;
    }
    return tagContents;
}

/*!
  \brief Create a textual description of the contents of the
  Uri (U) record.

  \param record the record to analyze
  \return plain text description of the record contents.
  */
QString NfcNdefParser::parseUriRecord(const QNdefNfcUriRecord& record)
{
    QString tagContents("[URI]\n");
    tagContents.append(record.uri().toString());
    return tagContents;
}

/*!
  \brief Create a textual description of the contents of the
  Text (T) record.

  \param record the record to analyze
  \return plain text description of the record contents.
  */
QString NfcNdefParser::parseTextRecord(const QNdefNfcTextRecord& record)
{
    QString tagContents("[Text]\n");
    // Add the text info to the string, parsed by an extra method
    // as the same content is also present for example in the Smart Poster.
    tagContents.append(textRecordToString(record));
    return tagContents;
}

/*!
  \brief Convert the details of a text record to a string.

  Seperated from parseTextRecord() as this is also used by the
  Smart Poster parser.
  */
QString NfcNdefParser::textRecordToString(const QNdefNfcTextRecord& textRecord)
{
    QString txt("Title: " + textRecord.text() + "\n");
    txt.append("Locale: " + textRecord.locale() + "\n");
    const QString textEncoding = textRecord.encoding() == QNdefNfcTextRecord::Utf8 ? "UTF-8" : "UTF-16";
    txt.append("Encoding: " + textEncoding + "\n");
    return txt;
}

/*!
  \brief Create a textual description of the contents of the
  Smart Poster (Sp) record.

  \param record the record to analyze
  \return plain text description of the record contents.
  */
QString NfcNdefParser::parseSpRecord(const NdefNfcSpRecord& record)
{
    QString tagContents("[Smart Poster]\n");

    // Uri
    tagContents.append("Uri: " + record.uri().toString() + "\n");

    // Title
    tagContents.append("Title count: " + QString::number(record.titleCount()) + "\n");
    if (record.titleCount() > 0)
    {
        foreach (QNdefNfcTextRecord curTitle, record.titles()) {
            tagContents.append(textRecordToString(curTitle));
        }
    }

    // Action
    if (record.actionInUse())
    {
        QString spActionString = "Unknown";
        switch (record.action())
        {
        case NdefNfcSpRecord::DoAction:
            spActionString = "Do Action";
            break;
        case NdefNfcSpRecord::SaveForLater:
            spActionString = "Save for later";
            break;
        case NdefNfcSpRecord::OpenForEditing:
            spActionString = "Open for editing";
            break;
        case NdefNfcSpRecord::RFU:
            spActionString = "RFU";
            break;
        }
        tagContents.append("Action: " + spActionString + "\n");
    }

    // Size
    if (record.sizeInUse())
    {
        tagContents.append("Size: " + QString::number(record.size()) + "\n");
    }

    // Type
    if (record.mimeTypeInUse())
    {
        tagContents.append("Type: " + record.mimeType() + "\n");
    }

    // Image
    if (record.imageInUse())
    {
        NdefNfcMimeImageRecord spImageRecord = record.image();
        QByteArray imgFormat = spImageRecord.format();
        if (!imgFormat.isEmpty()) {
            tagContents.append("Image format: " + imgFormat + "\n");
        }
        QImage spImage = spImageRecord.image();
        if (!spImage.isNull())
        {
            if (m_imgCache) {
                const int imgId = m_imgCache->addImage(spImage);
                qDebug() << "Stored image into cache, id: " << imgId;
                emit nfcTagImage(imgId);
            } else {
                qDebug() << "Image cache not set";
            }
        }
    }

    // Raw contents
    //tagContents.append(record.rawContents());
    return tagContents;
}

/*!
  \brief Create a textual description of the contents of the
  image mime type record.

  The parsing works regardless of the actual image format used and
  supports all image formats available to Qt.

  \param record the record to analyze
  \return plain text description of the record contents.
  */
QString NfcNdefParser::parseImageRecord(const NdefNfcMimeImageRecord& record)
{
    QString tagContents("[Image]\n");
    // Read image format (png, gif, jpg, etc.)
    QByteArray imgFormat = record.format();
    if (!imgFormat.isEmpty()) {
        tagContents.append("Format: " + imgFormat + "\n");
    }
    // Retrieve the image
    QImage img = record.image();
    if (!img.isNull()) {
        // Image size
        const QSize imgSize = img.size();
        tagContents.append("Width: " + QString::number(imgSize.width()) + ", height: " + QString::number(imgSize.height()));

        // Store the image in the cache to show it on the screen
        if (m_imgCache) {
            const int imgId = m_imgCache->addImage(img);
            qDebug() << "Stored image into cache, id: " << imgId;
            emit nfcTagImage(imgId);
        } else {
            tagContents.append("Error: Image cache not set\n");
        }
    }

    return tagContents;
}

/*!
  \brief Create a textual description of the contents of the
  Uri record.

  \param record the record to analyze
  \return plain text description of the record contents.
  */
QString NfcNdefParser::parseVcardRecord(NdefNfcMimeVcardRecord& record)
{
    QString tagContents("[vCard]\n");

    // Parse the list of contacts from the record
    QList<QContact> contacts = record.contacts();
    if (!contacts.isEmpty())
    {
        // At least one contact could be successfully retrieved
        // (will in most cases only be one in the NFC scenario)
        foreach (QContact curContact, contacts) {
            //const QString contactDisplayLabel = curContact.displayLabel();
            // Get a list containing all details of the contact, for easier display.
            QList<QContactDetail> details = curContact.details();

            foreach (QContactDetail curDetail, details) {
                // Go through all contact details
                const QString detailName = curDetail.definitionName();
                if (detailName == QContactThumbnail::DefinitionName) {
                    // Special case: image
                    // Fetch the thumbnail and store it in the image cache.
                    QContactThumbnail contactThumb = curContact.detail<QContactThumbnail>();
                    QImage contactThumbImage = contactThumb.thumbnail();
                    if (!contactThumbImage.isNull())
                    {
                        if (m_imgCache) {
                            const int imgId = m_imgCache->addImage(contactThumbImage);
                            qDebug() << "Stored image into cache, id: " << imgId;
                            emit nfcTagImage(imgId);
                        } else {
                            qDebug() << "Image cache not set";
                        }
                    }
                } else {
                    // Any other detail except the image:
                    // add the detail name and its contents to the description.
                    tagContents.append(detailName + ": ");
                    // We just add all values related to the detail converted to a string.
                    QVariantMap valueMap = curDetail.variantValues();
                    foreach (QVariant curValue, valueMap) {

                        tagContents.append(curValue.toString() + " ");
                    }
                    tagContents.append("\n");
                }
            }
        }
    }
    else
    {
        // Error parsing the vCard into a QContact instance
        // Show the error message.s
        QString errorMsg = record.error();
        if (!errorMsg.isEmpty()) {
            tagContents.append(errorMsg);
        }
    }
    return tagContents;
}

/*!
  \brief Return a textual description of the \a typeName of the NDEF record.
  */
QString NfcNdefParser::convertRecordTypeNameToString(const QNdefRecord::TypeNameFormat typeName)
{
    QString typeNameString = "Unknown";
    switch (typeName)
    {
    case QNdefRecord::Empty:
        typeNameString = "Empty NDEF record (does not contain a payload)";
        break;
    case QNdefRecord::NfcRtd:
        typeNameString = "NFC RTD Specification";
        break;
    case QNdefRecord::Mime:
        typeNameString = "RFC 2046 (Mime)";
        break;
    case QNdefRecord::Uri:
        typeNameString = "RFC 3986 (Url)";
        break;
    case QNdefRecord::ExternalRtd:
        typeNameString = "External type name";
        break;
    case QNdefRecord::Unknown:
        typeNameString = "Unknown record type; should be treated similar to content with MIME type 'application/octet-stream' without further context";
        break;
    }
    return typeNameString;
}
