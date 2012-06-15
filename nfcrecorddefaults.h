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
#ifndef NFCRECORDDEFAULTS_H
#define NFCRECORDDEFAULTS_H

#include <QObject>
#include "nfctypes.h"

class NfcRecordDefaults : public QObject
{
    Q_OBJECT
public:
    explicit NfcRecordDefaults(QObject *parent = 0);

public:
    QString itemHeaderTextDefault(const NfcTypes::MessageType messageType);
    void itemContentDefault(const NfcTypes::MessageType msgType, const NfcTypes::RecordContent contentType, QString &defaultTitle, QString &defaultContents);
    QVariantList itemSelectionDefault(const NfcTypes::RecordContent contentType, int &defaultSelectedItem);
};

#endif // NFCRECORDDEFAULTS_H
