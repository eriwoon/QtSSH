#include "xzconfig.h"
#include <iostream>
#include<time.h>

XZConfig::XZConfig()
{
    this->m_iLogLevel = LOG_INFO;
}

QString XZConfig::GetLogDesc(int iLogLevel)
{
    QString ret;
    switch(iLogLevel)
    {
    case LOG_INFO:
        ret = "INFO : ";
        break;
    case LOG_WARNING:
        ret = "WARNING : ";
        break;
    case LOG_ERROR:
        ret = "ERROR : ";
        break;
    default:
    {}
    }
    return ret;
}

void XZConfig::Log(int iLogLevel,QString sLogDesc)
{
    if(this->m_iLogLevel <= iLogLevel)
    {
        //get current time
        static time_t lasttime = 0;
        static int number = 0;
        time_t curtime;
        struct tm * timeinfo;
        time (&curtime);
        if(lasttime != curtime)
        {
            lasttime = curtime;
            if(number != 0 )
                qDebug()<<"-------------------"<<number<<"--------------------";
            number ++;
        }
        timeinfo = localtime (&curtime);

        char buff[20];
        strftime(buff,20,"%F %T",timeinfo);

        qDebug()<<buff<<" : "<<xzconfig.GetLogDesc(LOG_INFO)<<sLogDesc;
    }
}
