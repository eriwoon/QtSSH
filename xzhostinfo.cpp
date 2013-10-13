#include "xzhostinfo.h"
#include "xzconfig.h"
#include "xzdatebase.h"

#include <QListWidget>
#include <QVector>
#include <QList>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>

/*XZHostInfo::XZHostInfo(QDialog *parent) :
    QDialog(parent)
{

}*/

XZHostInfo::XZHostInfo(XZDatebase* db,int table, QDialog *parent):
    QDialog(parent)
{
    xzconfig.Log(LOG_INFO,"Created a new Configure Host window!");

    this->datebase = db;
    this->m_Table = table;
    this->m_selectedKey = -1;

    //interface related
    this->InitializeWidget();
    this->InitialConnection();
    this->taInitialList();

    //show the first record, if the first record doesn't exist, disable all the lineedit widget.
    if(this->m_item.size() > 0)
    {
        this->listWidget->setCurrentItem(this->m_item[0].item);
        this->listWidget->itemClicked(this->m_item[0].item);
    }
    else
    {
        for(int i = 0; i < NUM_LINEEDIT; i++)
        {
            if(this->e[i] != 0)
            {
                this->e[i]->clear();
                this->e[i]->setDisabled(true);
            }
        }
    }
}

void XZHostInfo::InitializeWidget()
{
    for(int i = 0; i < NUM_LABEL ; i++){this->l[i] = 0;}
    for(int i = 0; i < NUM_LINEEDIT; i++){this->e[i]  = 0;}
    for(int i = 0; i < NUM_PUSHBUTTON ; i++){this->b[i] = 0;}
    this->Headlabel = 0;
    this->TailLabel = 0;

    //this->resize(700,550);

    //initialize
    this->listWidget = new QListWidget(this);

    this->taSetLabelName();

    this->b[1] = new QPushButton("New");
    connect(this->b[1], SIGNAL(clicked()), this, SLOT(onClickNew()));
    this->b[2] = new QPushButton("Delete");
    connect(this->b[2], SIGNAL(clicked()), this, SLOT(onClickDelete()));
    this->b[3] = new QPushButton("Quit");
    connect(this->b[3], SIGNAL(clicked()), this, SLOT(close()));
    this->b[4] = new QPushButton("Save");
    connect(this->b[4], SIGNAL(clicked()), this, SLOT(onClickSave()));

    QVBoxLayout* t1 = new QVBoxLayout();
    for(int i = 0; i < NUM_LINEEDIT ; i ++)
    {
        if(this->e[i] != 0)
            t1->addWidget(this->e[i]);
    }

    QVBoxLayout* t2 = new QVBoxLayout();
    for(int i = 0; i < NUM_LABEL ; i ++)
    {
        if(this->l[i] != 0)
            t2->addWidget(this->l[i]);
    }

    QHBoxLayout* t3 = new QHBoxLayout();
    t3->addLayout(t2);
    t3->addLayout(t1);

    QHBoxLayout* t5 = new QHBoxLayout();
    t5->addStretch();
    //t5->addWidget(this->b[4]);

    QVBoxLayout* t4 = new QVBoxLayout();
    if(this->Headlabel != 0)
        t4->addWidget(this->Headlabel);
    t4->addLayout(t3);
    t4->addLayout(t5);
    if(this->TailLabel != 0)
        t4->addWidget(this->TailLabel);
    t4->addStretch();

    QHBoxLayout* t11 = new QHBoxLayout();
    t11->addWidget(this->b[1]);
    t11->addWidget(this->b[2]);

    QVBoxLayout* t12 = new QVBoxLayout();
    t12->addWidget(this->listWidget);
    t12->addLayout(t11);

    QGridLayout* t33 = new QGridLayout();
    t33->addLayout(t12,1,1);
    t33->addLayout(t4,1,2);
    t33->setColumnStretch(1,1);
    t33->setColumnStretch(2,2);

    QHBoxLayout* t41 = new QHBoxLayout();
    t41->addStretch();
    t41->addWidget(this->b[4]);
    t41->addWidget(this->b[3]);

    QVBoxLayout* t51 = new QVBoxLayout();
    t51->addLayout(t33);
    t51->addLayout(t41);

    this->setLayout(t51);

    //set connection
}

void XZHostInfo::InitialConnection()
{
    xzconfig.Log(LOG_INFO,"XZHostInfo::InitialConnection()");
    connect(this->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(ConnectedList(QListWidgetItem*)));
}

void XZHostInfo::ConnectedList(QListWidgetItem* pos)
{
    xzconfig.Log(LOG_INFO,"You Clicked the ListWidget!");

    for(QVector<XZItemList>::Iterator iter = this->m_item.begin();
        iter != this->m_item.end();
        iter ++)
    {
        if(iter->item == pos)
        {
            //qDebug()<<iter->key;
            this->m_selectedKey = iter->key;
            this->taSetLineEditText();

            /*if(this->Headlabel != 0)
            {
                QString str("Record Key: ");
                str += QString::number(this->m_selectedKey);
                this->Headlabel->setText(str);
                xzconfig.Log(LOG_INFO, str);
            }*/
            return;
        }
    }
    xzconfig.Log(LOG_ERROR,"Didn't find the one selected!");
}

void XZHostInfo::onClickSave()
{

    if(this->m_selectedKey >= 0)
    {
        this->taGetLineEditText();

        for(QVector<XZItemList>::Iterator iter = this->m_item.begin();
            iter != this->m_item.end();
            iter ++)
        {
            if(iter->key == this->m_selectedKey)
            {
                iter->item->setText(this->e[1]->text());
            }
        }
    }
    else
    {
        xzconfig.Log(LOG_INFO,"No row selected!");
    }
}

void XZHostInfo::onClickNew()
{
    this->taCreateNewRecord();
    this->taInitialList();
    this->listWidget->setCurrentItem(this->m_item.last().item);
    this->listWidget->itemClicked(this->m_item.last().item);

    for(int i = 0; i < NUM_LINEEDIT; i++)
    {
        if(this->e[i] != 0 && !this->e[i]->isEnabled())
        {
            this->e[i]->setEnabled(true);
        }
    }
}

void XZHostInfo::onClickDelete()
{
    xzconfig.Log(LOG_INFO, "Clicked Delete button!");
    this->taDeleteOldRecord();

    if(this->m_item.size() > 0)
    {
        this->listWidget->setCurrentItem(this->m_item[0].item);
        this->listWidget->itemClicked(this->m_item[0].item);
    }
    else
    {
        for(int i = 0; i < NUM_LINEEDIT; i++)
        {
            if(this->e[i] != 0)
            {
                this->e[i]->clear();
                this->e[i]->setDisabled(true);
            }
        }
    }
}

void XZHostInfo::taInitialList()
{
    this->m_item.clear();
    this->listWidget->clear();
    XZItemList tmp;

    switch(this->m_Table)
    {
    case TABLE_XZ_HOSTINFO:
        this->lisHostInfo.clear();

        //database related
        datebase->getDB_XZ_HOSTINFO(lisHostInfo);

        for(QList<DB_XZ_HOSTINFO>::Iterator iter = lisHostInfo.begin();
            iter != lisHostInfo.end();
            iter ++)
        {
            tmp.item = new QListWidgetItem(QString::number(iter->Key) + "|" + iter->Name,this->listWidget);
            tmp.key = iter->Key;
            this->m_item.push_back(tmp);
        }
        break;
    case TABLE_XZ_EXECUTE:
        this->lisHostInfo.clear();

        //database related
        datebase->getDB_XZ_EXECUTE(this->lisExecute);

        for(QList<DB_XZ_EXECUTE>::Iterator iter = lisExecute.begin();
            iter != lisExecute.end();
            iter ++)
        {
            tmp.item = new QListWidgetItem(QString::number(iter->Key) + "|" + iter->Name,this->listWidget);
            tmp.key = iter->Key;
            this->m_item.push_back(tmp);
        }
        break;
    default:
        break;
    }


}

void XZHostInfo::taSetLabelName()
{
    switch (this->m_Table) {
    case TABLE_XZ_HOSTINFO:
        this->setWindowTitle("Configure Host");

        this->l[1] = new QLabel("Name:");
        this->l[2] = new QLabel("IP:");
        this->l[3] = new QLabel("User");
        this->l[4] = new QLabel("Passwd:");
        this->l[5] = new QLabel("Desc:");
        //this->Headlabel = new QLabel();

        this->e[1] = new QLineEdit();
        this->e[2] = new QLineEdit();
        this->e[3] = new QLineEdit();
        this->e[4] = new QLineEdit();
        this->e[5] = new QLineEdit();
        break;
    case TABLE_XZ_EXECUTE:
        this->setWindowTitle("Configure Executable");

        this->l[1] = new QLabel("Name:");
        this->l[2] = new QLabel("Directory:");
        this->l[3] = new QLabel("Type:");
        this->l[4] = new QLabel("Command:");
        this->l[5] = new QLabel("Desc:");
        //this->Headlabel = new QLabel();
        this->TailLabel = new QLabel("Note: \n\tType-0:SSH 1:SFTP");

        this->e[1] = new QLineEdit();
        this->e[2] = new QLineEdit();
        this->e[3] = new QLineEdit();
        this->e[4] = new QLineEdit();
        this->e[5] = new QLineEdit();
        break;
    default:
        break;
    }
}

void XZHostInfo::taSetLineEditText()
{
    switch(this->m_Table)
    {
    case TABLE_XZ_HOSTINFO:
        for(QList<DB_XZ_HOSTINFO>::Iterator iter = this->lisHostInfo.begin();
            iter != this->lisHostInfo.end();
            iter ++)
        {
            if(iter->Key == this->m_selectedKey)
            {
                this->e[1]->setText(iter->Name);
                this->e[2]->setText(iter->IP);
                this->e[3]->setText(iter->User);
                this->e[4]->setText(iter->Passwd);
                this->e[5]->setText(iter->Desc);
                break;
            }
        }
        break;
    case TABLE_XZ_EXECUTE:
        for(QList<DB_XZ_EXECUTE>::Iterator iter = this->lisExecute.begin();
            iter != this->lisExecute.end();
            iter ++)
        {
            if(iter->Key == this->m_selectedKey)
            {
                this->e[1]->setText(iter->Name);
                this->e[2]->setText(iter->Directory);
                this->e[3]->setText(QString::number(iter->Type));
                this->e[4]->setText(iter->Command);
                this->e[5]->setText(iter->Desc);
                break;
            }
        }
        break;
    default:
        break;
    }
}

void XZHostInfo::taGetLineEditText()
{
    switch(this->m_Table){
    case TABLE_XZ_HOSTINFO:
        for(QList<DB_XZ_HOSTINFO>::Iterator iter = this->lisHostInfo.begin();
            iter != this->lisHostInfo.end();
            iter ++)
        {
            if(iter->Key == this->m_selectedKey)
            {
                iter->Name   = this->e[1]->text();
                iter->IP     = this->e[2]->text();
                iter->User   = this->e[3]->text();
                iter->Passwd = this->e[4]->text();
                iter->Desc   = this->e[5]->text();

                this->datebase->modifyDB_XZHOSTINFO(this->m_selectedKey,*iter);


            }
        }
        break;
    case TABLE_XZ_EXECUTE:
        for(QList<DB_XZ_EXECUTE>::Iterator iter = this->lisExecute.begin();
            iter != this->lisExecute.end();
            iter ++)
        {
            if(iter->Key == this->m_selectedKey)
            {
                iter->Name      = this->e[1]->text();
                iter->Directory = this->e[2]->text();
                iter->Type      = this->e[3]->text().toInt();
                iter->Command   = this->e[4]->text();
                iter->Desc      = this->e[5]->text();

                this->datebase->modifyDB_XZ_EXECUTE(this->m_selectedKey,*iter);
            }
        }
        break;
    default:
        break;
    }
}

void XZHostInfo::taCreateNewRecord()
{
    DB_XZ_HOSTINFO tmpHostInfo;
    DB_XZ_EXECUTE tmpExecute;

    switch(this->m_Table)
    {
    case TABLE_XZ_HOSTINFO:
        tmpHostInfo.Name = "new record";
        datebase->insertDB_XZ_HOSTINFO(tmpHostInfo);
        break;

    case TABLE_XZ_EXECUTE:
        tmpExecute.Name = "new record";
        tmpExecute.Type = 0;
        datebase->insertDB_XZ_EXECUTE(tmpExecute);
        break;

    default:
        break;
    }
}

void XZHostInfo::taDeleteOldRecord()
{
    switch(this->m_Table)
    {
    case TABLE_XZ_HOSTINFO:
        if(this->m_selectedKey >= 0)
        {
            datebase->deleteDB_XZ_HOSTINFO(this->m_selectedKey);
            this->taInitialList();
        }
        break;
    case TABLE_XZ_EXECUTE:
        if(this->m_selectedKey >= 0)
        {
            datebase->deleteDB_XZ_EXECUTE(this->m_selectedKey);
            this->taInitialList();
        }
    default:
        break;
    }
}
