/*
 Objective C++ using libssh2 (http://www.libssh2.org/).
 LIBRARY NEEDED
        ssl crypto ssh2
 ABOUT LIBSSH2
        http://www.libssh2.org/docs.html
        http://www.libssh2.org/examples/
 Date
    2010-07-30
 Author
    xiaohan
 Source
    http://blog.csdn.net/xiaoqi_whu/article/details/5865166
*/
#include "ssh2.h"
#include <iostream>
/* set a fd into nonblocking mod#include <grp.h>e. */
void ssh2_t::set_nonblocking(int fd)
{
    int val;
    if ((val = fcntl(fd, F_GETFL)) == -1)
        return;
    if (!(val & O_NONBLOCK)) {
        val |= O_NONBLOCK;
        fcntl(fd, F_SETFL, val);
    }
}
int ssh2_t::waitsocket(int timeout/*=-1*/)
{
    int rc;
    struct pollfd pfd;
    bzero(&pfd, sizeof(pfd));
    pfd.events = 0; //POLLHUP | POLLERR;
    pfd.fd = m_sock;
    int dir = 0;
    // now make sure we wait in the correct direction
    dir = libssh2_session_block_directions(m_session);
    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        pfd.events |= POLLIN;
    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        pfd.events |= POLLOUT;
    rc = poll(&pfd, 1, timeout*1000);
    return rc;
}
long ssh2_t::tvdiff(struct timeval newer, struct timeval older)
{
    return (newer.tv_sec-older.tv_sec)*1000 + (newer.tv_usec-older.tv_usec)/1000;
}
void ssh2_t::disconnect()
{
    if (m_session)
    {
        libssh2_session_disconnect(m_session, "Normal Shutdown");
        libssh2_session_free(m_session);
        m_session = NULL;
    }
    if (m_sock)
    {
        ::close(m_sock);
        m_sock = 0;
    }
    m_connected = 0;
}
int ssh2_t::connect(const std::string& ip, unsigned short port, const std::string& username, const std::string& password)
{
    clear_error();
    if (m_connected)
    {
        // 判断是否是重复connect, 否则先disconnect之
        char ipaddr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &m_sin.sin_addr, ipaddr, INET_ADDRSTRLEN);
        if (ip==ipaddr && port==ntohs(m_sin.sin_port))
        {
            return 0;
        }
        else
        {
            disconnect();
        }
    }
    int rc = 0; // 返回结果的临时变量
    /* 连接ssh端口 */
    inet_pton(AF_INET, ip.c_str(), &m_sin.sin_addr);
    m_sin.sin_family = AF_INET;
    m_sin.sin_port = htons(port);
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    while ((m_connected=::connect(m_sock, (struct sockaddr*)(&m_sin), sizeof(struct sockaddr_in)))==-1 && EINTR==errno);
    if (m_sock<=0 || m_connected)
    {
        get_sys_error();
        disconnect();
        return -1;
    }
    // 建立session
    m_session = libssh2_session_init();
    if (NULL==m_session)
    {
        get_lib_error();
        disconnect();
        return -1;
    }
    libssh2_session_set_blocking(m_session, 0); // 非阻塞socket
    while ((rc=libssh2_session_startup(m_session, m_sock))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    if (rc)
    {
        get_lib_error();
        disconnect();
        return -1;
    }
    // 检查验证方法 : 仅使用password方式
    char *userauthlist = NULL;
    while ( (userauthlist=libssh2_userauth_list(m_session, username.c_str(), username.size()))==NULL
        && libssh2_session_last_error(m_session,NULL,NULL,0)==LIBSSH2_ERROR_EAGAIN
        &&  waitsocket()>=0);
    if (NULL==userauthlist || strstr(userauthlist, "password") == NULL)
    {
        get_lib_error();
        disconnect();
        return -1;
    }
    // auth
    while ((rc=libssh2_userauth_password(m_session, username.c_str(), password.c_str()))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    if (rc)
    {
        get_lib_error();
        disconnect();
        return -1;
    }
    m_connected = 1;
    return 0;
}
int ssh2_t::exec(const std::string& cmdline,
        char* stdout_buf/*=NULL*/, int stdout_buflen/*=0*/, int* stdout_recvlen/*=NULL*/,
        char* stderr_buf/*=NULL*/, int stderr_buflen/*=0*/, int* stderr_recvlen/*=NULL*/,
        int   stdout_fd/*=-1*/, int stderr_fd/*=-1*/)
{
    clear_error();
    if (0==m_connected)
    {
        /// FIX ME : do connect
        snprintf(m_errmsg, m_errmsg_buflen, "%s", "None connection yet");
        return -1;
    }
    int rc = 0;
    int ret = 0;
    LIBSSH2_CHANNEL*  channel = NULL;
    while( (channel= libssh2_channel_open_session(m_session))==NULL
        && libssh2_session_last_error(m_session,NULL,NULL,0)==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0 );
    if( channel == NULL )
    {
        get_sys_error();
        return -1;
    }
    libssh2_channel_set_blocking(channel, 1);
    while ((rc=libssh2_channel_exec(channel, cmdline.c_str()))==LIBSSH2_ERROR_EAGAIN && waitsocket()>=0);
    if (rc)
    {
        ret = -1;
        get_lib_error();
        goto clearup;
    }
    // 获取标准输出
    if (stdout_recvlen) *stdout_recvlen = 0;
    if (stdout_buf) *stdout_buf = 0;
    if (0==rc && (stdout_fd>=0 || (stdout_buf&&stdout_buflen>0)))
    {
        while (1)
        {
            int nread = libssh2_channel_read(channel, m_buf, m_buflen);
            if (LIBSSH2_ERROR_EAGAIN==nread)
            {
                waitsocket();
                continue;
            }
            if (nread > 0)
            {
                if (stdout_fd>=0)
                {
                    write(stdout_fd, m_buf, nread); // FIX ME : 未检查错误
                }
                int len = std::min(nread, stdout_buflen-1); // 尽量多的写入buf, 但至少留一个字节置为NUL
                if (stdout_buf && len>0)
                {
                    memcpy(stdout_buf, m_buf, len);
                    stdout_buf += len;
                    stdout_buflen -= len;
                    *stdout_buf = 0; // 以NUL结尾
                    if (stdout_recvlen) *stdout_recvlen += len;
                }
            }
            else
            {
                get_lib_error();
                break;
            }
        } // end while (1)
    }
    // 获取标准错误
    if (stderr_recvlen) *stderr_recvlen = 0;
    if (stderr_buf) *stderr_buf = 0;
    if (0==rc && (stderr_fd>=0 || (stderr_buf&&stderr_buflen>0)))
    {
        while (1)
        {
            int nread = libssh2_channel_read(channel, m_buf, m_buflen);
            if (LIBSSH2_ERROR_EAGAIN==nread)
            {
                waitsocket();
                continue;
            }
            if (nread > 0)
            {
                if (stderr_fd>=0)
                {
                    write(stderr_fd, m_buf, nread); // FIX ME : 未检查错误
                }
                int len = std::min(nread, stderr_buflen-1); // 尽量多的写入buf, 但至少留一个字节置为NUL
                if (stderr_buf && len>0)
                {
                    memcpy(stderr_buf, m_buf, len);
                    stderr_buf += len;
                    stderr_buflen -= len;
                    *stderr_buf = 0; // 以NUL结尾
                    if (stderr_recvlen) *stderr_recvlen += len;
                }
            }
            else
            {
                get_lib_error();
                break;
            }
        } // end while (1)
    }
clearup:
    while( channel && (rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN && waitsocket()>=0);
    if(channel && rc== 0)
    {
        ret = libssh2_channel_get_exit_status( channel );
        libssh2_channel_free(channel);
        channel = NULL;
    }
    return ret;
}
////
LIBSSH2_CHANNEL* ssh2_t::channel_open()
{
    clear_error();
    if (0==m_connected)
    {
        /// FIX ME : do connect
        snprintf(m_errmsg, m_errmsg_buflen, "%s", "None connection yet");
        return NULL;
    }
    LIBSSH2_CHANNEL*  channel = NULL;
    while( (channel= libssh2_channel_open_session(m_session))==NULL
        && libssh2_session_last_error(m_session,NULL,NULL,0)==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0 );
    if( channel == NULL )
    {
        get_sys_error();
    }
    else
    {
        libssh2_channel_set_blocking(channel, 1);
    }
    return channel;
}
int ssh2_t::channel_close(LIBSSH2_CHANNEL* channel)
{
    int rc = 0, ret=0;
    while(channel && (rc=libssh2_channel_close(channel))==LIBSSH2_ERROR_EAGAIN && waitsocket()>=0);
    if(channel && rc== 0)
    {
        ret = libssh2_channel_get_exit_status( channel );
        libssh2_channel_free(channel);
    }
    return  ret;
}
int ssh2_t::channel_exec(LIBSSH2_CHANNEL* channel, const std::string& cmdline)
{
    int rc = 0;
    while ((rc=libssh2_channel_exec(channel, cmdline.c_str()))==LIBSSH2_ERROR_EAGAIN && waitsocket()>=0);
    if (rc)
    {
        get_lib_error();
        return -1;
    }
    return libssh2_channel_get_exit_status(channel);
}
int ssh2_t::channel_write(LIBSSH2_CHANNEL* channel, const char* buf, size_t len)
{
    int ret = 0, rn=0;
    while (len)
    {
        while ((rn=libssh2_channel_write(channel, buf, len))==LIBSSH2_ERROR_EAGAIN && waitsocket()>=0);
        if (rn>0)
        {
            ret += rn;
            buf += rn;
            len -= rn;
        }
        else
        {
            break;
        }
    }
    get_lib_error();
    return ret;
}
int ssh2_t::channel_read(LIBSSH2_CHANNEL* channel, char* buf, size_t len)
{
    int ret = 0, rn=0;
    while (len)
    {
        while ((rn=libssh2_channel_read(channel, buf, len))==LIBSSH2_ERROR_EAGAIN && waitsocket()>=0);
        if (rn>0)
        {
            ret += rn;
            buf += rn;
            len -= rn;
        }
        else
        {
            break;
        }
    }
    get_lib_error();
    return ret;
}
LIBSSH2_SFTP* ssh2_t::sftp_init()
{
    if (NULL==m_sftp)
    {
        while ((m_sftp=libssh2_sftp_init(m_session))==NULL
            && libssh2_session_last_error(m_session, NULL, NULL, 0)==LIBSSH2_ERROR_EAGAIN
            && waitsocket()>=0);
    }
    get_lib_error();
    return m_sftp;
}
int ssh2_t::sftp_shutdown()
{
    if (NULL==m_sftp)
    {
        return 0;
    }
    int rc = 0;
    while ((rc=libssh2_sftp_shutdown(m_sftp))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
int ssh2_t::sftp_close_handle(LIBSSH2_SFTP_HANDLE* sftp_handle)
{
    if (NULL==sftp_handle)
    {
        return 0;
    }
    int rc = 0;
    while ((rc=libssh2_sftp_close(sftp_handle))==LIBSSH2_ERROR_EAGAIN);
    return rc;
}
LIBSSH2_SFTP_HANDLE* ssh2_t::sftp_open(const std::string& path, long flags, long mode)
{
    if (NULL==m_sftp)
    {
        if (NULL==sftp_init()) return NULL;
        return NULL;
    }
    LIBSSH2_SFTP_HANDLE* sftp_handle = NULL;
    while ((sftp_handle=libssh2_sftp_open(m_sftp, path.c_str(), flags, mode))==NULL
        && libssh2_session_last_errno(m_session)==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    get_lib_error();
    return sftp_handle;
}
int ssh2_t::sftp_close(LIBSSH2_SFTP_HANDLE* sftp_handle)
{
    return sftp_close_handle(sftp_handle);
}
int ssh2_t::sftp_read(LIBSSH2_SFTP_HANDLE* sftp_handle, char* buf, size_t len)
{
    int ret = 0;
    while (len)
    {
        int rn = 0;
        while ((rn=libssh2_sftp_read(sftp_handle, buf, len))==LIBSSH2_ERROR_EAGAIN
            && waitsocket()>=0);
        if (rn<=0)
        {
            break;
        }
        buf += rn;
        len -= rn;
        ret += rn;
    }
    get_lib_error();
    return ret;
}
int ssh2_t::sftp_write(LIBSSH2_SFTP_HANDLE* sftp_handle, const char* buf, size_t len)
{
    int ret = 0;
    while (len)
    {
        int rn = 0;
        while ((rn=libssh2_sftp_write(sftp_handle, buf, len))==LIBSSH2_ERROR_EAGAIN
            && waitsocket()>=0);
        if (rn<=0)
        {
            break;
        }
        buf += rn;
        len -= rn;
        ret += rn;
    }
    get_lib_error();
    return ret;
}
LIBSSH2_SFTP_HANDLE* ssh2_t::sftp_opendir(const std::string& path)
{
    if (NULL==m_sftp)
    {
        return NULL;
    }
    LIBSSH2_SFTP_HANDLE* sftp_handle = NULL;
    while ((sftp_handle=libssh2_sftp_opendir(m_sftp, path.c_str()))==NULL
        && libssh2_session_last_errno(m_session)==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    get_lib_error();
    return sftp_handle;
}
int ssh2_t::sftp_closedir(LIBSSH2_SFTP_HANDLE* sftp_handle)
{
    return sftp_close_handle(sftp_handle);
}
int ssh2_t::sftp_readdir(LIBSSH2_SFTP_HANDLE* sftp_handle, char* buf, size_t buflen, LIBSSH2_SFTP_ATTRIBUTES* attrs)
{
    int rc = 0;
    while ((rc=libssh2_sftp_readdir(sftp_handle, buf, buflen, attrs))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
int ssh2_t::sftp_unlink(const std::string& path)
{
    int rc = 0;
    while ((rc=libssh2_sftp_unlink(m_sftp, path.c_str()))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
int ssh2_t::sftp_mkdir(const std::string& path, long mode)
{
    int rc = 0;
    while ((rc=libssh2_sftp_mkdir(m_sftp, path.c_str(), mode))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
int ssh2_t::sftp_rmdir(const std::string& path)
{
    int rc = 0;
    while ((rc=libssh2_sftp_rmdir(m_sftp, path.c_str()))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
int ssh2_t::sftp_stat(const std::string& path, LIBSSH2_SFTP_ATTRIBUTES* attrs)
{
    int rc = 0;
    while ((rc=libssh2_sftp_stat(m_sftp, path.c_str(), attrs))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
int ssh2_t::sftp_lstat(const std::string& path, LIBSSH2_SFTP_ATTRIBUTES* attrs)
{
    int rc = 0;
    while ((rc=libssh2_sftp_lstat(m_sftp, path.c_str(), attrs))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
int ssh2_t::sftp_setstat(const std::string& path, LIBSSH2_SFTP_ATTRIBUTES* attrs)
{
    int rc = 0;
    while ((rc=libssh2_sftp_setstat(m_sftp, path.c_str(), attrs))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
int ssh2_t::sftp_readlink(const std::string& path, char* buf, size_t buflen)
{
    int rc = 0;
    while ((rc=libssh2_sftp_readlink(m_sftp, path.c_str(), buf, buflen))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
int ssh2_t::sftp_symlink(const std::string& path, const std::string& link)
{
    int rc = 0;
    while ((rc=libssh2_sftp_symlink(m_sftp, path.c_str(), (char*)link.c_str()))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
int ssh2_t::sftp_realpath(const std::string& path, char* buf, size_t buflen)
{
    int rc = 0;
    while ((rc=libssh2_sftp_readlink(m_sftp, path.c_str(), buf, buflen))==LIBSSH2_ERROR_EAGAIN
        && waitsocket()>=0);
    return rc;
}
void ssh2_t::init_attrs(LIBSSH2_SFTP_ATTRIBUTES* attrs, struct stat* st)
{
    bzero(attrs, sizeof(*attrs));
    attrs->filesize = st->st_size;
    attrs->flags |= LIBSSH2_SFTP_ATTR_SIZE;
    attrs->permissions = st->st_mode;
    attrs->flags |= LIBSSH2_SFTP_ATTR_PERMISSIONS;
    attrs->atime = st->st_atime;
    attrs->mtime = st->st_mtime;
    attrs->flags |= LIBSSH2_SFTP_ATTR_ACMODTIME;
}
int ssh2_t::sftp_copy_file(const std::string& local, const std::string& remote, char* buf, size_t buflen)
{
    struct stat st;
    bzero(&st, sizeof(st));
    stat(local.c_str(), &st);
    if (!S_ISREG(st.st_mode) && !S_ISLNK(st.st_mode))
    {
        return -1;
    }
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    init_attrs(&attrs, &st);
    int fd = ::open(local.c_str(), O_RDONLY);
    if (fd<0)
    {
        return -1;
    }
    LIBSSH2_SFTP_HANDLE* handle = NULL;
    handle = sftp_open(remote, LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC, attrs.permissions&0777);
    if (NULL==handle)
    {
        ::close(fd);
        return -1;
    }
    int ret = 0;
    while (1)
    {
        int rn=0, wn=0;
        while ((rn=::read(fd, buf, buflen))==-1 && EAGAIN==errno);
        if (rn>0)
        {
            wn = sftp_write(handle, buf, rn);
            if (wn!=rn)
            {
                get_lib_error();
                ret = -1;
                break;
            }
        }
        if (rn<=0)
        {
            if (rn<0) ret=-1;
            break;
        }
    };
    sftp_close(handle);
    ::close(fd);
    sftp_setstat(remote, &attrs);
    return ret;
}
int ssh2_t::sftp_copy_link(const std::string& local, const std::string& remote, char* buf, size_t buflen)
{
    if (::readlink(local.c_str(), buf, buflen)<0)
    {
        return -1;
    }
    return sftp_symlink(remote, buf);
}
int ssh2_t::sftp_copy_dir(const std::string& local, const std::string& remote, char* buf, size_t buflen)
{
    int ret = 0;
    struct stat st;
    bzero(&st, sizeof(st));
    stat(local.c_str(), &st);
    if (!S_ISDIR(st.st_mode))
    {
        return -1;
    }
    DIR* dir = ::opendir(local.c_str());
    if (NULL==dir)
    {
        return -1;
    }
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    init_attrs(&attrs, &st);
    sftp_mkdir(remote, attrs.permissions&0777);
    if (sftp_stat(remote, &attrs))
    {
        return -1;
    }
    struct dirent entry, *pentry;
    while (readdir_r(dir, &entry, &pentry)==0)
    {
        if (NULL==pentry)
        {
            break;
        }
        if (strncmp(entry.d_name, ".", 1)==0 || strncmp(entry.d_name, "..", 2)==0)
        {
            continue;
        }
        std::string this_local = local + "/" + entry.d_name;
        std::string this_remote = remote + "/" + entry.d_name;
        sftp_copy(this_local, this_remote, buf, buflen);
    }
    ::closedir(dir);
    dir = NULL;
    return ret;
}
int ssh2_t::sftp_copy(const std::string& local, const std::string& remote, char* buf/*=NULL*/, size_t buflen/*=0*/)
{
    std::string local_fix = fixpath(local);
    std::string remote_fix = fixpath(remote);
    struct stat st;
    bzero(&st, sizeof(st));
    if (::stat(local_fix.c_str(), &st))
    {
        get_sys_error();
        return -1;
    }
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    bzero(&attrs, sizeof(attrs));
    if (sftp_lstat(remote_fix, &attrs)==0 && attrs.flags&LIBSSH2_SFTP_ATTR_PERMISSIONS && LIBSSH2_SFTP_S_ISDIR(attrs.permissions))
    {
        remote_fix = remote_fix + "/" + basename(local_fix);
    }
    int ret = 0;
    int alloc_buf = 0;
    if (NULL==buf)
    {
        buflen = 1024*2;
        buf = new char[buflen];
        alloc_buf = 1;
    }
    if (S_ISREG(st.st_mode))
    {
        ret = sftp_copy_file(local_fix, remote_fix, buf, buflen);
    }
    else if (S_ISLNK(st.st_mode))
    {
        ret = sftp_copy_link(local_fix, remote_fix, buf, buflen);
    }
    else if (S_ISDIR(st.st_mode))
    {
        ret = sftp_copy_dir(local_fix, remote_fix, buf, buflen);
    }
    else
    {
        ret = -1;
    }
    if (alloc_buf)
    {
        delete[] buf;
    }
    return ret;
}
int ssh2_t::remote_chown(const std::string& remote, const std::string& user, const std::string& group)
{
    std::string cmdline = std::string("chown ") + user + ":" + group + " " + remote;
    int ret = exec(cmdline, NULL, 0, NULL, m_errmsg, m_errmsg_buflen);
    if (ret!=0)
    {
        m_errno = SSH2_ERRNO_REMOTE_CHOWN_FAILID;
    }
    return ret;
}
int ssh2_t::sendfile(const std::string& local, const std::string& remote, int keep_owner/*=0*/)
{
    clear_error();
    off_t total_send = 0;
    struct stat st;
    if (stat(local.c_str(), &st))
    {
        get_sys_error();
        return -2;
    }
    int ret = 0;
    int fd = 0;  // local file fd
    LIBSSH2_SFTP *sftp_session = NULL;
    LIBSSH2_SFTP_HANDLE *sftp_handle = NULL;
    while  ((sftp_session = libssh2_sftp_init(m_session))==NULL
        && libssh2_session_last_errno(m_session)==LIBSSH2_ERROR_EAGAIN
        /*&& waitsocket()>=0*/);
    if (NULL==sftp_session)
    {
        get_lib_error();
        return -3;
    }
    while ((sftp_handle = libssh2_sftp_open(sftp_session, remote.c_str(),
                LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                st.st_mode))==NULL
            && libssh2_session_last_errno(m_session) == LIBSSH2_ERROR_EAGAIN) ;
    if (NULL==sftp_handle)
    {
        ret = -1;
        get_lib_error();
        goto clearup;
    }
    fd = open(local.c_str(), O_RDONLY);
    if (fd < 0)
    {
        ret = -1;
        get_sys_error();
        goto clearup;
    }
    while (1)
    {
        int nread = ::read(fd, m_buf, m_buflen);
        if (nread <= 0)
        {
            if (nread<0)
            {
                // read出错
                get_sys_error();
            }
            break;
        }
        char *write_ptr = m_buf;
        while (nread>0)
        {
            int nwrite = libssh2_sftp_write(sftp_handle, write_ptr, nread);
            if (LIBSSH2_ERROR_EAGAIN == nwrite)
            {
                continue;
            }
            if (nwrite < 0)
            {
                break;
            }
            else
            {
                total_send += nwrite;
                nread -= nwrite;
                write_ptr += nwrite;
            }
        }
        // 仍有未写入的序列, 则出错推出循环
        if (nread)
        {
            get_lib_error();
            break;
        }
    }
    if (total_send<st.st_size)
    {
        ret = -1;
    }
    if (keep_owner)
    {
        remote_chown(remote, uid_to_name(st.st_uid), gid_to_name(st.st_gid));
    }
clearup:
    if (fd>0) ::close(fd);
    while (sftp_handle && libssh2_sftp_close(sftp_handle) == LIBSSH2_ERROR_EAGAIN);
    while (sftp_session && libssh2_sftp_shutdown(sftp_session) == LIBSSH2_ERROR_EAGAIN);
    sftp_handle = NULL;
    sftp_session = NULL;
    return ret;
}
int ssh2_t::recvfile(const std::string& local, const std::string& remote)
{
    clear_error();
    off_t total_recv = 0;
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    struct stat st;
    LIBSSH2_SFTP *sftp_session = NULL;
    LIBSSH2_SFTP_HANDLE *sftp_handle = NULL;
    int rc = 0;
    int fd = 0;
    int ret = 0;
    while  ((sftp_session = libssh2_sftp_init(m_session))==NULL
        && libssh2_session_last_errno(m_session)==LIBSSH2_ERROR_EAGAIN
        /*&& waitsocket()>=0*/);
    if (NULL==sftp_session)
    {
        get_lib_error();
        return -1;
    }
    while ((sftp_handle = libssh2_sftp_open(sftp_session, remote.c_str(), LIBSSH2_FXF_READ, 0))==NULL
            && libssh2_session_last_errno(m_session) == LIBSSH2_ERROR_EAGAIN) ;
    if (NULL==sftp_handle)
    {
        ret = -1;
        get_lib_error();
        goto clearup;
    }
    while ((rc=libssh2_sftp_stat(sftp_session, remote.c_str(), &attrs)==LIBSSH2_ERROR_EAGAIN));
    if (rc)
    {
        ret = -1;
        get_lib_error();
        goto clearup;
    }
    else
    {
        // FIX ME : 未检查是否为文件
        st.st_size = attrs.flags & LIBSSH2_SFTP_ATTR_SIZE ? attrs.filesize : 0;
        st.st_mode = attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS ? attrs.permissions : 0644;
        st.st_atime = attrs.flags & LIBSSH2_SFTP_ATTR_ACMODTIME ? attrs.atime : time(NULL);
        st.st_mtime = attrs.flags & LIBSSH2_SFTP_ATTR_ACMODTIME ? attrs.mtime : time(NULL);
        fd = open(local.c_str(), O_RDWR|O_CREAT|O_TRUNC, st.st_mode);
    }
    if (fd < 0)
    {
        ret = -1;
        get_sys_error();
        goto clearup;
    }
    while (1)
    {
        int nread = libssh2_sftp_read(sftp_handle, m_buf, m_buflen);
        if (LIBSSH2_ERROR_EAGAIN == nread)
        {
            waitsocket();
            continue;
        }
        if (nread <= 0)
        {
            if (nread < 0)
            {
                // libssh2_channel_read错误
                ret = -1;
                get_lib_error();
            }
            break;
        }
        char *write_ptr = m_buf;
        while (nread>0)
        {
            int nwrite = ::write(fd, write_ptr, nread);
            if (nwrite < 0)
            {
                break;
            }
            else
            {
                total_recv += nwrite;
                nread -= nwrite;
                write_ptr += nwrite;
            }
        }
        if (nread)
        {
            ret = -1;
            get_sys_error();
            break;
        }
    }
    if (attrs.flags & LIBSSH2_SFTP_ATTR_SIZE && total_recv<(off_t)attrs.filesize)
    {
        ret = -1;
    }
clearup:
    if (fd>0) ::close(fd);
    while (sftp_handle && libssh2_sftp_close(sftp_handle) == LIBSSH2_ERROR_EAGAIN);
    while (sftp_session && libssh2_sftp_shutdown(sftp_session) == LIBSSH2_ERROR_EAGAIN);
    sftp_handle = NULL;
    sftp_session = NULL;
    return ret;
}
std::string ssh2_t::uid_to_name(uid_t uid)
{
    std::string name;
    struct passwd pwd, *pret=NULL;
    if (getpwuid_r(uid, &pwd, m_buf, m_buflen, &pret)==0 && pret==&pwd && pwd.pw_name)
    {
        name = pwd.pw_name;
    }
    return name;
}
std::string ssh2_t::gid_to_name(gid_t gid)
{
    std::string name;
    struct group grp, *pret = NULL;
    if (getgrgid_r(gid, &grp, m_buf, m_buflen, &pret)==0 && pret==&grp && grp.gr_name)
    {
        name = grp.gr_name;
    }
    return name;
}
std::string ssh2_t::basename(const std::string& path)
{
    std::string p = fixpath(path);
    std::string::size_type pos = p.rfind('/');
    return p.substr(pos+1);
}
std::string ssh2_t::dirname(const std::string& path)
{
    std::string p = fixpath(path);
    std::string::size_type pos = p.rfind('/');
    return p.substr(0, pos);
}
std::string ssh2_t::fixpath(const std::string& path)
{
    std::string p = path;
    std::string::size_type pos_pre = 0, pos=0;
    while ((pos=p.find("//", pos_pre))!=std::string::npos)
    {
        p.replace(pos, 2, "/");
        pos_pre = pos;
    }
    pos = p.rfind("/");
    if (pos==p.size()-1)
    {
        p = p.substr(0, pos);
    }
    return p;
}
