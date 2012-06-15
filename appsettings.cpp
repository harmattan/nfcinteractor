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

#include "appsettings.h"

AppSettings::AppSettings(QObject *parent) :
    QObject(parent),
    m_logNdefToFile(true),
    m_logNdefDir(DEFAULT_NDEF_LOG_DIR),
    m_deleteTagBeforeWriting(false),
    m_useSnep(true),
    m_useConnectionLess(false),
    m_nfcUri(LLCP_CONNECTIONORIENTED_SERVICENAME),
    m_nfcPort(LLCP_CONNECTIONLESS_PORT),
    m_sendThroughServerSocket(false),
    m_connectClientSocket(true),
    m_connectServerSocket(true)
{
    loadSettings();
}

void AppSettings::setUseConnectionLess(const bool useConnectionLess)
{
    if (useConnectionLess != m_useConnectionLess) {
        m_useConnectionLess = useConnectionLess;
        saveSettings();
    }
}

bool AppSettings::useConnectionLess() const
{
    return m_useConnectionLess;
}

void AppSettings::setNfcUri(const QString &nfcUri)
{
    if (nfcUri != m_nfcUri) {
        m_nfcUri = nfcUri;
        saveSettings();
    }
}

QString AppSettings::nfcUri() const
{
    return m_nfcUri;
}

void AppSettings::setNfcPort(const int nfcPort)
{
    if (nfcPort != m_nfcPort) {
        m_nfcPort = nfcPort;
        saveSettings();
    }
}

int AppSettings::nfcPort() const
{
    return m_nfcPort;
}

void AppSettings::setSendThroughServerSocket(const bool sendThroughServerSocket)
{
    if (sendThroughServerSocket != m_sendThroughServerSocket) {
        m_sendThroughServerSocket = sendThroughServerSocket;
        saveSettings();
    }
}

bool AppSettings::sendThroughServerSocket() const
{
    return m_sendThroughServerSocket;
}

void AppSettings::setConnectClientSocket(const bool connectClientSocket)
{
    if (connectClientSocket != m_connectClientSocket) {
        m_connectClientSocket = connectClientSocket;
        saveSettings();
    }
}

bool AppSettings::connectClientSocket() const
{
    return m_connectClientSocket;
}

void AppSettings::setConnectServerSocket(const bool connectServerSocket)
{
    if (connectServerSocket != m_connectServerSocket) {
        m_connectServerSocket = connectServerSocket;
        saveSettings();
    }
}

bool AppSettings::connectServerSocket() const
{
    return m_connectServerSocket;
}

void AppSettings::setLogNdefToFile(const bool logNdefToFile)
{
    if (logNdefToFile != m_logNdefToFile) {
        m_logNdefToFile = logNdefToFile;
        saveSettings();
    }
}

bool AppSettings::logNdefToFile() const
{
    return m_logNdefToFile;
}

void AppSettings::setLogNdefDir(const QString &logNdefDir)
{
    if (logNdefDir != m_logNdefDir) {
        m_logNdefDir = logNdefDir;
        saveSettings();
    }
}

QString AppSettings::logNdefDir() const
{
    return m_logNdefDir;
}

QString AppSettings::logNdefDir(const bool collected)
{
    return m_logNdefDir + (collected ? COLLECTED_LOG_DIR : SAVED_LOG_DIR);
}

void AppSettings::setDeleteTagBeforeWriting(const bool deleteTagBeforeWriting)
{
    if (deleteTagBeforeWriting != m_deleteTagBeforeWriting) {
        m_deleteTagBeforeWriting = deleteTagBeforeWriting;
        saveSettings();
    }
}

bool AppSettings::deleteTagBeforeWriting() const
{
    return m_deleteTagBeforeWriting;
}

void AppSettings::setUseSnep(const bool useSnep)
{
    if (useSnep != m_useSnep) {
        m_useSnep = useSnep;
        saveSettings();
    }
}

bool AppSettings::useSnep() const
{
    return m_useSnep;
}

void AppSettings::saveSettings()
{
    // TODO: add peer to peer settings
    QSettings settings(SETTINGS_ORG, SETTINGS_APP, this);
    settings.setValue("settingsversion", SETTINGS_VERSION);
    settings.setValue("logNdefToFile", m_logNdefToFile);
    settings.setValue("logNdefDir", m_logNdefDir);
    settings.setValue("deleteTags", m_deleteTagBeforeWriting);
    settings.setValue("useSnep", m_useSnep);
}

void AppSettings::loadSettings()
{
    QSettings settings(SETTINGS_ORG, SETTINGS_APP, this);
    if (settings.value("settingsversion", -1) == SETTINGS_VERSION) {
        // Only load settings from version SETTINGS_VERSION with this code
        m_logNdefToFile = settings.value("logNdefToFile", true).toBool();
        m_logNdefDir = settings.value("logNdefDir", DEFAULT_NDEF_LOG_DIR).toString();
        m_deleteTagBeforeWriting = settings.value("deleteTags", false).toBool();
        m_useSnep = settings.value("useSnep", true).toBool();
    }
}

