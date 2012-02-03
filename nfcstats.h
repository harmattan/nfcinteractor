#ifndef NFCSTATS_H
#define NFCSTATS_H

#include <QObject>
#include <QHash>
#include <QSettings>
#include <QDebug>
#include "nfctypes.h"

class NfcStats : public QObject
{
    Q_OBJECT
public:
    explicit NfcStats(QObject *parent = 0);
    
public:
    int tagReadCount();
    void incTagReadCount();

    QHash<NfcTypes::MessageType,int> msgWrittenCountAll();
    int msgWrittenCount(NfcTypes::MessageType msgType);
    Q_INVOKABLE int advMsgWrittenCount();
    bool isAdvMsgType(NfcTypes::MessageType msgType);

    //void incComposedMsgCountInt(int msgType);
    void resetComposedMsgCount();
    void incComposedMsgCount(NfcTypes::MessageType msgType);
    void commitComposedToWrittenCount();

private:

    void incWrittenMsgCount(NfcTypes::MessageType msgType, const int numMsgPresent);

    void loadStats();
    void loadCount(QSettings *settings, const NfcTypes::MessageType msgType);
    void saveStats();
    void saveCount(QSettings* settings, const NfcTypes::MessageType msgType);

private:
    int m_tagReadCount;
    QHash<NfcTypes::MessageType,int> m_composedMsgCount;
    QHash<NfcTypes::MessageType,int> m_writtenMsgCount;
};

#endif // NFCSTATS_H
