#ifndef NDEFNFCSTORELINKRECORD_H
#define NDEFNFCSTORELINKRECORD_H

#include <QNdefMessage>
#include <QNdefRecord>
#include <QString>
#include <QUrl>
#include <QNdefNfcUriRecord>
#include "ndefnfcsprecord.h"
#include "ndefnfcsmarturirecord.h"

QTM_USE_NAMESPACE

class NdefNfcStoreLinkRecord : public NdefNfcSmartUriRecord
{
public:
    NdefNfcStoreLinkRecord();

public:
    enum AppStore {
        //Undefined = 0,  // Needed only for hash lookup
        StoreNokia = 0,
        StoreSymbian,
        StoreMeeGoHarmattan,
        StoreSeries40,
        StoreWindowsPhone,
        StoreAndroid,
        StoreiOS,
        StoreBlackberry,
        StoreCustomName
    };

public:
    void addAppId(const AppStore appStore, const QString& appId);
    QString appId(AppStore appStore) const;

    private:
    void updatePayload();
    QUrl generateStoreLink(const AppStore appStore, const QString& appId);
    QUrl generateMultiStoreLink();
    QString getWebCharForAppStore(const NdefNfcStoreLinkRecord::AppStore appStore);

private:
    QHash<AppStore,QString> m_appIds;
};

#endif // NDEFNFCSTORELINKRECORD_H
