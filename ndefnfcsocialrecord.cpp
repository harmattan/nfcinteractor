#include "ndefnfcsocialrecord.h"

NdefNfcSocialRecord::NdefNfcSocialRecord()
    : NdefNfcSmartUriRecord(),
    m_socialType(Twitter)
{
}

QString NdefNfcSocialRecord::socialUserName() const
{
    return m_socialUserName;
}

void NdefNfcSocialRecord::setSocialUserName(const QString &socialUserName)
{
    m_socialUserName = socialUserName;
    updatePayload();
}

NdefNfcSocialRecord::NfcSocialType NdefNfcSocialRecord::socialType() const
{
    return m_socialType;
}

void NdefNfcSocialRecord::setSocialType(const NdefNfcSocialRecord::NfcSocialType socialType)
{
    if (m_socialType != socialType) {
        m_socialType = socialType;
        updatePayload();
    }
}

void NdefNfcSocialRecord::updatePayload()
{
    QUrl uri;
    switch (m_socialType) {
    case Twitter:
        uri = "http://twitter.com/" + m_socialUserName;
        break;
    case LinkedIn:
        uri = "http://linkedin.com/in/" + m_socialUserName;
        break;
    case Facebook:
        uri = "http://facebook.com/" + m_socialUserName;
        break;
    case Xing:
        uri = "http://xing.com/profile/" + m_socialUserName;
        break;
    case VKontakte:
        uri = "http://vkontakte.ru/" + m_socialUserName;
        break;
    }

    NdefNfcSmartUriRecord::setUri(uri);
    qDebug() << "Social network tag url: " << uri;
}
