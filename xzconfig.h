#ifndef XZCONFIG_H
#define XZCONFIG_H

#include <QString>
#include <QDebug>

enum{LOG_INFO = 0,
    LOG_WARNING,
    LOG_ERROR,
    LOG_NO};

class XZConfig
{
public:
    XZConfig();

private:
    int m_iLogLevel;

private:
    QString GetLogDesc(int);

public:
    void Log(int,QString);
};

static XZConfig xzconfig;

#endif // XZCONFIG_H
