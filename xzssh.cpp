#include "xzssh.h"
#include "xzconfig.h"
#include <QTimer>

int XZSSh::waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);


    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;

    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;

    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

    return rc;
}

XZSSh::XZSSh(QObject *)
{
    //XZOUTPUT;
    this->m_socket = new QTcpSocket();
}

int XZSSh::xzssh_connect(QString peerAddr, quint16 peerPort, QString username, QString password)
{
    // for exit use
    char* errmsg = NULL;
    int errmsg_len = 0;

    // 判断是否是重复connect, 否则先disconnect之
    if (this->m_socket->state() != QAbstractSocket::UnconnectedState)
    {
        if(this->m_socket->peerAddress().toString() == peerAddr && this->m_socket->peerPort() == peerPort)
        {
            return 0;
        }
        else
        {
            this->m_socket->disconnect();
        }
    }

    int libssh2_error = libssh2_init(0);
    if(libssh2_error)
    {
        this->m_iErrNo = -1;
        this->m_sErrMsg = "libssh2_init failed.";
        this->m_socket->disconnect();
        return -2;
    }

    m_socket->connectToHost(QHostAddress(peerAddr), peerPort);
    if(!m_socket->waitForConnected(5000))
    {
        this->m_iErrNo = -1;
        this->m_sErrMsg = m_socket->errorString();
        this->m_socket->disconnect();
        return -1;
    }

    m_session = libssh2_session_init();
    if(!m_session)
    {
        libssh2_session_last_error(m_session, &errmsg, &errmsg_len, 0);
        if (errmsg_len>0 && errmsg)
        {
            this->m_sErrMsg = errmsg;
            errmsg = 0;
        }

        this->m_socket->disconnect();
        return -2;
    }

    while ((libssh2_error = libssh2_session_handshake(m_session, m_socket->socketDescriptor())) == LIBSSH2_ERROR_EAGAIN);
    if (libssh2_error) {
        libssh2_session_last_error(m_session, &errmsg, &errmsg_len, 0);
        if (errmsg_len>0 && errmsg)
        {
            this->m_sErrMsg = errmsg;
            errmsg = 0;
        }

        this->m_socket->disconnect();
        return -3;
    }

    libssh2_userauth_list(m_session, username.toLocal8Bit(), username.length());
    if(libssh2_userauth_password(
        m_session,
        username.toLocal8Bit(),
        password.toLocal8Bit()))
    {
        libssh2_session_last_error(m_session, &errmsg, &errmsg_len, 0);
        if (errmsg_len>0 && errmsg)
        {
            this->m_sErrMsg = errmsg;
            errmsg = 0;
        }

        this->m_socket->disconnect();
        return -4;
    }

    return 0;
}

int XZSSh::xzssh_disconnect()
{
    this->m_socket->disconnect();
    return 0;
}

QString XZSSh::xzssh_getErrMsg()
{
    return this->m_sErrMsg;
}

int XZSSh::xzssh_getErrNo()
{
    return this->m_iErrNo;
}

int XZSSh::xzssh_exec(QString commandline, QString &result)
{
    LIBSSH2_CHANNEL *channel;
    int rc;
    int exitcode;
    int bytecount = 0;


    while( (channel = libssh2_channel_open_session(m_session)) == NULL &&

           libssh2_session_last_error(m_session,NULL,NULL,0) ==

           LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(m_socket->socketDescriptor(), m_session);
    }
    if( channel == NULL )
    {
        fprintf(stderr,"Error\n");
        exit( 1 );
    }
    while( (rc = libssh2_channel_exec(channel, commandline.toLocal8Bit().data())) ==

           LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(m_socket->socketDescriptor(), m_session);
    }
    if( rc != 0 )
    {
        fprintf(stderr,"Error\n");
        exit( 1 );
    }
    for( ;; )
    {
        /* loop until we block */
        int rc;
        do
        {
            char buffer[0x4000];
            rc = libssh2_channel_read( channel, buffer, sizeof(buffer) );

            if( rc > 0 )
            {
                int i;
                bytecount += rc;
                fprintf(stderr, "We read:\n");
                for( i=0; i < rc; ++i )
                    fputc( buffer[i], stderr);
                fprintf(stderr, "\n");
            }
            else {
                if( rc != LIBSSH2_ERROR_EAGAIN )
                    /* no need to output this for the EAGAIN case */
                    fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
            }
        }
        while( rc > 0 );

        /* this is due to blocking that would occur otherwise so we loop on
           this condition */
        if( rc == LIBSSH2_ERROR_EAGAIN )
        {
            waitsocket(m_socket->socketDescriptor(), m_session);
        }
        else
            break;
    }
    exitcode = 127;

    return 0;
}
