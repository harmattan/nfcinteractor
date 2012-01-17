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

#include "nfcrecordmodel.h"

NfcRecordModel::NfcRecordModel(QObject *parent) :
    QAbstractListModel(parent)
{
    // TODO: don't discard default role names (?)
//    QHash roles = roleNames();
//    roles.insert(FirstNameRole, QByteArray("firstName"));
//    roles.insert(LastNameRole, QByteArray("lastName"));
//    setRoleNames(roles);
//    QHash<int, QByteArray> roles;
//    roles[TitleRole] = "title";
//    roles[MessageTypeRole] = "messageType";
//    roles[RecordContentRole] = "recordContent";
//    roles[DefaultTextRole] = "defaultText";
//    roles[RemoveVisibleRole] = "removeVisible";
//    roles[AddVisibleRole] = "addVisible";
    NfcRecordItem* prototype = new NfcRecordItem(this);
    setRoleNames(prototype->roleNames());
    delete prototype;

    m_nfcModelToNdef = new NfcModelToNdef(m_recordItems, this);
}

NfcRecordModel::~NfcRecordModel()
{
    qDeleteAll(m_recordItems);  // Only required if we store pointers
    m_recordItems.clear();
}

QNdefMessage* NfcRecordModel::convertToNdefMessage()
{
    return m_nfcModelToNdef->convertToNdefMessage();
}

int NfcRecordModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_recordItems.size();
}

QVariant NfcRecordModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_recordItems.size())
        return QVariant();

    return m_recordItems.at(index.row())->data(role);
//    const NfcRecordItem &recordItem = recordItems[index.row()];
//    switch (role) {
//    case TitleRole:
//        return recordItem.title();
//    case MessageTypeRole:
//        return recordItem.messageType();
//    case RecordContentRole:
//        return recordItem.recordContent();
//    case DefaultTextRole:
//        return recordItem.defaultText();
//    case RemoveVisibleRole:
//        return recordItem.removeVisible();
//    case AddVisibleRole:
//        return recordItem.addVisible();
//    default:
//        return QVariant();
//    }
}

QModelIndex NfcRecordModel::indexFromItem(const NfcRecordItem *item) const
{
    Q_ASSERT(item);
    for(int row = 0; row < m_recordItems.size(); ++row) {
        if(m_recordItems.at(row) == item) return index(row);
    }
    return QModelIndex();
}

void NfcRecordModel::handleItemChange()
{
    NfcRecordItem* item = static_cast<NfcRecordItem*>(sender());
    QModelIndex index = indexFromItem(item);
    qDebug() << "NfcRecordModel::handleItemChange(): " << m_recordItems.at(index.row())->defaultText();
    if(index.isValid()) {
        qDebug() << "NfcRecordModel::handleItemChange: emitting dataChanged signal";
        emit dataChanged(index, index);
        emit recordItemsModified();
    }
}

void NfcRecordModel::addRecord(const QString &title, const int messageType, const int recordContent, const QString &defaultText, const bool removeVisible, const bool addVisible, const int recordId)
{
    addRecord(title, (NfcTypes::MessageType)messageType, (NfcTypes::RecordContent)recordContent, defaultText, removeVisible, addVisible, recordId);
}

void NfcRecordModel::addRecord(const QString &title, const NfcTypes::MessageType messageType, const NfcTypes::RecordContent recordContent, const QString &defaultText, const bool removeVisible, const bool addVisible, const int recordId)
{
    NfcRecordItem* newRecordItem = new NfcRecordItem(title, messageType, recordContent, defaultText, removeVisible, addVisible, recordId, this);
    addRecordItem(newRecordItem);
}

void NfcRecordModel::addRecordItem(NfcRecordItem *newRecordItem)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    connect(newRecordItem, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_recordItems.append(newRecordItem);
    endInsertRows();
    //qDebug() << "New item, message type = " << newRecordItem->messageType();
    emit recordItemsModified();
}

void NfcRecordModel::insertRecordItem(const int row, NfcRecordItem *newRecordItem)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(newRecordItem, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_recordItems.insert(row, newRecordItem);
    endInsertRows();
    //qDebug() << "New item, message type = " << newRecordItem->messageType();
    emit recordItemsModified();
}

void NfcRecordModel::addCompleteRecordWithDefault(const int messageTypeInt)
{
    // Get next available record ID
    const int recordId = nextAvailableRecordId();
    const NfcTypes::MessageType messageType = (NfcTypes::MessageType) messageTypeInt;
    //qDebug() << "Add new complete record with default content for type: " << messageTypeInt;
    switch (messageType) {
    case NfcTypes::MsgSmartPoster:
        addRecord("Smart Poster Record", NfcTypes::MsgSmartPoster, NfcTypes::RecordHeader, "", true, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgSmartPoster, NfcTypes::RecordUri, false,  recordId);
        simpleAppendRecordItem(NfcTypes::MsgSmartPoster, NfcTypes::RecordText, true,  recordId);
        simpleAppendRecordItem(NfcTypes::MsgSmartPoster, NfcTypes::RecordTextLanguage, false,  recordId);
        break;
    case NfcTypes::MsgUri:
        addRecord("URI Record", NfcTypes::MsgUri, NfcTypes::RecordHeader, "", true, false, recordId);
        simpleAppendRecordItem(NfcTypes::MsgUri, NfcTypes::RecordUri, false,  recordId);
        break;
    case NfcTypes::MsgText:
        addRecord("Text Record", NfcTypes::MsgText, NfcTypes::RecordHeader, "", true, false, recordId);
        simpleAppendRecordItem(NfcTypes::MsgText, NfcTypes::RecordText, false,  recordId);
        simpleAppendRecordItem(NfcTypes::MsgText, NfcTypes::RecordTextLanguage, false,  recordId);
        break;
    case NfcTypes::MsgSms:
        addRecord("SMS Record", NfcTypes::MsgSms, NfcTypes::RecordHeader, "", true, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgSms, NfcTypes::RecordPhoneNumber, false,  recordId);
        simpleAppendRecordItem(NfcTypes::MsgSms, NfcTypes::RecordSmsBody, false,  recordId);
        break;
    case NfcTypes::MsgBusinessCard:
        addRecord("Business Card", NfcTypes::MsgBusinessCard, NfcTypes::RecordHeader, "", true, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgBusinessCard, NfcTypes::RecordFirstName, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgBusinessCard, NfcTypes::RecordLastName, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgBusinessCard, NfcTypes::RecordPhoneNumber, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgBusinessCard, NfcTypes::RecordEmailAddress, true, recordId);
        break;
    case NfcTypes::MsgSocialNetwork:
        addRecord("Social Network", NfcTypes::MsgSocialNetwork, NfcTypes::RecordHeader, "", true, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgSocialNetwork, NfcTypes::RecordSocialNetworkType, false, recordId);
        simpleAppendRecordItem(NfcTypes::MsgSocialNetwork, NfcTypes::RecordSocialNetworkName, false, recordId);
        break;
    case NfcTypes::MsgGeo: {
        addRecord("Geo Record", NfcTypes::MsgGeo, NfcTypes::RecordHeader, "", true, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgGeo, NfcTypes::RecordGeoType, false, recordId);
        simpleAppendRecordItem(NfcTypes::MsgGeo, NfcTypes::RecordGeoLatitude, false, recordId);
        simpleAppendRecordItem(NfcTypes::MsgGeo, NfcTypes::RecordGeoLongitude, false, recordId);
        break; }
    case NfcTypes::MsgStore: {
        addRecord("Store Record", NfcTypes::MsgStore, NfcTypes::RecordHeader, "", true, true, recordId);
#ifdef MEEGO_EDITION_HARMATTAN
        simpleAppendRecordItem(NfcTypes::MsgStore, NfcTypes::RecordStoreMeeGoHarmattan, true, recordId);
#else
        simpleAppendRecordItem(NfcTypes::MsgStore, NfcTypes::RecordStoreSymbian, true, recordId);
#endif
        break; }
    case NfcTypes::MsgImage:
        // TODO: not supported yet
        break;
    case NfcTypes::MsgAnnotatedUrl:
        addRecord("URI Record", NfcTypes::MsgUri, NfcTypes::RecordHeader, "", true, false, recordId);
        simpleAppendRecordItem(NfcTypes::MsgUri, NfcTypes::RecordUri, false,  recordId);
        addRecord("Text Record", NfcTypes::MsgText, NfcTypes::RecordHeader, "", true, false, recordId);
        simpleAppendRecordItem(NfcTypes::MsgText, NfcTypes::RecordText, false,  recordId);
        simpleAppendRecordItem(NfcTypes::MsgText, NfcTypes::RecordTextLanguage, false,  recordId);
        break;
    case NfcTypes::MsgCustom:
        addRecord("Custom Record", NfcTypes::MsgCustom, NfcTypes::RecordHeader, "", true, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgCustom, NfcTypes::RecordTypeNameFormat, false,  recordId);
        simpleAppendRecordItem(NfcTypes::MsgCustom, NfcTypes::RecordTypeName, true,  recordId);
        break;
    case NfcTypes::MsgCombination:
        addRecord("Custom Record", NfcTypes::MsgCustom, NfcTypes::RecordHeader, "", true, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgCustom, NfcTypes::RecordTypeNameFormat, false,  recordId);
        simpleAppendRecordItem(NfcTypes::MsgCustom, NfcTypes::RecordTypeName, true,  recordId);
        addRecord("URI Record", NfcTypes::MsgUri, NfcTypes::RecordHeader, "", true, false, recordId);
        simpleAppendRecordItem(NfcTypes::MsgUri, NfcTypes::RecordUri, false,  recordId);
        break;
    case NfcTypes::MsgNfcAutostart:
        addRecord("Custom Record", NfcTypes::MsgCustom, NfcTypes::RecordHeader, "", true, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgCustom, NfcTypes::RecordTypeNameFormat, false,  recordId);
        simpleAppendRecordItem(NfcTypes::MsgCustom, NfcTypes::RecordTypeName, true,  recordId);
        m_recordItems.last()->setDefaultText("nokia.com:nfcinteractor");
        addRecord("Smart Poster Record", NfcTypes::MsgSmartPoster, NfcTypes::RecordHeader, "", true, true, recordId);
        simpleAppendRecordItem(NfcTypes::MsgSmartPoster, NfcTypes::RecordUri, false,  recordId);
        m_recordItems.last()->setDefaultText("http://nfcinteractor.com");
        simpleAppendRecordItem(NfcTypes::MsgSmartPoster, NfcTypes::RecordText, true,  recordId);
        m_recordItems.last()->setDefaultText("Download Nfc Interactor");
        simpleAppendRecordItem(NfcTypes::MsgSmartPoster, NfcTypes::RecordTextLanguage, false,  recordId);
        // TODO: when app is in the store, change to store record
//        addRecord("Store Record", NfcTypes::MsgStore, NfcTypes::RecordHeader, "", true, true, recordId);
//        simpleAppendRecordItem(NfcTypes::MsgStore, NfcTypes::RecordStoreMeeGoHarmattan, true, recordId);
//        m_recordItems[m_recordItems.count()]->setDefaultText("");
//        simpleAppendRecordItem(NfcTypes::MsgStore, NfcTypes::RecordStoreSymbian, true, recordId);
//        m_recordItems[m_recordItems.count()]->setDefaultText("");
//        simpleAppendRecordItem(NfcTypes::MsgStore, NfcTypes::RecordText, true,  recordId);
//        m_recordItems.last()->setDefaultText("Download Nfc Interactor");
//        simpleAppendRecordItem(NfcTypes::MsgStore, NfcTypes::RecordTextLanguage, false,  recordId);
        break;
    }
}


bool NfcRecordModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    qDebug() << "NfcRecordModel::setData()";
    if (index.isValid() && role == Qt::EditRole) {
        m_recordItems.at(index.row())->setData(value, role);
        //emit dataChanged(index, index);  //  Not needed, emitted by the item
        return true;
    }
    return false;
}

void NfcRecordModel::setDataValue(int index, const QVariant &value, const QString &role)
{
    //qDebug() << "NfcRecordModel::setDataValue (before): index = " << index << ", value = " << value << ", role = " << role;
    //qDebug() << "Current text: " << m_recordItems.at(index)->defaultText();
    if (index < 0 || index >= m_recordItems.size())
        return;

    m_recordItems.at(index)->setData(value, role);

    //qDebug() << "NfcRecordModel::setDataValue (after): " << m_recordItems.at(index)->defaultText();
}

Qt::ItemFlags NfcRecordModel::flags ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

void NfcRecordModel::addContentToRecord(int recordIndex, int messageTypeInt, int contentTypeInt)
{
    if (recordIndex < 0 || recordIndex > m_recordItems.count())
        return;

    qDebug() << "Add content to record";
    // Search the end of the record, so that we can add the new info at the end
    const int parentRecordId = m_recordItems[recordIndex]->recordId();
    qDebug() << "Parent record index: " << recordIndex << ", id: " << parentRecordId;
    const int insertPosition = lastRecordContentIndex(recordIndex);
    const NfcTypes::MessageType messageType = (NfcTypes::MessageType) messageTypeInt;
    const NfcTypes::RecordContent contentType = (NfcTypes::RecordContent) contentTypeInt;
    switch (contentType) {
    case NfcTypes::RecordText:
    {
        // Add two items for the text record, therefore handle it here
        // and not in the generic simpleInsertRecordItem() method.
        simpleInsertRecordItem(insertPosition, messageType, NfcTypes::RecordTextLanguage, false, parentRecordId);
        simpleInsertRecordItem(insertPosition, messageType, NfcTypes::RecordText, true, parentRecordId);
        break;
    }
    case NfcTypes::RecordHeader:
        // Shouldn't be possible to add a header as content to an existing record
        qDebug() << "Error: attempt to add a header as a content for another record";
        break;
    default:
    {
        simpleInsertRecordItem(insertPosition, messageType, contentType, true, parentRecordId);
        break;
    }

    }
    // Check if another content item can potentially be added to the record
    if (recordIndex >= 0) {
        QList<QObject*> possibleContent = possibleContentForRecord(recordIndex);
        if (possibleContent.size() == 0) {
            // It is possible to add content items to the record
            // -> enable the add button for the header
            m_recordItems[recordIndex]->setAddVisible(false);
        }
    }
}

void NfcRecordModel::simpleInsertRecordItem(const int insertPosition, const NfcTypes::MessageType messageType, const NfcTypes::RecordContent contentType, const bool removeVisible, const int parentRecordId)
{
    QString defaultTitle;
    QString defaultContents;
    getDefaultsForRecordContent(messageType, contentType, defaultTitle, defaultContents);
    NfcRecordItem* newRecordItem = new NfcRecordItem(defaultTitle, messageType, contentType, defaultContents, removeVisible, false, parentRecordId, this);
    if (contentType == NfcTypes::RecordSpAction ||
            contentType == NfcTypes::RecordGeoType ||
            contentType == NfcTypes::RecordTypeNameFormat ||
            contentType == NfcTypes::RecordSocialNetworkType) {
        // Selection item - also add the selection options to the item
        int defaultSelectedItem = 0;
        QVariantList selectionItems = getSelectionItemsForRecordContent(contentType, defaultSelectedItem);
        newRecordItem->setSelectOptions(selectionItems);
        newRecordItem->setSelectedOption(defaultSelectedItem);
    }
    insertRecordItem(insertPosition, newRecordItem);
}


void NfcRecordModel::simpleAppendRecordItem(const NfcTypes::MessageType messageType, const NfcTypes::RecordContent contentType, const bool removeVisible, const int parentRecordId)
{
    simpleInsertRecordItem(rowCount(), messageType, contentType, removeVisible, parentRecordId);
}


/*!
  \brief Return the model index after the last entry of the specified record.
  This can be used to insert a new content item at the end of the record.
  */
int NfcRecordModel::lastRecordContentIndex(const int recordIndex) {
    const int searchForRecordId = m_recordItems[recordIndex]->recordId();
    int curSearchIndex = recordIndex;
    while (curSearchIndex < m_recordItems.count())
    {
        if (m_recordItems[curSearchIndex]->recordId() != searchForRecordId) {
            return curSearchIndex;
        }
        curSearchIndex++;
    }
    // Reached the end of the model, so this record was the last
    // Return the end index of the list (== count)
    return curSearchIndex;
}



bool NfcRecordModel::isContentInRecord(const int recordIndex, const int searchForRecordContent) {
    return isContentInRecord(recordIndex, (NfcTypes::RecordContent)searchForRecordContent);
}

/*!
  \brief Check if the record UI already contains the specified content item.
  \a recordIndex index of the parent record, whose children should be searched
  \a searchForRecordContent record content type to check if available in the parent record.
  \return true if the specified record content type is already present in the parent record.
  */
bool NfcRecordModel::isContentInRecord(const int recordIndex, const NfcTypes::RecordContent searchForRecordContent) {
    if (recordIndex < 0 || recordIndex > m_recordItems.size())
        return true;

    int curIndex = recordIndex;
    int recordId = m_recordItems[recordIndex]->recordId();
    while (curIndex < m_recordItems.size()) {
        if (m_recordItems[curIndex]->recordId() != recordId) {
            // Found a content item already belonging to a different record ID
            // -> no, the specified record doesn't contain the UI for the
            // specified record content yet
            return false;
        }
        if (m_recordItems[curIndex]->recordContent() == searchForRecordContent) {
            // Found the content item we're looking for!
            return true;
        }
        curIndex ++;
    }
    return false;
}

/*!
  \brief Return the model index of the header record item for the given index.
  If the specified index is already a header, the same index is returned.
  */
int NfcRecordModel::findHeaderForIndex(const int recordIndex) {
    for (int curIndex = recordIndex; curIndex >= 0; curIndex --) {
        if (m_recordItems[curIndex]->recordContent() == NfcTypes::RecordHeader) {
            return curIndex;
        }
    }
    // If the UI doesn't contain an error, it shouldn't be possible not to find
    // a header item
    return -1;
}
/*!
  \brief Return the next unused record id that can be used to add a new record to the UI.
  This method assumes that record ids are sorted, and will return the id of the last
  item in the model plus 1.
  */
int NfcRecordModel::nextAvailableRecordId() {
    const int numRecords = m_recordItems.count();
    if (numRecords > 0) {
        return m_recordItems[numRecords - 1]->recordId() + 1;
    } else {
        return 0;
    }
}


/*!
  \brief Remove the specified record / record content item from the UI.
  */
void NfcRecordModel::removeRecord(const int removeRecordIndex) {
    const NfcTypes::RecordContent recordContent = m_recordItems[removeRecordIndex]->recordContent();
    const int recordId = m_recordItems[removeRecordIndex]->recordId();
    const int recordHeaderIndex = findHeaderForIndex(removeRecordIndex);

    removeRecordFromModel(removeRecordIndex);
    // If it was a header, also remove all its children with the same record id
    // (or until the next header is found)
    if (recordContent == NfcTypes::RecordHeader) {
        while (m_recordItems.size() > removeRecordIndex) {
            if (m_recordItems[removeRecordIndex]->recordContent() == NfcTypes::RecordHeader) {
                break;
            } else if (m_recordItems[removeRecordIndex]->recordId() == recordId) {
                removeRecordFromModel(removeRecordIndex);
            }
        }
    } else {
        if (recordContent == NfcTypes::RecordText) {
            // If we just deleted a text entry, also delete the following text language (if present)
            if (m_recordItems.count() > removeRecordIndex && m_recordItems[removeRecordIndex]->recordContent() == NfcTypes::RecordTextLanguage)
            {
                removeRecordFromModel(removeRecordIndex);
            }
        }

        // Check if it is possible to add items to the record now
        // If yes, we might need to re-enable the add button of the header.
        if (recordHeaderIndex >= 0) {
            QList<QObject*> possibleContent = possibleContentForRecord(recordHeaderIndex);
            if (possibleContent.size() > 0) {
                // It is possible to add content items to the record
                // -> enable the add button for the header
                m_recordItems[recordHeaderIndex]->setAddVisible(true);
            }
        }
    }
    // Note: Don't remove record if all its content is empty

}

void NfcRecordModel::removeRecordFromModel(const int removeRecordIndex)
{
    if (m_recordItems.size() > removeRecordIndex) {
        beginRemoveRows(QModelIndex(), removeRecordIndex, removeRecordIndex);
        m_recordItems.removeAt(removeRecordIndex);
        endRemoveRows();
        emit recordItemsModified();
    }
}

int NfcRecordModel::size() const
{
    return m_recordItems.size();
}


QList<QObject*> NfcRecordModel::possibleContentForRecord(int recordIndex)
{
    QList<QObject*> possibleContent;
    if (recordIndex < 0 || recordIndex > m_recordItems.size() ||
            m_recordItems[recordIndex]->recordContent() != NfcTypes::RecordHeader)
        return possibleContent;

    const NfcTypes::MessageType messageType = m_recordItems[recordIndex]->messageType();
    switch (messageType) {
    case NfcTypes::MsgUri:
    case NfcTypes::MsgText:
    case NfcTypes::MsgImage:
    case NfcTypes::MsgAnnotatedUrl:
    case NfcTypes::MsgCombination:
    case NfcTypes::MsgNfcAutostart:
        // No content can be added/removed to these records
        break;
    case NfcTypes::MsgSmartPoster:
    {
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordUri);
        checkPossibleContentForRecord(possibleContent, false, recordIndex, messageType, NfcTypes::RecordText);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordSpAction);
        //checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordImageFilename);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordSpType);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordSpSize);
        break;
    }
    case NfcTypes::MsgBusinessCard:
    {
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordNamePrefix);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordFirstName);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordMiddleName);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordLastName);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordNameSuffix);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordNickname);

        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordEmailAddress);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordPhoneNumber);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordContactUrl);

        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordOrganizationName);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordOrganizationDepartment);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordOrganizationRole);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordOrganizationTitle);

        //RecordBirthday, // not implemented yet, would require a date editor field
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordNote);

        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordCountry);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordLocality);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordPostOfficeBox);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordPostcode);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordRegion);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordStreet);
        break;
    }
    case NfcTypes::MsgSms:
    case NfcTypes::MsgGeo:
    case NfcTypes::MsgSocialNetwork:
    {
        // Smart URI types - can convert to a Smart Poster record, by default URI record.
        checkPossibleContentForRecord(possibleContent, false, recordIndex, messageType, NfcTypes::RecordText);
        break;
    }
    case NfcTypes::MsgStore:
    {
        // Smart URI types - can convert to a Smart Poster record, by default URI record.
        checkPossibleContentForRecord(possibleContent, false, recordIndex, messageType, NfcTypes::RecordText);
        if (!isContentInRecord(recordIndex, NfcTypes::RecordStoreSymbian) &&
                !isContentInRecord(recordIndex, NfcTypes::RecordStoreMeeGoHarmattan) &&
                !isContentInRecord(recordIndex, NfcTypes::RecordStoreSeries40)) {
            // Generic Nokia Store link only available when no Symbian, MeeGo Harmattan or Series 40 specific ID is here
            checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordStoreNokia);
        }
        if (!isContentInRecord(recordIndex, NfcTypes::RecordStoreNokia)) {
            // Symbian, MeeGo Harmattan and Series 40 links only possible if no generic
            // Nokia app id is present
            checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordStoreSymbian);
            checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordStoreMeeGoHarmattan);
            checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordStoreSeries40);
        }
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordStoreWindowsPhone);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordStoreAndroid);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordStoreiOS);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordStoreBlackberry);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordStoreCustomName);
        break;
    }
    case NfcTypes::MsgCustom:
    {
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordTypeName);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordRawPayload);
        checkPossibleContentForRecord(possibleContent, true, recordIndex, messageType, NfcTypes::RecordId);
        break;
    }
    }
    qDebug() << "Number of possible additional content items (C++): " << possibleContent.size();
    return possibleContent;
}



/*!
  \brief Add an item to the model of the content item dialog.
  \param onlyIfNotYetPresent check the current record and only allow adding the new
  content item if the record doesn't already show the UI for that content item.
  */
void NfcRecordModel::checkPossibleContentForRecord(QList<QObject*> &contentList, const bool onlyIfNotYetPresent, const int recordIndex, const NfcTypes::MessageType searchForMsgType, const NfcTypes::RecordContent searchForRecordContent, QString description)
{
    if (!onlyIfNotYetPresent || !isContentInRecord(recordIndex, searchForRecordContent)) {
        if (description.isEmpty()) {
            // No description provided - use the defaulf for this record content type
            QString defaultContents;
            getDefaultsForRecordContent(searchForMsgType, searchForRecordContent, description, defaultContents);
        }
        contentList.append(new NfcRecordItem(description, searchForMsgType, searchForRecordContent, QString(), false, false, -1, this));
    }

}

void NfcRecordModel::getDefaultsForRecordContent(const NfcTypes::MessageType msgType, const NfcTypes::RecordContent contentType, QString& defaultTitle, QString& defaultContents)
{
    defaultTitle = "";
    defaultContents = "";
    switch (contentType) {
    case NfcTypes::RecordUri:
        defaultTitle = "URI";
        defaultContents = "http://nokia.com";
        break;
    case NfcTypes::RecordText: {
        if (msgType == NfcTypes::MsgText) {
            defaultTitle = "Text";
        } else {
            defaultTitle = "Title text";
        }
        switch (msgType) {
        case NfcTypes::MsgSms:
            defaultContents = "Send SMS";
            break;
        case NfcTypes::MsgSocialNetwork:
            defaultContents = "Follow me";
            break;
        case NfcTypes::MsgGeo:
            defaultContents = "View location";
            break;
        case NfcTypes::MsgStore:
            defaultContents = "Download app";
            break;
        default:
            defaultContents = "Nokia";
        }
        break; }
    case NfcTypes::RecordTextLanguage:
        defaultTitle = "Language";
        defaultContents = "en";
        break;
    case NfcTypes::RecordSmsBody:
        defaultTitle = "SMS Body";
        defaultContents = "Hello";
        break;
    case NfcTypes::RecordSpAction:
        defaultTitle = "Action";
        // Selection item - no default contents string
        break;
    case NfcTypes::RecordImageFilename:
        defaultTitle = "Image filename";
        break;
    case NfcTypes::RecordSpType:
        defaultTitle = "Type of linked content";
        defaultContents = "text/html";
        break;
    case NfcTypes::RecordSpSize:
        defaultTitle = "Size of linked content";
        defaultContents = "0";
        break;
    case NfcTypes::RecordTypeNameFormat:
        defaultTitle = "Type Name Format";
        // Selection item - no default contents string
        break;
    case NfcTypes::RecordTypeName:
        defaultTitle = "Type Name";
        defaultContents = "nokia.com:custom";
        break;
    case NfcTypes::RecordId:
        defaultTitle = "Id";
        break;
    case NfcTypes::RecordRawPayload:
        defaultTitle = "Raw Payload";
        defaultContents = "Nokia";
        break;
    case NfcTypes::RecordSocialNetworkType:
        defaultTitle = "Social Network";
        break;
    case NfcTypes::RecordSocialNetworkName:
        defaultTitle = "User name";
        defaultContents = "mopius";
        break;
        // ----------------------------------------------------------------
        // Contacts
    case NfcTypes::RecordNamePrefix:
        defaultTitle = "Name Prefix";
        break;
    case NfcTypes::RecordFirstName:
        defaultTitle = "First name";
        defaultContents = "Joe";
        break;
    case NfcTypes::RecordMiddleName:
        defaultTitle = "Middle name";
        break;
    case NfcTypes::RecordLastName:
        defaultTitle = "Last name";
        defaultContents = "Bloggs";
        break;
    case NfcTypes::RecordNameSuffix:
        defaultTitle = "Name suffix";
        defaultContents = "";
        break;
    case NfcTypes::RecordNickname:
        defaultTitle = "Nickname";
        break;
    case NfcTypes::RecordEmailAddress:
        defaultTitle = "Email address";
        defaultContents = "joe.bloggs@nokia.com";
        break;
    case NfcTypes::RecordPhoneNumber:
        defaultTitle = "Phone number";
        defaultContents = "+1234";
        break;
    case NfcTypes::RecordContactUrl:
        defaultTitle = "URL";
        defaultContents = "http://developer.nokia.com/";
        break;
    case NfcTypes::RecordOrganizationName:
        defaultTitle = "Organization name";
        break;
    case NfcTypes::RecordOrganizationDepartment:
        defaultTitle = "Organization department";
        break;
    case NfcTypes::RecordOrganizationRole:
        defaultTitle = "Organization role";
        break;
    case NfcTypes::RecordOrganizationTitle:
        defaultTitle = "Organization title";
        break;
    case NfcTypes::RecordBirthday:
        defaultTitle = "Birthday (YYYY-MM-DD)";
        defaultContents = "1980-03-25";
        break;
    case NfcTypes::RecordNote:
        defaultTitle = "Note";
        break;
    case NfcTypes::RecordCountry:
        defaultTitle = "Country";
        break;
    case NfcTypes::RecordLocality:
        defaultTitle = "Locality";
        break;
    case NfcTypes::RecordPostOfficeBox:
        defaultTitle = "Post Office Box";
        break;
    case NfcTypes::RecordPostcode:
        defaultTitle = "Postcode";
        break;
    case NfcTypes::RecordRegion:
        defaultTitle = "Region";
        break;
    case NfcTypes::RecordStreet:
        defaultTitle = "Street";
        break;
        // ----------------------------------------------------------------
        // Geo
    case NfcTypes::RecordGeoType:
        defaultTitle = "Geo Tag Type";
        // Selection item - no default contents string
        break;
    case NfcTypes::RecordGeoLatitude:
        defaultTitle = "Latitude (dec deg., WGS-84)";
        defaultContents = "60.17";
        break;
    case NfcTypes::RecordGeoLongitude:
        defaultTitle = "Longitude (dec deg., WGS-84)";
        defaultContents = "24.829";
        break;
        // ----------------------------------------------------------------
        // Store
    case NfcTypes::RecordStoreNokia:
        defaultTitle = "Nokia Store generic ID";
        // TODO: add id of Nfc Interactor by default, once known
        break;
    case NfcTypes::RecordStoreSymbian:
        defaultTitle = "Symbian ID in Nokia Store";
        defaultContents = "184295";
        break;
    case NfcTypes::RecordStoreMeeGoHarmattan:
        defaultTitle = "MeeGo ID in Nokia Store";
        defaultContents = "214283";
        break;
    case NfcTypes::RecordStoreSeries40:
        defaultTitle = "Series 40 ID in Nokia Store";
        break;
    case NfcTypes::RecordStoreWindowsPhone:
        defaultTitle = "Windows Phone Marketplace ID";
        break;
    case NfcTypes::RecordStoreAndroid:
        defaultTitle = "Android Marketplace ID";
        break;
    case NfcTypes::RecordStoreiOS:
        defaultTitle = "iOS App Store ID";
        break;
    case NfcTypes::RecordStoreBlackberry:
        defaultTitle = "BlackBerry App World ID";
        break;
    case NfcTypes::RecordStoreCustomName:
        defaultTitle = "nfcinteractor.com app name";
        defaultContents = "nfcinteractor";
        break;
    default:
        qDebug() << "Warning: don't have defaults for requested content type in NfcRecordModel::getDefaultsForRecordContent().";
        break;
    }
}

QVariantList NfcRecordModel::getSelectionItemsForRecordContent(const NfcTypes::RecordContent contentType, int& defaultSelectedItem)
{
    QVariantList selectionItems;
    defaultSelectedItem = 0;
    switch (contentType) {
    case NfcTypes::RecordSpAction:
        selectionItems << "Do the action";
        selectionItems << "Save for later";
        selectionItems << "Open for editing";
        break;
    case NfcTypes::RecordGeoType:
        selectionItems << "geo: URI scheme";
        selectionItems << "Nokia Maps link";
        selectionItems << "Generic redirect NfcInteractor.com";
        break;
    case NfcTypes::RecordSocialNetworkType:
        selectionItems << "Twitter";
        selectionItems << "LinkedIn";
        selectionItems << "Facebook";
        selectionItems << "Xing";
        selectionItems << QString::fromWCharArray(L"\x0412\x041A\x043E\x043D\x0442\x0430\x043A\x0442\x0435");
        break;
    case NfcTypes::RecordTypeNameFormat:
        selectionItems << "Empty";
        selectionItems << "NfcRtd";
        selectionItems << "Mime";
        selectionItems << "Uri";
        selectionItems << "ExternalRtd";
        selectionItems << "Unknown";
        defaultSelectedItem = 4;
        break;
    default:
        qDebug() << "Warning: don't have defaults for requested content type in NfcRecordModel::getDefaultSelectionItemsForRecordContent().";
        break;
    }
    return selectionItems;



}

