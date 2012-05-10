#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include "nfctypes.h"

// Used for the QSettings file, for stats and IAP
#define SETTINGS_ORG "Nokia"
#define SETTINGS_APP "NfcInteractor"
#define SETTINGS_VERSION 1

#if defined(Q_OS_SYMBIAN)
    #define DEFAULT_NDEF_LOG_DIR "E:/nfc/"
#elif defined(MEEGO_EDITION_HARMATTAN)
    #define DEFAULT_NDEF_LOG_DIR "/home/user/MyDocs/nfc/"
#elif defined(QT_SIMULATOR)
    #define DEFAULT_NDEF_LOG_DIR "C:/nfc/"
#endif
#define COLLECTED_LOG_DIR "collected/"
#define SAVED_LOG_DIR "saved/"

class AppSettings : public QObject
{
    Q_OBJECT
public:
    explicit AppSettings(QObject *parent = 0);
    
signals:
    
public slots:
    void setLogNdefToFile(const bool logNdefToFile);
    bool logNdefToFile();
    void setLogNdefDir(const QString& logNdefDir);
    QString logNdefDir();
    QString logNdefDir(const bool collected);
    void saveSettings();

private:
    void loadSettings();

private:
    /*! Is logging of read tags to files enabled. */
    bool m_logNdefToFile;
    /*! If logging tags to files is enabled, in which directory so store them. */
    QString m_logNdefDir;
};

#endif // APPSETTINGS_H
