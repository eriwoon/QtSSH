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

XZHostInfo::XZHostInfo(XZDatebase* db,QDialog *parent):
    QDialog(parent)
{
    xzconfig.Log(LOG_INFO,"Created a new Configure Host window!");

    this->datebase = db;
    this->m_selectedKey = 0;

    //interface related
    this->InitializeWidget();
    this->InitialConnection();
    this->InitialList();

    if(this->m_item.size() > 0)
    {
        this->listWidget->setCurrentItem(this->m_item[0].item);
        this->listWidget->itemClicked(this->m_item[0].item);
    }
}

void XZHostInfo::InitializeWidget()
{
    this->setWindowTitle("Configure Host");
    //this->resize(700,550);

    //initialize
    this->listWidget = new QListWidget(this);
    this->l1 = new QLabel("Name:");
    this->l2 = new QLabel("IP:");
    this->l3 = new QLabel("User");
    this->l4 = new QLabel("Passwd:");
    this->l5 = new QLabel("Desc:");
    this->l6 = new QLabel("Key");
    this->l6->hide();

    this->e1 = new QLineEdit();
    this->e2 = new QLineEdit();
    this->e3 = new QLineEdit();
    this->e4 = new QLineEdit();
    this->e5 = new QLineEdit();

    this->b1 = new QPushButton("New");
    connect(this->b1, SIGNAL(clicked()), this, SLOT(onClickNew()));
    this->b2 = new QPushButton("Delete");
    connect(this->b2, SIGNAL(clicked()), this, SLOT(onClickDelete()));
    this->b3 = new QPushButton("Quit");
    connect(this->b3, SIGNAL(clicked()), this, SLOT(close()));
    this->b4 = new QPushButton("Save");
    connect(this->b4, SIGNAL(clicked()), this, SLOT(onClickSave()));

    QVBoxLayout* t1 = new QVBoxLayout();
    t1->addWidget(this->e1);
    t1->addWidget(this->e2);
    t1->addWidget(this->e3);
    t1->addWidget(this->e4);
    t1->addWidget(this->e5);

    QVBoxLayout* t2 = new QVBoxLayout();
    t2->addWidget(this->l1);
    t2->addWidget(this->l2);
    t2->addWidget(this->l3);
    t2->addWidget(this->l4);
    t2->addWidget(this->l5);

    QHBoxLayout* t3 = new QHBoxLayout();
    t3->addLayout(t2);
    t3->addLayout(t1);

    QHBoxLayout* t5 = new QHBoxLayout();
    t5->addStretch();
    t5->addWidget(this->b4);

    QVBoxLayout* t4 = new QVBoxLayout();
    t4->addWidget(this->l6);
    t4->addLayout(t3);
    t4->addLayout(t5);
    t4->addStretch();

    QHBoxLayout* t11 = new QHBoxLayout();
    t11->addWidget(this->b1);
    t11->addWidget(this->b2);

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
    t41->addWidget(this->b3);

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
    //xzconfig.Log(LOG_INFO,"You Clicked the ListWidget!");
    for(QVector<XZItemList>::Iterator iter = this->m_item.begin();
        iter != this->m_item.end();
        iter ++)
    {
        if(iter->item == pos)
        {
            //qDebug()<<iter->key;
            this->m_selectedKey = iter->key;
            for(QList<DB_XZ_HOSTINFO>::Iterator iter = this->lisHostInfo.begin();
                iter != this->lisHostInfo.end();
                iter ++)
            {
                if(iter->Key == this->m_selectedKey)
                {
                    this->e1->setText(iter->Name);
                    this->e2->setText(iter->IP);
                    this->e3->setText(iter->User);
                    this->e4->setText(iter->Passwd);
                    this->e5->setText(iter->Desc);
                    break;
                }
            }
            return;
        }
    }
    xzconfig.Log(LOG_ERROR,"Didn't find the one selected!");
}

void XZHostInfo::onClickSave()
{

    if(this->m_selectedKey > 0)
    {
        for(QList<DB_XZ_HOSTINFO>::Iterator iter = this->lisHostInfo.begin();
            iter != this->lisHostInfo.end();
            iter ++)
        {
            if(iter->Key == this->m_selectedKey)
            {
                iter->Name = this->e1->text();
                iter->IP = this->e2->text();
                iter->User = this->e3->text();
                iter->Passwd = this->e4->text();
                iter->Desc = this->e5->text();

                this->datebase->modifyDB_XZHOSTINFO(this->m_selectedKey,*iter);


            }
        }

        for(QVector<XZItemList>::Iterator iter = this->m_item.begin();
            iter != this->m_item.end();
            iter ++)
        {
            if(iter->key == this->m_selectedKey)
            {
                iter->item->setText(this->e1->text());
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
    DB_XZ_HOSTINFO tmp;
    tmp.Name = "new record";
    datebase->insertDB_XZ_HOSTINFO(tmp);
    this->InitialList();
    this->listWidget->setCurrentItem(this->m_item.last().item);
    this->listWidget->itemClicked(this->m_item.last().item);

}

void XZHostInfo::onClickDelete()
{
    if(this->m_selectedKey > 0)
    {
        datebase->deleteDB_XZ_HOSTINFO(this->m_selectedKey);
        this->InitialList();
    }
}

void XZHostInfo::InitialList()
{
    this->lisHostInfo.clear();
    this->m_item.clear();
    this->listWidget->clear();

    //database related
    datebase->getDB_XZ_HOSTINFO(lisHostInfo);

    XZItemList tmp;
    for(QList<DB_XZ_HOSTINFO>::Iterator iter = lisHostInfo.begin();
        iter != lisHostInfo.end();
        iter ++)
    {
        tmp.item = new QListWidgetItem(iter->Name,this->listWidget);
        tmp.key = iter->Key;
        this->m_item.push_back(tmp);
    }
}
