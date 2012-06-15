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

#include "snepmanager.h"

SnepManager::SnepManager(QObject *parent) :
    QObject(parent)
{
}

QByteArray SnepManager::wrapNdefInSnepPut(const QNdefMessage* ndefMessage)
{
    // Create SNEP header
    // ------------------
    QByteArray snepMsg;
    QDataStream snepStream(&snepMsg, QIODevice::WriteOnly);
    QByteArray rawMessage = ndefMessage->toByteArray();

    // Version (two 4-bit unsigned integers)
    snepStream << SNEP_VERSION;

    // Request Field (8-bit unsigned integer)
    snepStream << SNEP_REQ_PUT;

    // Size (32-bit unsigned integer)
    snepStream << (quint32)rawMessage.size();

    // Information (x bytes)
    snepMsg.append(rawMessage);

    qDebug() << "SNEP message: " << snepMsg;
    qDebug() << "SNEP total size: " << snepMsg.size();

    QString arrayContents = "";
    for (int i = 0; i < snepMsg.size(); ++i) {
        arrayContents.append(QString("0x") + QString::number(snepMsg.at(i), 16) + " ");
    }
    qDebug() << "SNEP Raw contents:\n" << arrayContents;

    return snepMsg;
}

QNdefMessage SnepManager::analyzeSnepMessage(QByteArray& rawMessage, QString& results)
{
    // Version
    QDataStream snepStream(&rawMessage, QIODevice::ReadOnly);

    quint8 version;
    snepStream >> version;

    if (version != SNEP_VERSION)
    {
        // TODO: Parse major + minor SNEP version
        results.append("Unsupported SNEP version (" + QString::number(version) + ")\n");
        return QNdefMessage();
    }

    const int protocolHeaderSize = 6;   // version (1b) + command (1b) + length (4b) = 6 bytes

    // Command
    quint8 command;
    snepStream >> command;
    results.append("SNEP: " + convertSnepCommandToText(command) + "\n");

    // Length
    quint32 length;
    snepStream >> length;
    if (length > 0)
    {
        results.append("Length: " + QString::number(length) + " Bytes");

        QByteArray snepContents;
        snepStream >> snepContents;

        // Read contents
        if (command == SNEP_REQ_PUT) {
            // Read NDEF message and send to
            // NfcInfo::ndefMessageRead
            QNdefMessage containedNdef = QNdefMessage::fromByteArray(rawMessage.mid(protocolHeaderSize));
            qDebug() << "NDEF message received a " << containedNdef.count() << " records";

            // TODO: send back success
            return containedNdef;
        }
        else
        {
            // Read raw data
            results.append("Contents: ");
            results.append(rawMessage.mid(protocolHeaderSize) + "\n");
        }

    }

    // If success (writing), inform UI
    if (command == SNEP_RES_SUCCESS)
    {
        emit nfcSnepSuccess();
    }

    return QNdefMessage();
}


QString SnepManager::convertSnepCommandToText(quint8 command)
{
    QString cmdTxt;
    switch (command)
    {
    case SNEP_REQ_CONTINUE:
        cmdTxt.append("Send remaining fragments");
        break;
    case SNEP_REQ_GET:
        cmdTxt.append("Return an NDEF message");
        break;
    case SNEP_REQ_PUT:
        cmdTxt.append("Accept an NDEF message");
        break;
    case SNEP_REQ_REJECT:
        cmdTxt.append("Do not send remaining fragments");
        break;
    case SNEP_RES_CONTINUE:
        cmdTxt.append("Continue sending remaining fragments");
        break;
    case SNEP_RES_SUCCESS:
        cmdTxt.append("Operation succeeded");
        break;
    case SNEP_RES_NOTFOUND:
        cmdTxt.append("Resource not found");
        break;
    case SNEP_RES_EXCESSDATA:
        cmdTxt.append("Resource exceeds data size limit");
        break;
    case SNEP_RES_BADREQUEST:
        cmdTxt.append("Malformed request not understood");
        break;
    case SNEP_RES_NOTIMPLEMENTED:
        cmdTxt.append("Unsupported functionality requested");
        break;
    case SNEP_RES_UNSUPPORTEDVERSION:
        cmdTxt.append("Unsupported protocol version");
        break;
    case SNEP_RES_REJECT:
        cmdTxt.append("Do not send remaining fragments");
        break;
    default:
        cmdTxt.append("Unknown response");
        break;
    }
    return cmdTxt;
}

