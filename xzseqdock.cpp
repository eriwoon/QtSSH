#include "xzseqdock.h"
#include "mainwindow.h"
#include "xzconfig.h"
#include "xzssh.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

XZSeqDock::XZSeqDock(const QString & title, void *mwindow,XZDatebase* database)
    :QDockWidget(title),db(database)
{
    XZOUTPUT
    row = -1;
    process = new XZProcessExecute(mwindow);
    this->InitializeWidget(mwindow);
}

void XZSeqDock::InitializeWidget(void *mwindow)
{
    QHBoxLayout* h1 = new QHBoxLayout();
    seqlist = new QListWidget();
    seqDesc = new QListWidget();
    connect(process,SIGNAL(CurrentExecuteNo(int)),this,SLOT(onChangeCurrentExecuteNo(int)));
    connect(seqlist,SIGNAL(activated(QModelIndex)),this,SLOT(onClickStart()));
    connect(seqlist,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(onChooseSequence(QListWidgetItem * , QListWidgetItem *)));


    //set items for seqlist
    this->setItems();

    start = new QPushButton("start");
    start->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    connect(start,SIGNAL(clicked()),this,SLOT(onClickStart()));
    h1->addWidget(seqlist);
    h1->addWidget(start);

    QHBoxLayout* l2 = new QHBoxLayout();
    stop    = new QPushButton("stop");
    connect(stop,SIGNAL(clicked()),process,SLOT(onClickStop()));
    refresh = new QPushButton("refresh");
    connect(refresh,SIGNAL(clicked()),this,SLOT(setItems()));
    clear   = new QPushButton("clear");
    connect(clear,SIGNAL(clicked()),(MainWindow*)mwindow,SLOT(clearOutput()));
    test    = new QPushButton("test");
#ifndef TEST
    test->hide();
#endif
    connect(test,SIGNAL(clicked()),this,SLOT(onClickTest()));

    l2->addWidget(stop);
    l2->addWidget(refresh);
    l2->addWidget(clear);
    l2->addWidget(test);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(h1);
    mainLayout->addLayout(l2);
    mainLayout->addWidget(seqDesc);

    //we need to add an qwidget before add layout into it
    QWidget* widget = new QWidget();
    widget->setLayout(mainLayout);
    this->setWidget(widget);

}

void XZSeqDock::setItems()
{
    xzconfig.Log(LOG_INFO,"refresh Items on SeqList");

    //get information from database
    this->lisHostInfo.clear();
    db->getDB_XZ_HOSTINFO(lisHostInfo);
    this->lisExecute.clear();
    db->getDB_XZ_EXECUTE(this->lisExecute);
    this->lisSequence.clear();
    db->getDB_XZ_SEQUENCE(this->lisSequence);

    seqDesc->clear();
    seqlist->clear();
    for(QList<DB_XZ_SEQUENCE>::Iterator iter = lisSequence.begin();
        iter != lisSequence.end();
        iter ++)
    {
        seqlist->addItem(QString::number(iter->Key) + '|' + iter->Name);
    }
}

void XZSeqDock::onChooseSequence(QListWidgetItem * current , QListWidgetItem * previous)
{
    //remove the description of the previous item
    int prerow = -1;
    if(previous != 0)
    {
        prerow = this->seqlist->row(previous);
        if(prerow >= 0 && prerow < this->lisSequence.size())
            previous->setText(QString::number(this->lisSequence[prerow].Key) + '|' + this->lisSequence[prerow].Name );
    }

    //add description to current item
    if(current != 0)
    {
        row = this->seqlist->row(current);

        if(row >= 0 && row < this->lisSequence.size())
            current->setText(QString::number(this->lisSequence[row].Key) + '|' + this->lisSequence[row].Name + "\n" + this->lisSequence[row].Desc);
    }


    if(row >=0 && row < this->lisSequence.size())
    {
        xzconfig.Log(LOG_INFO,QString("Choosed one sequence, Current Row No.:" + QString::number(row)
                     + "; Key:" + QString::number(this->lisSequence[row].Key)));
        //add the description information to seqDesc
        this->seqDesc->clear();
        DB_XZ_EXECUTE* t = 0;
        for(int i = 0, no = 1; i < SEQUENCE_NUM ; i ++)
        {
            if(this->lisSequence[row].Exe[i] >= 0 && (t = this->getExecuteByKey(this->lisSequence[row].Exe[i])) != 0)
            {
                this->seqDesc->addItem(QString::number(no) + "|" +  t->Name);
                no ++;
            }
        }
    }
}

void XZSeqDock::onClickStart()
{
    xzconfig.Log(LOG_INFO,"Clicked start");
    int row = this->seqlist->currentRow();
    if(row < 0 || row >= this->lisSequence.size())
    {
        xzconfig.Log(LOG_ERROR,tr("Now Row selected!"));
        return;
    }

    xzconfig.Log(LOG_INFO,QString("Current Row is ") + QString::number(row));

    execute = new QList<DB_XZ_EXECUTE>;
    for(int i = 0; i < SEQUENCE_NUM; i++)
    {
        if(this->lisSequence[row].Exe[i] != 0)
            execute->push_back(*getExecuteByKey(this->lisSequence[row].Exe[i]));

    }
    this->process->process(execute, &(this->lisHostInfo));
}

DB_XZ_EXECUTE* XZSeqDock::getExecuteByKey(int key)
{
    for(QList<DB_XZ_EXECUTE>::Iterator iter = this->lisExecute.begin();
        iter != this->lisExecute.end();
        iter ++)
    {
        if(iter->Key == key)
            return &(*iter);
    }
    return 0;
}

void XZSeqDock::onChangeCurrentExecuteNo(int number)
{
    if(row >= 0 && row < this->lisSequence.size())
    {
        this->seqDesc->clear();
        DB_XZ_EXECUTE* t = 0;
        for(int i = 0, no = 1; i < SEQUENCE_NUM ; i ++)
        {
            if(this->lisSequence[row].Exe[i] >= 0 && (t = this->getExecuteByKey(this->lisSequence[row].Exe[i])) != 0)
            {
                if(number == no)
                    this->seqDesc->addItem(QString("->") + QString::number(no) + "|" +  t->Name);
                else
                    this->seqDesc->addItem(QString::number(no) + "|" +  t->Name);
                no ++;
            }
        }
    }
}

void XZSeqDock::onClickTest()
{
    xzconfig.Log(LOG_INFO, "Clicked Test");
    XZSSh xzssh;

    int re = 0;

    xzconfig.Log(LOG_INFO, "xzssh.xzssh_connect(\"127.0.0.1\",22,\"eric\",\"xiaozhen\");");
    re = xzssh.xzssh_connect("127.0.0.1",22,"eric","xiaozhen");
    xzconfig.Log(LOG_INFO, "re = " + QString::number(re));
    if(re != 0)
    {
        xzconfig.Log(LOG_ERROR, xzssh.xzssh_getErrMsg());
    }

    /*xzconfig.Log(LOG_INFO, "xzssh.xzssh_connect(\"146.185.182.192\",22,\"xiaozhen\",\"abcd\");");
    re = xzssh.xzssh_connect("146.185.182.192",22,"xiaozhen","abcd");
    xzconfig.Log(LOG_INFO, "re = " + QString::number(re));
    if(re != 0)
    {
        xzconfig.Log(LOG_ERROR, xzssh.xzssh_getErrMsg());
    }*/

    xzconfig.Log(LOG_INFO, "xzssh.xzssh_connect(\"146.185.182.192\",22,\"xiaozhen\",\"****\");");
    re = xzssh.xzssh_connect("146.185.182.192",22,"xiaozhen","Zxc12Axxici");
    xzconfig.Log(LOG_INFO, "re = " + QString::number(re));
    if(re != 0)
    {
        xzconfig.Log(LOG_ERROR, xzssh.xzssh_getErrMsg());
    }

    QString result;
    xzconfig.Log(LOG_INFO, "xzssh.xzssh_exec(\"date | tee 1.unl\",result);");
    re = xzssh.xzssh_exec("date | tee 1.unl",&result);
    xzconfig.Log(LOG_INFO, "re = " + QString::number(re) + ", " + result);
    if(re != 0)
    {
        xzconfig.Log(LOG_ERROR, xzssh.xzssh_getErrMsg());
    }

    result.clear();
    xzconfig.Log(LOG_INFO, "xzssh.xzssh_exec(\"netstat -ie | tee 2.unl\",result);");
    re = xzssh.xzssh_exec("netstat -ie | tee 2.unl",&result);
    xzconfig.Log(LOG_INFO, "re = " + QString::number(re) + ", " + result);
    if(re != 0)
    {
        xzconfig.Log(LOG_ERROR, xzssh.xzssh_getErrMsg());
    }

    re = xzssh.xzssh_sftpGet("1.unl","1.txt");
    re = xzssh.xzssh_sftpGet("2.unl","2.txt");

}
