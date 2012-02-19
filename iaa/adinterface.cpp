#include "adinterface.h"
#include "uachecker.h"
#include "requestqueue.h"
#include <qplatformdefs.h>
#include <QEventLoop>
#include <QVariant>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QUrl>
#include <QDesktopServices>

#ifdef Q_OS_SYMBIAN
#include <APGTASK.H>
#include <APGCLI.H>
#include <EIKENV.H>
#endif

/*!
    \qmlclass AdInterface
    \ingroup com.inneractive
    \inherits QtObject
    The adInterface object is part of the \l {inneractive QML Components} module.

    AdInterface is exposed as a context property called "adInterface". It is not possible to
    instantiate a AdInterface object in QML.
*/

/*!
    \qmlproperty bool AdInterface::networkAccessible
    This property contains information of network accessibility.
*/

/*!
    \qmlsignal AdInterface::networkNotAccessible()
    This signal is emited after network connection fails or user doesn't allow
    connection to internet.
    For example:
    \code
        Connections {
            target: adInterface
            onNetworkNotAccessible: {
                //... show dialog to user about networkaccessibility and quit application...
            }
        }
    \endcode
*/
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

// Opens url in native browser in Harmattan and Symbian
void AdInterface::openAd(const QUrl &adUrl)
{
#if defined(Q_OS_SYMBIAN)

    QString adUrlString(adUrl.toString());
    TUid browserUid = TUid::Uid(0x10008D39);
    adUrlString.prepend("4 ");
    TUid id( browserUid ); // S3
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( id );
    if ( task.Exists() ) {
      task.SendMessage( TUid::Uid( 0 ), TPtrC8((TUint8*)adUrlString.toUtf8().data()));
    } else {
      RApaLsSession appArcSession;
      TThreadId id;
      User::LeaveIfError( appArcSession.Connect() );
      appArcSession.StartDocument( TPtrC16(static_cast<const TUint16*>(adUrlString.utf16()), adUrlString.length()), browserUid, id);
      appArcSession.Close();
    }
#elif defined(MEEGO_EDITION_HARMATTAN)
    QProcess::startDetached(QString("/usr/bin/invoker --type=m /usr/bin/grob %1").arg(adUrl.toString()));
#else
    QDesktopServices::openUrl(adUrl);
#endif
}

// Checks User-Agent using UAChecker
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
