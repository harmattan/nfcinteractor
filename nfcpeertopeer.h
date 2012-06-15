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
#ifndef NFCPEERTOPEER_H
#define NFCPEERTOPEER_H

#include <QObject>
#include <QDeclarativeView>
#include <QTimer>
#include <qnearfieldmanager.h>
#include <qllcpserver.h>
#include <qllcpsocket.h>
#include "appsettings.h"
#include "snepmanager.h"

QTM_USE_NAMESPACE   // Use Qt Mobility namespace


class NfcPeerToPeer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY busyChanged)

public:
    explicit NfcPeerToPeer(QObject *parent = 0);
    ~NfcPeerToPeer();

    void setAppSettings(AppSettings* appSettings);
    void setNfcManager(QNearFieldManager* nfcManager);

    bool isBusy() const;
signals:
    void nfcStatusUpdate(const QString& nfcStatusText);
    void nfcStatusError(const QString& nfcStatusErrorText);
    void chatMessage(const QString& nfcClientMessage);
    void ndefMessage(const QNdefMessage& nfcNdefMessage);
    void statusMessage(const QString& statusMessage);
    void settingsApplied();
    void busyChanged();
    void nfcSnepSuccess();

public slots:
    void applySettings();
    void initAndStartNfc();
    void sendText(const QString& text);
    void sendNdefMessage(const QNdefMessage* message);
    void targetDetected(QNearFieldTarget *target);
    void targetLost(QNearFieldTarget *target);

private slots:
    void doApplySettings();

    void targetError(QNearFieldTarget::Error error, const QNearFieldTarget::RequestId &id);

    void handleNewConnection();
    void serverSocketError(QLlcpSocket::SocketError socketError);
    void serverSocketStateChanged ( QLlcpSocket::SocketState socketState );
    void serverSocketDisconnected ( );

    void readTextClient();
    void readTextServer();
    void clientSocketDisconnected();
    void clientSocketError(QLlcpSocket::SocketError socketError);
    void clientSocketStateChanged ( QLlcpSocket::SocketState socketState );

private:
    void resetAll();
    void copyNfcUriFromAppSettings();
    void initClientSocket();
    void readText(QLlcpSocket *socket, const bool isServerSocket);
    bool sendCachedText();
    QString convertTargetErrorToString(QNearFieldTarget::Error error);
    QString convertSocketStateToString(QLlcpSocket::SocketState socketState);
    QString convertSocketErrorToString(QLlcpSocket::SocketError socketError);


private:
    AppSettings* m_appSettings;
    char* m_nfcUri;
    int m_nfcPort;
    QNearFieldManager *m_nfcManager;
    SnepManager* m_snepManager;
    QNearFieldTarget *m_nfcTarget;
    QLlcpServer *m_nfcServer;
    QLlcpSocket *m_nfcClientSocket;
    QLlcpSocket *m_nfcServerSocket;
    QByteArray m_sendDataQueue;
    bool m_isResetting;
    // Use connection-less or connection-oriented LLCP.
    // In case of connection-less, will connect to: m_nfcPort
    // For connection-oriented, connects to: m_nfcUri
    // If set to use connection-less mode, the other three settings are not used
    // (sendThroughServerSocket, connectClientSocket, connectServerSocket), as the
    // connection only goes through a single socket anyway.
    // Note: the value of this one has to be set when constructing the class and
    // can't be changed afterwards! (as sockets need to be initialized differently).
    bool m_useConnectionLess;
    // Send text through the server socket or through the client socket.
    // Only used for connection-oriented mode.
    bool m_sendThroughServerSocket;
    // Connect the client socket once a suitable target is found.
    // Only used for connection-oriented mode.
    // Note: if sendThroughServerSocket is false, this setting has to be true.
    // Otherwise, the app would be unable to send data (can't send data through a
    // not-connected client socket).
    bool m_connectClientSocket;
    // Start a server to listen for incoming client connections.
    // Only used for connection-oriented mode.
    bool m_connectServerSocket;

    bool m_isBusy;
};

#endif // NFCPEERTOPEER_H
