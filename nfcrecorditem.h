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
#ifndef NFCRECORDITEM_H
#define NFCRECORDITEM_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QByteArray>
#include <QHash>
#include "nfctypes.h"
#include <QDebug>

class NfcRecordItem : public QObject
{
    Q_OBJECT

    //Q_PROPERTY(QString currentText READ currentText WRITE setCurrentText NOTIFY dataChanged)
public:
    enum RecordRoles {
        TitleRole = Qt::UserRole + 1,
        MessageTypeRole,
        RecordContentRole,
        CurrentTextRole,
        SelectOptionsRole,
        SelectedOptionRole,
        RemoveVisibleRole,
        AddVisibleRole,
        RecordIdRole
    };

public:
    NfcRecordItem(QObject* parent = 0) : QObject(parent) {}
    NfcRecordItem(const QString &title, NfcTypes::MessageType messageType, NfcTypes::RecordContent recordContent, const QString &currentText, bool removeVisible, bool addVisible, int recordId, QObject* parent = 0);

public:
    //QString id() const = 0;
    QVariant data(int role) const;
    void setData(const QVariant& value, const QString& role);
    void setData(const QVariant& value, const int role);
    QHash<int, QByteArray> roleNames() const;

public:
    Q_INVOKABLE QString title() const;
    void setTitle(const QString& title);

    Q_INVOKABLE NfcTypes::MessageType messageType() const;
    void setMessageType(const NfcTypes::MessageType messageType);

    Q_INVOKABLE NfcTypes::RecordContent recordContent() const;
    Q_INVOKABLE int recordContentInt() const;
    void setRecordContent(const NfcTypes::RecordContent recordContent);

    QString currentText() const;
    Q_INVOKABLE void setCurrentText(const QString& currentText);

    QVariantList selectOptions() const;
    void setSelectOptions(const QVariantList selectOptions);

    int selectedOption() const;
    void setSelectedOption(const int selectedOption);

    bool removeVisible() const;
    void setRemoveVisible(const bool removeVisible);

    bool addVisible() const;
    void setAddVisible(const bool addVisible);

    int recordId() const;
    void setRecordId(const int recordId);

    //int getIntForRoleString(const QString &role);
signals:
    void dataChanged();

private:
    QString m_title;
    NfcTypes::MessageType m_messageType;
    NfcTypes::RecordContent m_recordContent;
    QString m_currentText;
    QVariantList m_selectOptions;
    int m_selectedOption;
    bool m_removeVisible;
    bool m_addVisible;
    int m_recordId;
};

// TODO
//Q_DECLARE_METATYPE(NfcRecordItem)

#endif // NFCRECORDITEM_H
