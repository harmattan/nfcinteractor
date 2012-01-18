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

/*!
  \brief Create an app store link to download the app, either direct
  or for multiple stores using the nfcinteractor.com web service.

  Tags to download an app are useful in many cases, either as
  the sole record on a tag, or in combination with a custom record
  in a single NDEF message. This would then allow your app to
  auto-start when already installed on the phone (through the
  custom record your app is registered for), or if the app isn't
  installed, the phone will ignore the record and instead parse
  this store record and send the user to download the app.

  IF you only add the app ID for a single app store, the class
  will write the direct store link to the tag. This causes some
  phones to directly open the store (e.g., on the Nokia N9).

  If you want your tag to work for multiple platforms, you can use
  the web service at nfcinteractor.com to pass multiple ids for
  different platforms. The web service will then detect the
  operating system of the phone, and redirect it to the correct
  app store link.

  The web service also supports specifying a name instead of passing
  different IDs, to make further maintenance easier (StoreCustomName).
  See the instructions at nfcinteractor.com how to host the service, in
  order to add your own names to the script.

  As this class is based on the Smart URI base class, the
  payload is formatted as a URI record initially. When first
  adding Smart Poster information (like a title), the payload
  instantly transforms into a Smart Poster.

  \version 1.0.0
  */
class NdefNfcStoreLinkRecord : public NdefNfcSmartUriRecord
{
public:
    NdefNfcStoreLinkRecord();

public:
    /*! Which app store to link to. */
    enum AppStore {
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
    /*! Add an app id for a specified app store. If you only add
      a single app id, the class will generate a direct store link.
      If you add more than one app store, it will by default use
      the nfcinteractor.com web service. */
    void addAppId(const AppStore appStore, const QString& appId);
    /*! Retrieve the app id for the specified app store, if it has
      already been set. */
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
