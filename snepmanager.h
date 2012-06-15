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
#ifndef SNEPMANAGER_H
#define SNEPMANAGER_H

#include <QObject>
#include <QDebug>
#include <QNdefMessage>

// The Version field is a single octet representing a
// structure of two 4-bit unsigned integers. The most significant 4 bits SHALL denote the major
// protocol version. The least significant 4 bits SHALL denote the minor protocol version.
#define SNEP_VERSION        (quint8)0x10
// Request
#define SNEP_REQ_CONTINUE   (quint8)0x0
#define SNEP_REQ_GET        (quint8)0x1
#define SNEP_REQ_PUT        (quint8)0x2
#define SNEP_REQ_REJECT     (quint8)0x7F
// Response
#define SNEP_RES_CONTINUE   (quint8)0x80
#define SNEP_RES_SUCCESS    (quint8)0x81
#define SNEP_RES_NOTFOUND   (quint8)0xC0
#define SNEP_RES_EXCESSDATA (quint8)0xC1
#define SNEP_RES_BADREQUEST (quint8)0xC2
#define SNEP_RES_NOTIMPLEMENTED (quint8)0xE0
#define SNEP_RES_UNSUPPORTEDVERSION (quint8)0xE1
#define SNEP_RES_REJECT     (quint8)0xFF

QTM_USE_NAMESPACE

class SnepManager : public QObject
{
    Q_OBJECT
public:
    explicit SnepManager(QObject *parent = 0);
    
    QByteArray wrapNdefInSnepPut(const QNdefMessage* ndefMessage);
    QNdefMessage analyzeSnepMessage(QByteArray &rawMessage, QString &results);
signals:
    void nfcSnepSuccess();
    
public slots:
private:
    QString convertSnepCommandToText(quint8 command);

};

#endif // SNEPMANAGER_H
