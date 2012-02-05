#ifndef UACHECKER_H
#define UACHECKER_H

#include <QObject>

#ifdef Q_OS_SYMBIAN
class CBrCtlInterface;
class QTcpServer;
#endif

class UAChecker : public QObject
{
    Q_OBJECT
public:
    explicit UAChecker(QObject *parent = 0);

    QByteArray userAgent() const { return m_userAgent; }

#ifdef Q_OS_SYMBIAN
private slots:
    void handleNewConnection();
    void readData();
#endif

signals:
    void gotUserAgent();


private:
    QByteArray m_userAgent;
#ifdef Q_OS_SYMBIAN
    QTcpServer *m_serv;
    CBrCtlInterface *iBrCtlInterface;
#endif
};

#endif // UACHECKER_H
