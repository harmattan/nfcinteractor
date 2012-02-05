#include "requestqueue.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <qplatformdefs.h>
#include "adinterface.h"
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
namespace {
const QLatin1String baseUrl("http://m2m1.inner-active.com/simpleM2M/clientRequestAd");
}
RequestQueue::RequestQueue(AdInterface *parent) :
    QObject(parent)
  , m_nam(new QNetworkAccessManager(this))
  , m_requestRunning(false)
  , m_confman(new QNetworkConfigurationManager(this))
  , m_nsession(0)
  , m_onlineCheck(false)
{
    connect(m_confman, SIGNAL(onlineStateChanged(bool)), parent, SIGNAL(networkAccessibilityChanged(bool)));
    connect(m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(adRequestFinished(QNetworkReply*)));
    m_nsession = new QNetworkSession(m_confman->defaultConfiguration(), this);
}

RequestQueue::~RequestQueue()
{
}

bool RequestQueue::isOnline() const
{
    return m_confman->isOnline();
}

void RequestQueue::addToQueue(QObject *object)
{
    // no need to add same object multipletimes
    if (!m_adItemQueue.contains(object)) {
        m_adItemQueue.enqueue(object);
        QTimer::singleShot(0, this, SLOT(handleRequests()));
    }
}

void RequestQueue::handleRequests()
{
    if (m_adItemQueue.isEmpty() || m_requestRunning)
        return;
    m_requestRunning = true;

    QObject *adItem = m_adItemQueue.dequeue();
    QMetaObject::invokeMethod(adItem, "__createQuery", Qt::DirectConnection);
    QUrl requestUrl = adItem->property("__query").toUrl();
    if (!requestUrl.isValid()) {
        QMetaObject::invokeMethod(adItem, "adError",
                                  Q_ARG(QString, QLatin1String("Not valid query url!")));
        m_requestRunning = false;
        return;
    }
#if defined(Q_OS_SYMBIAN) || defined(MEEGO_EDITION_HARMATTAN) || defined(Q_WS_MAEMO_5)
    // online checking works only on mobile
    if (!m_onlineCheck && !m_confman->isOnline()) {
        m_onlineCheck = true;
        m_nsession->open();
        if (!m_nsession->waitForOpened()) {
            AdInterface *adI = qobject_cast<AdInterface*>(parent());
            emit adI->networkNotAccessible();
        }
    }

    if (!m_confman->isOnline()) {
        QMetaObject::invokeMethod(adItem, "adError",
                                  Q_ARG(QString, QLatin1String("Network not accessible!")));
        m_requestRunning = false;
        return;
    }
#endif

    // Add timestamp
    requestUrl.addQueryItem(QLatin1String("t"), QString::number(QDateTime::currentDateTime().toTime_t()));

    // Set User-Agent header
    QNetworkRequest req(requestUrl);
    req.setRawHeader("User-Agent", m_userAgent);

    qDebug() << "AdRequest:" << req.url() << "\nUA:" << m_userAgent;
    QNetworkReply *rep = m_nam->get(req);

    rep->setProperty("AdItem", QVariant::fromValue(adItem));
}

void RequestQueue::adRequestFinished(QNetworkReply *req)
{
    if (!req)
        return;
    QObject *adItem = req->property("AdItem").value<QObject*>();
    if (!adItem) {
        req->deleteLater();
        return;
    }

    QByteArray data = req->readAll();

    m_requestRunning = false;
    if (req->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 200) {
        QMetaObject::invokeMethod(adItem, "adError",
                                  Q_ARG(QString,req->errorString()));
        // When no connectivity -> UnknownNetworkError
        if (req->error() == QNetworkReply::UnknownNetworkError) {
            AdInterface *adI = qobject_cast<AdInterface*>(parent());
            emit adI->networkNotAccessible();
        }
        req->deleteLater();
        QTimer::singleShot(0, this, SLOT(handleRequests()));
        return;
    }
    req->deleteLater();

    // Update client ID
    int idStart = data.indexOf("Client Id=\"")+11;
    int idSize = data.indexOf("\"/>",data.indexOf("Id=\""))-idStart;
    QMetaObject::invokeMethod(adItem, "__idUpdated", Qt::DirectConnection,
                              Q_ARG(QVariant, data.mid(idStart,idSize)));

    adItem->setProperty("__xml", QString::fromUtf8(data));
    QTimer::singleShot(0, this, SLOT(handleRequests()));
}

