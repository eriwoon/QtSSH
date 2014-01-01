#ifndef XZSSH_H
#define XZSSH_H

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <QTcpSocket>
#include <QString>
#include <QHostAddress>

class XZSSh : QObject
{
    Q_OBJECT
public:
    explicit XZSSh(QObject *parent = 0);
    
public:
    int xzssh_connect(QString peerAddr, quint16 peerPort, QString username, QString password);
    int xzssh_disconnect();
    int xzssh_exec(QString, QString&);
    QString xzssh_getErrMsg();
    int xzssh_getErrNo();

private:
    static int waitsocket(int, LIBSSH2_SESSION *);
    LIBSSH2_SESSION* m_session;
    QTcpSocket* m_socket;

    int m_iErrNo;
    QString m_sErrMsg;
signals:
    
public slots:
    
};

#endif // XZSSH_H
