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
#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include "nfctypes.h"

// Used for the QSettings file, for stats and IAP
#define SETTINGS_ORG "Nokia"
#define SETTINGS_APP "NfcInteractor"
#define SETTINGS_VERSION 3

#if defined(Q_OS_SYMBIAN)
    #define DEFAULT_NDEF_LOG_DIR "E:/nfc/"
#elif defined(MEEGO_EDITION_HARMATTAN)
    #define DEFAULT_NDEF_LOG_DIR "/home/user/MyDocs/nfc/"
#elif defined(QT_SIMULATOR)
    #define DEFAULT_NDEF_LOG_DIR "C:/nfc/"
#endif
#define COLLECTED_LOG_DIR "collected/"
#define SAVED_LOG_DIR "saved/"

/*! Default connection URN for the connection-oriented SNEP mode. */
#define LLCP_CONNECTIONORIENTED_SERVICENAME "urn:nfc:sn:snep"
/*! Default connection port for connection-less mode. */
#define LLCP_CONNECTIONLESS_PORT    4

class AppSettings : public QObject
{
    Q_OBJECT
public:
    explicit AppSettings(QObject *parent = 0);
    
signals:
    
public slots:
    void saveSettings();

    void setLogNdefToFile(const bool logNdefToFile);
    bool logNdefToFile() const;
    void setLogNdefDir(const QString& logNdefDir);
    QString logNdefDir() const;
    QString logNdefDir(const bool collected);

    void setDeleteTagBeforeWriting(const bool deleteTagBeforeWriting);
    bool deleteTagBeforeWriting() const;

    void setUseSnep(const bool useSnep);
    bool useSnep() const;

    // Peer to peer
    void setUseConnectionLess(const bool useConnectionLess);
    bool useConnectionLess() const;
    void setNfcUri(const QString& nfcUri);
    QString nfcUri() const;
    void setNfcPort(const int nfcPort);
    int nfcPort() const;
    void setSendThroughServerSocket(const bool sendThroughServerSocket);
    bool sendThroughServerSocket() const;
    void setConnectClientSocket(const bool connectClientSocket);
    bool connectClientSocket() const;
    void setConnectServerSocket(const bool connectServerSocket);
    bool connectServerSocket() const;

private:
    void loadSettings();

private:
    /*! Is logging of read tags to files enabled. */
    bool m_logNdefToFile;
    /*! If logging tags to files is enabled, in which directory so store them. */
    QString m_logNdefDir;


    /*! Write an empty message to the tag before writing the cached message.
      This increases the stability when writing to factory-empty tags, as
      writing an empty message formats the tag, and the successive NDEF
      message writing then usually works fine. */
    bool m_deleteTagBeforeWriting;

    /*! Use the SNEP (Simple Ndef Exchange Protocol) for peer-to-peer communication. */
    bool m_useSnep;

    // Peer to peer
    bool m_useConnectionLess;
    QString m_nfcUri;
    int m_nfcPort;
    bool m_sendThroughServerSocket;
    bool m_connectClientSocket;
    bool m_connectServerSocket;
};

#endif // APPSETTINGS_H
