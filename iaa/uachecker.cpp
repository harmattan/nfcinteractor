#include "uachecker.h"
#include <qplatformdefs.h>
#include <QtGlobal>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QWidget>

#ifdef Q_OS_SYMBIAN
#include <BrCtlInterface.h>
#endif

UAChecker::UAChecker(QObject *parent) :
    QObject(parent)
{

#ifdef Q_OS_SYMBIAN
    QWidget *widget = QApplication::allWidgets().first();
    if (widget) {
        m_serv = new QTcpServer(this);
        connect(m_serv, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
        m_serv->listen(QHostAddress::LocalHost);
        CCoeControl *c = widget->winId();
        QT_TRAP_THROWING(iBrCtlInterface = CreateBrowserControlL(c, TRect(0,0,0,0), NULL, TBrCtlDefs::ECommandIdBase));
        QString serverUrl = QString(QLatin1String("http://localhost:%1")).arg(m_serv->serverPort());

        QT_TRAP_THROWING(iBrCtlInterface->LoadUrlL(TPtrC16(static_cast<const TUint16*>(serverUrl.utf16()), serverUrl.length())));
    } else {
        qDebug() << "Can't check User-Agent. Using generic E7 User-Agent";
        m_userAgent = "Mozilla/5.0 (Symbian/3; Series60/5.2 NokiaE7-00/025.007; Profile/MIDP-2.1 Configuration/CLDC-1.1 ) AppleWebKit/533.4 (KHTML, like Gecko) NokiaBrowser/7.3.1.37 Mobile Safari/533.4 3gpp-gba";
    }

#elif defined(MEEGO_EDITION_HARMATTAN)
    m_userAgent = "Mozilla/5.0 (MeeGo; NokiaN9) AppleWebKit/534.13 (KHTML, like Gecko) NokiaBrowser/8.5.0 Mobile Safari/534.13";
#elif defined(Q_WS_MAEMO_5)
    m_userAgent = "Mozilla/5.0 (X11; U; Linux armv7l; en-GB;rv:1.9.2.3pre) Gecko/20100723 Firefox/3.5 Maemo Browser 1.7.4.8 RX-51 N900"
#else
    m_userAgent = "Mozilla/5.0 Safari";
#endif

}

#ifdef Q_OS_SYMBIAN
void UAChecker::handleNewConnection()
{
    QTcpSocket *sock = 0;
    while (sock = m_serv->nextPendingConnection()) {
        connect(sock, SIGNAL(readyRead()), this, SLOT(readData()));
    }
}

void UAChecker::readData()
{
    QTcpSocket *sock = qobject_cast<QTcpSocket*>(sender());
    if (!sock)
        return;

    while (sock->canReadLine()) {
        QByteArray data = sock->readLine();
        if (data.startsWith("User-Agent: ")) {
            data.replace("User-Agent: ", "");
            data.replace('\n', "");
            m_userAgent = data;
            m_serv->close();
            delete iBrCtlInterface;
            m_serv->deleteLater();
            iBrCtlInterface = 0;
            m_serv = 0;
            emit gotUserAgent();
            return;
        }
    }
    qDebug() << "Can't check User-Agent. Using generic E7 User-Agent";
    m_userAgent = "Mozilla/5.0 (Symbian/3; Series60/5.2 NokiaE7-00/025.007; Profile/MIDP-2.1 Configuration/CLDC-1.1 ) AppleWebKit/533.4 (KHTML, like Gecko) NokiaBrowser/7.3.1.37 Mobile Safari/533.4 3gpp-gba";
}
#endif
