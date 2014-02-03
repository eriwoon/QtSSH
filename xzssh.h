#ifndef XZSSH_H
#define XZSSH_H

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <QTcpSocket>
#include <QString>
#include <QHostAddress>

class XZSSh : public QObject
{
    Q_OBJECT
public:
    explicit XZSSh(QObject *parent = 0);
    ~XZSSh();
    
public:
    int xzssh_connect(QString peerAddr, quint16 peerPort, QString username, QString password);
    int xzssh_disconnect();
    int xzssh_exec(QString, QString*);
    QString xzssh_getErrMsg();
    int xzssh_getErrNo();
    int xzssh_sftpGet(QString RemoteAddr, QString LocalAddr);

private:
    static int waitsocket(int, LIBSSH2_SESSION *);
    LIBSSH2_SESSION* m_session;
    QTcpSocket* m_socket;

    LIBSSH2_SFTP *m_sftp_session;
    LIBSSH2_SFTP_HANDLE *m_sftp_handle;

    int m_iErrNo;
    QString m_sErrMsg;
signals:
    
public slots:
    
};

#endif // XZSSH_H
