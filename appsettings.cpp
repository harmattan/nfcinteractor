#include "appsettings.h"

AppSettings::AppSettings(QObject *parent) :
    QObject(parent),
    m_logNdefToFile(true),
    m_logNdefDir(DEFAULT_NDEF_LOG_DIR)
{
    loadSettings();
}


void AppSettings::setLogNdefToFile(const bool logNdefToFile)
{
    if (logNdefToFile != m_logNdefToFile) {
        m_logNdefToFile = logNdefToFile;
        saveSettings();
    }
}

bool AppSettings::logNdefToFile()
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

QString AppSettings::logNdefDir()
{
    return m_logNdefDir;
}

void AppSettings::saveSettings()
{
    qDebug() << "Saving settings... (C++)";
    QSettings settings(SETTINGS_ORG, SETTINGS_APP, this);
    settings.setValue("settingsversion", SETTINGS_VERSION);
    settings.setValue("logNdefToFile", m_logNdefToFile);
    settings.setValue("logNdefDir", m_logNdefDir);
}

void AppSettings::loadSettings()
{
    qDebug() << "Loading settings... (C++)";
    QSettings settings(SETTINGS_ORG, SETTINGS_APP, this);
    if (settings.value("settingsversion", -1) == 1) {
        qDebug() << "Found settings... (C++)";
        // Only load settings from version 1 with this code
        m_logNdefToFile = settings.value("logNdefToFile", true).toBool();
        m_logNdefDir = settings.value("logNdefDir", DEFAULT_NDEF_LOG_DIR).toString();
    }
}

