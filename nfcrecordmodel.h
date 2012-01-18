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
#ifndef NFCRECORDMODEL_H
#define NFCRECORDMODEL_H

#include <QAbstractListModel>
#include "nfcrecorditem.h"
#include "nfcmodeltondef.h"
#include <QTimer>
#include <QDebug>
#include <QNdefMessage>
#include <QNdefRecord>
#include <QNdefNfcTextRecord>
#include <QNdefNfcUriRecord>
#include "ndefnfcsprecord.h"
#include "ndefnfcmimeimagerecord.h"
#include "ndefnfcmimevcardrecord.h"

// Forward declarations
class NfcModelToNdef;

class NfcRecordModel : public QAbstractListModel
{
    Q_OBJECT
public:


    explicit NfcRecordModel(QObject *parent = 0);
    ~NfcRecordModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    // Needed so that the QML code can change the model.
    // setData() isn't invokable.
    // see: https://bugreports.qt.nokia.com/browse/QTBUG-7932
    Q_INVOKABLE void setDataValue(int index, const QVariant &value, const QString &role);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QNdefMessage* convertToNdefMessage();
    QModelIndex indexFromItem(const NfcRecordItem *item) const;

    int size() const;

public slots:
    void addCompleteRecordWithDefault(const int messageTypeInt);
    void addRecord(const QString &title, const int messageType, const int recordContent, const QString &currentText, const bool removeVisible, const bool addVisible, const int recordId);
    void addRecord(const QString &title, const NfcTypes::MessageType messageType, const NfcTypes::RecordContent recordContent, const QString &currentText, const bool removeVisible, const bool addVisible, const int recordId);
    void addRecordItem(NfcRecordItem *newRecordItem);

    void insertRecordItem(const int row, NfcRecordItem *newRecordItem);
    /** Creates and inserts a new record item, using defaults for some of the most common info (remove is visible, add is invisible). */
    void simpleInsertRecordItem(const int insertPosition, const NfcTypes::MessageType messageType, const NfcTypes::RecordContent contentType, const bool removeVisible, const int parentRecordId);
    void simpleAppendRecordItem(const NfcTypes::MessageType messageType, const NfcTypes::RecordContent contentType, const bool removeVisible, const int parentRecordId);
    void getDefaultsForRecordContent(const NfcTypes::MessageType msgType, const NfcTypes::RecordContent contentType, QString &defaultTitle, QString &defaultContents);
    QVariantList getSelectionItemsForRecordContent(const NfcTypes::RecordContent contentType, int &defaultSelectedItem);

    void addContentToRecord(int recordIndex, int messageTypeInt, int newContentType);
    bool isContentInRecord(const int recordIndex, const NfcTypes::RecordContent searchForRecordContent);
    bool isContentInRecord(const int recordIndex, const int searchForRecordContent);

    void removeRecord(const int removeRecordIndex);

    int findHeaderForIndex(const int recordIndex);
    int nextAvailableRecordId();

    /**
      \brief Return a list of content items that can potentially be added
      to a record.
      \param recordIndex index of a header record, which will be searched.
      */
    //QList<NfcRecordItem*>*
    QList<QObject*> possibleContentForRecord(int recordIndex);

    //void emitDelayedDataChanged();

private slots:
    void handleItemChange();

private:
    int lastRecordContentIndex(const int recordIndex);
    void removeRecordFromModel(const int removeRecordIndex);
    void checkPossibleContentForRecord(QList<QObject*> &contentList, const bool onlyIfNotYetPresent, const int recordIndex, const NfcTypes::MessageType searchForMsgType, const NfcTypes::RecordContent searchForRecordContent, QString description = "");

signals:
    void recordItemsModified();

private:
    QList<NfcRecordItem*> m_recordItems;
    NfcModelToNdef* m_nfcModelToNdef;
    int m_updatedModelIndex;

};

#endif // NFCRECORDMODEL_H
