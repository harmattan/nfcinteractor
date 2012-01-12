#ifndef NDEFNFCSOCIALRECORD_H
#define NDEFNFCSOCIALRECORD_H

#include <QNdefMessage>
#include <QNdefRecord>
#include <QString>
#include <QUrl>
#include <QNdefNfcUriRecord>
#include "ndefnfcsprecord.h"
#include "ndefnfcsmarturirecord.h"

QTM_USE_NAMESPACE

class NdefNfcSocialRecord : public NdefNfcSmartUriRecord
{
public:
    NdefNfcSocialRecord();

public:
    enum NfcSocialType {
        Twitter = 0,
        LinkedIn,
        Facebook,
        Xing,
        VKontakte
    };

public:
    QString socialUserName() const;
    void setSocialUserName(const QString& socialUserName);

    NfcSocialType socialType() const;
    void setSocialType(const NfcSocialType socialType);

private:
    void updatePayload();

private:
    QString m_socialUserName;
    NfcSocialType m_socialType;
};

#endif // NDEFNFCSOCIALRECORD_H
