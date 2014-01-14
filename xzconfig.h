#ifndef XZCONFIG_H
#define XZCONFIG_H

#define TEST
#define LOG_LEVEL LOG_INFO

#include <QString>
#include <QDebug>
#include <QObject>

#define XZOUTPUT connect(&xzconfig,SIGNAL(output(QString)),(MainWindow*)mwindow,SLOT(setoutput(QString)));

enum{LOG_INFO = 0,
    LOG_WARNING,
    LOG_ERROR,
    LOG_OUTPUT};

class XZConfig : public QObject
{
    Q_OBJECT

public:
    XZConfig();
    void Log(int,QString);
private:
    int m_iLogLevel;
    QString GetLogDesc(int);

signals:
    void output(QString);
};

static XZConfig xzconfig;

#endif // XZCONFIG_H
