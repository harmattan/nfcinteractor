#include "adinterface.h"
#include "uachecker.h"
#include "requestqueue.h"
#include <QEventLoop>
#include <QVariant>
#include <QTimer>
#include <QDebug>

AdInterface::AdInterface(QObject *parent) :
    QObject(parent)
  , m_requestQueue(new RequestQueue(this))
{
    checkUserAgent();
}

bool AdInterface::networkAccessible() const
{
    return m_requestQueue->isOnline();
}

void AdInterface::requestAd(const QVariant &adItem)
{
    QObject *object = adItem.value<QObject*>();
    m_requestQueue->addToQueue(object);
}

void AdInterface::checkUserAgent()
{
    if (m_userAgent.isEmpty()) {
        UAChecker uachecker;
        if (uachecker.userAgent().isEmpty()) {
            QEventLoop loop;
            QTimer::singleShot(5000, &loop, SLOT(quit())); // timeout timer
            connect(&uachecker, SIGNAL(gotUserAgent()), &loop, SLOT(quit()));
            loop.exec();
        }
        m_userAgent = uachecker.userAgent();
    }
    m_requestQueue->setUserAgent(m_userAgent);
}
