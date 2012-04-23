#include "ndefnfcsocialrecord.h"

/*!
  \brief Create an empty Social record.

  Default social network is Twitter.
  */
NdefNfcSocialRecord::NdefNfcSocialRecord()
    : NdefNfcSmartUriRecord(),
    m_socialType(Twitter)
{
}

/*!
  \brief Get the user name / id of the social network.
  */
QString NdefNfcSocialRecord::socialUserName() const
{
    return m_socialUserName;
}

/*!
  \brief Set the user name / id for the social network.
  */
void NdefNfcSocialRecord::setSocialUserName(const QString &socialUserName)
{
    m_socialUserName = socialUserName;
    updatePayload();
}

/*!
  \brief Get the social network this class will create a link to.
  */
NdefNfcSocialRecord::NfcSocialType NdefNfcSocialRecord::socialType() const
{
    return m_socialType;
}

/*!
  \brief Set the social network this class will create a link to.
  */
void NdefNfcSocialRecord::setSocialType(const NdefNfcSocialRecord::NfcSocialType socialType)
{
    if (m_socialType != socialType) {
        m_socialType = socialType;
        updatePayload();
    }
}

/*!
  \brief Format the payload and set it through the Smart URI base class.
  */
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
    case Foursquare:
        uri = "http://m.foursquare.com/v/" + m_socialUserName;
        break;
    }

    NdefNfcSmartUriRecord::setUri(uri);
    qDebug() << "Social network tag url: " << uri;
}
