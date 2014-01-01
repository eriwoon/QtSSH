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
#ifndef _SSH2_H_CLASS_
#define _SSH2_H_CLASS_

#include <libssh2.h>
#include <libssh2_sftp.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/select.h>
#include <string>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
/* 自定义的错误码 */
#define SSH2_ERRNO_INVALID_USING            -200
#define SSH2_ERRNO_REMOTE_CHOWN_FAILID      -201
class ssh2_t
{
public:
    ssh2_t() : m_sock(0), m_session(NULL), m_connected(0), m_sftp(NULL), m_buf(NULL), m_buflen(1024), m_errno(0), m_errmsg(NULL), m_errmsg_buflen(1024)
    {
        m_buf = new char[m_buflen];
        assert(m_buf);
        m_errmsg = new char[m_errmsg_buflen];
        assert(m_errmsg);
    }
    virtual ~ssh2_t()
    {
        disconnect();
        delete[] m_buf;
        m_buf = NULL;
        delete[] m_errmsg;
        m_errmsg = NULL;
    }
    long tvdiff(struct timeval newer, struct timeval older);
    /**********************************************************************************************************************
    * @connect
    * DESCRIPTION
    *       建立到目标主机的ssh连接; 如果此前已连接到其他主机,将断开连接并连接到新的主机
    * PARAMETER LIST
    *       1 ip        std::string     [in]
    *       2 port      unsigned short  [in]
    *       3 username  std::string     [in]
    *       4 password  std::string     [in]
    * RETURN VALUE
    *       0, 成功;
    * ERRNO
    *       该函数将重置实例的errno和errmsg
    *       0        无错误
    *       >0       系统错误
    *       <0,>-100 libssh2错误
    **********************************************************************************************************************/
    int connect(const std::string& ip, unsigned short port, const std::string& username, const std::string& password);
    /**********************************************************************************************************************
    * @connected
    * DESCRIPTION
    *       连接是否建立成功
    * RETURN VALUE
    *       0, 无连接; 非0, 已连接
    * ERRNO
    *       INVALID
    **********************************************************************************************************************/
    int connected()const{return m_connected;}
    int connected(){return m_connected;}
    /**********************************************************************************************************************
    * @disconnect
    * DESCRIPTION
    *       断开已建立的连接
    * ERRNO
    *       INVALID
    **********************************************************************************************************************/
    void disconnect();
    LIBSSH2_CHANNEL* channel_open();
    int channel_close(LIBSSH2_CHANNEL* channel);
    int channel_exec(LIBSSH2_CHANNEL* channel, const std::string& cmdline);
    int channel_write(LIBSSH2_CHANNEL* channel, const char* buf, size_t len);
    int channel_read(LIBSSH2_CHANNEL* channel, char* buf, size_t len);
    /**********************************************************************************************************************
    * @exec
    * DESCRIPTION
    *       通过ssh连接执行命令, 并将执行过程的标准输出和标准出错写入指定的fd和内存buffer
    * PARAMETER LIST
    *       1 cmdline           std::string     [in] [required]     要执行的命令
    *       2 stdout_buf        char*           [mod][optional]     标准输出的buffer;
    *                                                               若NULL,stdout_buf stdout_buflen stdout_recvlen将被忽略;
    *                                                               若!NULL,将写入已NULL结尾的字符串
    *       3 stdout_buflen     int             [in] [optional]     标准输出的buffer长度;
    *                                                               若stdout_buf!=NULL, stdout_buflen必须>0
    *       4 stdout_recvlen    int*            [mod][optional]     若!NULL, *stdout_recvlen记录写入stdout_buf的实际长度(byte)
    *       5 stderr_buf        char*           [mod][optional]     标准错误的buffer;
    *                                                               若NULL,stderr_buf stderr_buflen stderr_recvlen将被忽略
    *                                                               若!NULL,将写入已NULL结尾的字符串
    *       6 stderr_buflen     int             [in] [optional]     标准错误的buffer长度;
    *                                                               若stderr_buf非NULL, stdout_buflen必须>0
    *       7 stderr_recvlen    int*            [mod][optional]     若NULL, *stderr_recvlen记录写入stderr_buf的实际长度(byte)
    *       8 stdout_fd         int             [in] [optional]     若>0, 标准输出将写入stdout_fd
    *       9 stderr_fd         int             [in] [optional]     若>0, 标准错误将写入stderr_fd
    * RETURN VALUE
    *       该函数将重置实例的errno和errmsg
    *       0, 成功;
    *       非零, 失败;
    * ERRNO
    **********************************************************************************************************************/
    int exec(const std::string& cmdline,
        char* stdout_buf=NULL, int stdout_buflen=0, int* stdout_recvlen=NULL,
        char* stderr_buf=NULL, int stderr_buflen=0, int* stderr_recvlen=NULL,
        int stdout_fd=-1, int stder_fd=-1);
    LIBSSH2_SFTP* sftp_init();
    int sftp_shutdown();
    int sftp_close_handle(LIBSSH2_SFTP_HANDLE* sftp_handle);
    LIBSSH2_SFTP_HANDLE* sftp_open(const std::string& path, long flags, long mode);
    int sftp_close(LIBSSH2_SFTP_HANDLE* sftp_handle);
    int sftp_read(LIBSSH2_SFTP_HANDLE* sftp_handle, char* buf, size_t len);
    int sftp_write(LIBSSH2_SFTP_HANDLE* sftp_handle, const char* buf, size_t len);
    LIBSSH2_SFTP_HANDLE* sftp_opendir(const std::string& path);
    int sftp_closedir(LIBSSH2_SFTP_HANDLE* sftp_handle);
    int sftp_readdir(LIBSSH2_SFTP_HANDLE* sftp_handle, char* buf, size_t buflen, LIBSSH2_SFTP_ATTRIBUTES* attrs);
    int sftp_unlink(const std::string& path);
    int sftp_mkdir(const std::string& path, long mode);
    int sftp_rmdir(const std::string& path);
    int sftp_stat(const std::string& path, LIBSSH2_SFTP_ATTRIBUTES* attrs);
    int sftp_lstat(const std::string& path, LIBSSH2_SFTP_ATTRIBUTES* attrs);
    int sftp_setstat(const std::string& path, LIBSSH2_SFTP_ATTRIBUTES* attrs);
    int sftp_readlink(const std::string& path, char* buf, size_t buflen);
    int sftp_symlink(const std::string& path, const std::string& link);
    int sftp_realpath(const std::string& path, char* buf, size_t buflen);
    void init_attrs(LIBSSH2_SFTP_ATTRIBUTES* attrs, struct stat* st);
    int sftp_copy_file(const std::string& local, const std::string& remote, char* buf, size_t buflen);
    int sftp_copy_link(const std::string& local, const std::string& remote, char* buf, size_t buflen);
    int sftp_copy_dir(const std::string& local, const std::string& remote, char* buf, size_t buflen);
    int sftp_copy(const std::string& local, const std::string& remote, char* buf=NULL, size_t buflen=0);
    /**********************************************************************************************************************
    * @remote_chown
    * DESCRIPTION
    *       修改远程主机指定文件的属主和属组
    * PARAMETER LIST
    *       1 remote    std::string     [in]
    *       2 user      std::string     [in]
    *       4 group     std::string     [in]
    * RETURN VALUE
    *       0, 成功;
    * ERRNO
    *       该函数将重置实例的errno和errmsg
    *       0        无错误
    *       >0       系统错误
    *       <0,>-100 libssh2错误
    **********************************************************************************************************************/
    int remote_chown(const std::string& remote, const std::string& user, const std::string& group);
    /**********************************************************************************************************************
    * @ sendfile
    * DESCRIPTION
    *       将本地文件scp到目标机器指定路径. 若目标主机文件已存在将被覆盖 WARNING! 不支持目录
    * PARAMETER LIST
    *       1 lcoal         std::string     [in] [required] 要传输的本地文件
    *       2 remote        std::string     [in] [required] 在目标机器上的全路径(包括名称)
    *       3 keep_owner    int             [in] [optional] 0,属主为连接使用的账户; else, 属主和属组与源文件相同
    * RETURN VALUE
    *       0, 成功;
    * ERRNO
    *       该函数将重置实例的errno和errmsg
    *       0        无错误
    *       >0       系统错误
    *       <0,>-100 libssh2错误
    **********************************************************************************************************************/
    int sendfile(const std::string& local, const std::string& remote, int keep_owner=0);
    /**********************************************************************************************************************
    * @ recvfile
    * DESCRIPTION
    *       将目标机器指定路径文件scp到本地. 若本机文件已存在将被覆盖 WARNING! 不支持目录
    * PARAMETER LIST
    *       1 lcoal         std::string     [in] [required] 要传输的文件在目标机器上的全路径
    *       2 remote        std::string     [in] [required] 本地文件路径(包括名称)
    * RETURN VALUE
    *       0, 成功;
    * ERRNO
    *       该函数将重置实例的errno和errmsg
    *       0        无错误
    *       >0       系统错误
    *       <0,>-100 libssh2错误
    **********************************************************************************************************************/
    int recvfile(const std::string& local, const std::string& remote);
    void clear_error(){m_errno=0;snprintf(m_errmsg, m_errmsg_buflen, "%s", "success");}
    const char* last_error()const{return m_errmsg;}
    int last_errno()const{return m_errno;}
protected:
    int waitsocket(int timeout=-1);
    void get_sys_error(){m_errno = errno; strerror_r(m_errno, m_errmsg, m_errmsg_buflen);}
    void get_lib_error()
    {
        m_errno = libssh2_session_last_errno(m_session);
        char* errmsg = NULL;
        int errmsg_len = 0;
        libssh2_session_last_error(m_session, &errmsg, &errmsg_len, 0);
        if (errmsg_len>0 && errmsg)
        {
            strncpy(m_errmsg, errmsg, m_errmsg_buflen);
        }
    }
    std::string uid_to_name(uid_t uid);
    std::string gid_to_name(gid_t gid);
    void set_nonblocking(int fd);
private:
    /* forbidden copy */
    ssh2_t(const ssh2_t& e);
    ssh2_t& operator=(const ssh2_t& e);
    std::string basename(const std::string& path);
    std::string dirname(const std::string& path);
    std::string fixpath(const std::string& path);
protected:
    struct sockaddr_in  m_sin;
    int                 m_sock;
    LIBSSH2_SESSION*    m_session;
    int                 m_connected;
    LIBSSH2_SFTP*       m_sftp;
    char*               m_buf;
    size_t              m_buflen;
    int                 m_errno;    // m_errno>0的部分是等于系统errno, m_errno<0是libssh2的错误
    char*               m_errmsg;   // NUL结尾的字符串
    size_t              m_errmsg_buflen;
};
#endif //_SSH2_H_CLASS_
