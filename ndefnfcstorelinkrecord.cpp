#include "ndefnfcstorelinkrecord.h"

NdefNfcStoreLinkRecord::NdefNfcStoreLinkRecord()
{
    updatePayload();
}

void NdefNfcStoreLinkRecord::addAppId(const NdefNfcStoreLinkRecord::AppStore appStore, const QString &appId)
{
    m_appIds.insert(appStore, appId);
    updatePayload();
}

QString NdefNfcStoreLinkRecord::appId(NdefNfcStoreLinkRecord::AppStore appStore) const
{
    return m_appIds.value(appStore);
}

void NdefNfcStoreLinkRecord::updatePayload()
{
    QUrl tagStoreUri;
    const int numIds = m_appIds.size();
    // One app store only,
    // or same app id for Nokia Store on different platforms
    // -> Generate direct link
    if (numIds == 1) {
        // Get the platform and ID of the specified
        // app and generate a direct link
        QHash<AppStore, QString>::const_iterator i = m_appIds.constBegin();
        while (i != m_appIds.constEnd()) {
            // Only one element in the hash map, but still
            // need to iterate over it to get it.
            tagStoreUri = generateStoreLink(i.key(), i.value());
            i++;
        }
    } else if (numIds > 1) {
        // Multiple app stores
        // -> Use nfcinteractor.com script
        tagStoreUri = generateMultiStoreLink();
    }

    // No app id set
    qDebug() << "Store URI before check: " << tagStoreUri;
    if (tagStoreUri.isEmpty()) {
        qDebug() << "isEmpty";
        tagStoreUri.setUrl("http://store.ovi.com");
    }

    // Set link to base class
    NdefNfcSmartUriRecord::setUri(tagStoreUri);
}

QUrl NdefNfcStoreLinkRecord::generateStoreLink(const NdefNfcStoreLinkRecord::AppStore appStore, const QString& appId)
{
    QUrl link;
    switch (appStore) {
    case StoreNokia:
    case StoreSymbian:
    case StoreMeeGoHarmattan:
    case StoreSeries40:
        link.setUrl("http://store.ovi.com/content/" + appId);
        break;
    case StoreWindowsPhone:
        link.setUrl("http://windowsphone.com/s?appId=" + appId);
        break;
    case StoreAndroid:
        link.setUrl("https://market.android.com/details?id=" + appId);
        break;
    case StoreiOS:
        link = "http://itunes.com/apps/" + appId;
        break;
    case StoreBlackberry:
        link = "http://appworld.blackberry.com/webstore/clientlaunch/" + appId;
        break;
    case StoreCustomName:
        link = "http://nfcinteractor.com/dl.php?c=" + appId;
        break;
    }
    return link;
}

QUrl NdefNfcStoreLinkRecord::generateMultiStoreLink()
{
    QUrl link("http://nfcinteractor.com/dl.php");
    QHash<AppStore, QString>::const_iterator i = m_appIds.constBegin();
    while (i != m_appIds.constEnd()) {
        //cout << i.key() << ": " << i.value() << endl;
        link.addQueryItem(getWebCharForAppStore(i.key()), i.value());
        ++i;
    }
    return link;
}

QString NdefNfcStoreLinkRecord::getWebCharForAppStore(const NdefNfcStoreLinkRecord::AppStore appStore) {
    switch (appStore) {
    case StoreNokia:
        return "n";
        break;
    case StoreSymbian:
        return "s";
        break;
    case StoreMeeGoHarmattan:
        return "h";
        break;
    case StoreSeries40:
        return "f";
        break;
    case StoreWindowsPhone:
        return "w";
        break;
    case StoreAndroid:
        return "a";
        break;
    case StoreiOS:
        return "i";
        break;
    case StoreBlackberry:
        return "b";
        break;
    case StoreCustomName:
        return "n";
        break;
    }
}
