#include "xzprocessexecute.h"
#include "mainwindow.h"
#include "xzdatebase.h"
#include "xzconfig.h"
#include <QFileInfo>

XZProcessExecute::XZProcessExecute()
{
}

XZProcessExecute::XZProcessExecute(void *mwindow)
{
    XZOUTPUT
    stop_flag = 0;
    xzconfig.Log(LOG_INFO,"Create instance of XZProcessExecute");
    cssh = new ssh2_t();
    this->lisExecute = 0;
    this->lisHostInfo = 0;
    connect(this,SIGNAL(ChangeStatus(QString)),(MainWindow*)mwindow,SLOT(setStatusBarTip(QString)));
}

void XZProcessExecute::onClickStop()
{
    xzconfig.Log(LOG_INFO,"You clicked STOP");
    emit this->ChangeStatus(QString("stopping..."));
    this->stop_flag = 1;
}

//execute the xz_execute item
void XZProcessExecute::process(QList<DB_XZ_EXECUTE>* lisExecute,QList<DB_XZ_HOSTINFO>* lisHostInfo)
{
    this->lisExecute = lisExecute;
    this->lisHostInfo = lisHostInfo;
    this->start();
}

XZProcessExecute::~XZProcessExecute()
{
    cssh->disconnect();
}

void XZProcessExecute::run()
{
    xzconfig.Log(LOG_INFO,"begin to process xz_execute item");
    emit this->ChangeStatus(QString("Running..."));
    if(lisExecute == 0 || lisHostInfo == 0)
    {
        xzconfig.Log(LOG_ERROR,"XZProcessExecute::process : input parameter not corrent!");
        return;
    }

    xzconfig.Log(LOG_INFO, QString("sizeof(lisExecute) = ") + QString::number(lisExecute->size()));
    for(QList<DB_XZ_EXECUTE>::Iterator cur = this->lisExecute->begin();
        cur != this->lisExecute->end();
        cur ++)
    {
        if(this->stop_flag == 1)
        {
            this->stop_flag = 0;
            break;
        }

        xzconfig.Log(LOG_OUTPUT,QString(tr("Current Item: ") + QString::number(cur - this->lisExecute->begin() + 1) + '|' + cur->Name));

        emit this->CurrentExecuteNo(cur - this->lisExecute->begin() + 1);

        //xzconfig.Log(LOG_INFO, QString("sizeof(lisHostInfo) = ") + QString::number(lisHostInfo->size()));
        QList<DB_XZ_HOSTINFO>::Iterator iter;
        for(iter = lisHostInfo->begin();
            iter != lisHostInfo->end();
            iter ++)
        {
            //xzconfig.Log(LOG_INFO, QString("iter->key = ") + QString::number(iter->Key) + "; cur->Key = " + QString::number(cur->Key));
            if (iter->Key == cur->Host)
            {
                if(cssh->connect(iter->IP.toStdString(),22,iter->User.toStdString(),iter->Passwd.toStdString()))
                {
                    xzconfig.Log(LOG_ERROR,QString(tr("Cannot connect to the server:")) + iter->IP);
                    return;
                }
                xzconfig.Log(LOG_OUTPUT,QString(tr("Connected to the server:")) + iter->Name);
                if(cur->Type == 1)
                {
                    QFileInfo fi(cur->Command);
                    if(cssh->recvfile((QString("recv/") + fi.fileName()).toStdString(),
                            cur->Command.toStdString()))
                    {
                        xzconfig.Log(LOG_ERROR,QString(tr("Cannot get the file:"))
                                     + cur->Command
                                     + tr(", ErrCode: ") + QString::number(cssh->last_errno())
                                     + tr(", ErrDesc: ") + cssh->last_error());
                        emit this->ChangeStatus(QString("ready"));
                        emit this->CurrentExecuteNo(0);
                        return;
                    }
                    xzconfig.Log(LOG_OUTPUT,QString("Successfully get file: ") + cur->Command);
                }
                else if(cur->Type == 0)
                {
                    if(cssh->exec(cur->Command.toStdString()))
                    {
                        xzconfig.Log(LOG_ERROR,QString(tr("Cannot execute the command:"))
                                     + cur->Command
                                     + tr(", ErrCode: ") + QString::number(cssh->last_errno())
                                     + tr(", ErrDesc: ") + cssh->last_error());
                        emit this->ChangeStatus(QString("ready"));
                        emit this->CurrentExecuteNo(0);
                        return;
                    }
                    xzconfig.Log(LOG_OUTPUT,QString("Successfully execute command: ") + cur->Command);
                }

                break;
            }
        }
        if (iter == lisHostInfo->end())
        {
            xzconfig.Log(LOG_ERROR,QString(tr("Cannot find Hostinfo ID: ")) + QString:: number(cur->Host));
        }
    }
    emit this->CurrentExecuteNo(0);
    emit this->ChangeStatus(QString("ready"));
    return;
}
