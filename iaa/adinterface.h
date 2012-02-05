#ifndef INNERINTERFACE_H
#define INNERINTERFACE_H

#include <QObject>
#include <QQueue>

class RequestQueue;
class AdInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool networkAccessible READ networkAccessible NOTIFY networkAccessibilityChanged)
    friend class RequestQueue;

public:
    explicit AdInterface(QObject *parent = 0);

    bool networkAccessible() const;

signals:
    void networkAccessibilityChanged(bool accessibility);
    void networkNotAccessible();

public slots:
    void requestAd(const QVariant &adItem);

private:
    void checkUserAgent();

    mutable QByteArray m_userAgent;

    RequestQueue* m_requestQueue;
};

#endif // INNERINTERFACE_H
