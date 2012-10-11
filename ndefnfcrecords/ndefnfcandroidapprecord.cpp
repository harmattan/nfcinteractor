#include "ndefnfcandroidapprecord.h"

/*!
  Name of the Android package.
  */
QString NdefNfcAndroidAppRecord::packageName() const
{
    const QByteArray p = payload();
    if (p.isEmpty())
        return QString();
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    return codec->toUnicode(p);
}

/*!
  Sets the name of the Android package.
  */
void NdefNfcAndroidAppRecord::setPackageName(const QString &packageName)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QByteArray p = codec->fromUnicode(packageName);
    setPayload(p);
}


