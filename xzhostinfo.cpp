#include "xzhostinfo.h"
#include "xzconfig.h"
#include "xzdatebase.h"
#include "xzssh.h"

#include <QHostAddress>
#include <QListWidget>
#include <QVector>
#include <QList>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include "mainwindow.h"
#include <QFormLayout>
#include <QVector>

#include <string>

/*XZHostInfo::XZHostInfo(QDialog *parent) :
    QDialog(parent)
{

}*/

XZHostInfo::XZHostInfo(XZDatebase* db, void *mwindow, int table, QDialog *parent):
    QDialog(parent)
{
    XZOUTPUT

    xzconfig.Log(LOG_INFO,"Created a new Configure Host window!");

    this->datebase = db;
    this->m_Table = table;
    this->m_selectedKey = -1;

    this->lisHostInfo.clear();
    datebase->getDB_XZ_HOSTINFO(this->lisHostInfo);
    this->lisExecute.clear();
    datebase->getDB_XZ_EXECUTE(this->lisExecute);
    this->lisSequence.clear();
    datebase->getDB_XZ_SEQUENCE(this->lisSequence);

    //interface related
    this->InitializeWidget(); //layout related
    this->InitialConnection();//create the connection when clicked one item
    this->taInitialList();    //

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

XZHostInfo::~XZHostInfo()
{
    emit refreshItems();
}

void XZHostInfo::InitializeWidget()
{
    for(int i = 0; i < NUM_LABEL ; i++){this->l[i] = 0;}
    for(int i = 0; i < NUM_LINEEDIT; i++){this->e[i]  = 0;}
    for(int i = 0; i < NUM_PUSHBUTTON ; i++){this->b[i] = 0;}
    for(int i = 0; i < NUM_COMBOBOX ; i++){this->c[i] = 0;}
    this->Headlabel = 0;
    this->TailLabel = 0;

    //this->resize(700,550);

    //initialize
    this->listWidget = new QListWidget(this);

    this->b[1] = new QPushButton("New");
    connect(this->b[1], SIGNAL(clicked()), this, SLOT(onClickNew()));
    this->b[2] = new QPushButton("Delete");
    connect(this->b[2], SIGNAL(clicked()), this, SLOT(onClickDelete()));
    this->b[3] = new QPushButton("Quit");
    connect(this->b[3], SIGNAL(clicked()), this, SLOT(close()));
    this->b[4] = new QPushButton("Save");
    connect(this->b[4], SIGNAL(clicked()), this, SLOT(onClickSave()));

    //add customized buttons here
    this->taPushButton();

    QFormLayout* t3 = new QFormLayout();
    this->taSetLabelName();

    for(QVector<XZNameAttr>::Iterator iter = vNames.begin();
        iter != vNames.end();
        iter ++)
    {
        switch(iter->Attr)
        {
        case TABLE_XZ_HOSTINFO:
            this->c[iter - vNames.begin() + 1] = new QComboBox();
            t3->addRow(iter->Name, this->c[iter - vNames.begin() + 1]);
            break;
        case TABLE_XZ_EXECUTE:
            this->c[iter - vNames.begin() + 1] = new QComboBox();
            t3->addRow(iter->Name, this->c[iter - vNames.begin() + 1]);
            break;
        default:
            this->e[iter - vNames.begin() + 1] = new QLineEdit();
            t3->addRow(iter->Name, this->e[iter - vNames.begin() + 1]);
        }
    }

    QHBoxLayout* t5 = new QHBoxLayout();
    t5->addStretch();
    //t5->addWidget(this->b[4]);

    //add customized push buttons
    QHBoxLayout* t6 = new QHBoxLayout();
    for(int i = 5; i < NUM_PUSHBUTTON; i++)
    {
	if( this->b[i] != 0)
		t6->addWidget(this->b[i]);
    }

    QVBoxLayout* t4 = new QVBoxLayout();
    if(this->Headlabel != 0)
        t4->addWidget(this->Headlabel);
    t4->addLayout(t3);
    t4->addLayout(t5);
    if(this->TailLabel != 0)
        t4->addWidget(this->TailLabel);
    t4->addLayout(t6);
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
    //xzconfig.Log(LOG_INFO,"You Clicked the ListWidget!");

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
                iter->item->setText(QString::number(this->m_selectedKey) + "|" + this->e[1]->text());
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

/*initialize the list widget*/
void XZHostInfo::taInitialList()
{
    this->m_item.clear();
    this->listWidget->clear();
    XZItemList tmp;

    switch(this->m_Table)
    {
    case TABLE_XZ_HOSTINFO:
        this->lisHostInfo.clear();
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
        this->lisExecute.clear();

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
    case TABLE_XZ_SEQUENCE:
        this->lisSequence.clear();

        datebase->getDB_XZ_SEQUENCE(this->lisSequence);

        for(QList<DB_XZ_SEQUENCE>::Iterator iter = lisSequence.begin();
            iter != lisSequence.end();
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
/*set the label names and create LineEdit widgets*/
void XZHostInfo::taSetLabelName()
{
    this->vNames.clear();

    switch(this->m_Table)
    {
    case TABLE_XZ_HOSTINFO:
        this->setWindowTitle(tr("Configure Host"));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("Name:"),-1));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("IP:"),-1));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("User"),-1));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("Password:"),-1));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("Desc"),-1));
        break;

    case TABLE_XZ_EXECUTE:
        this->setWindowTitle(tr("Configure Executable"));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("Name:"),-1));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("Host:"),TABLE_XZ_HOSTINFO));
        //this->vNames.append(XZNameAttr::makeNameAttr(tr("Directory:"),-1));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("Type:"),-1));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("Command:"),-1));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("Desc:"),-1));

        this->TailLabel = new QLabel("Note: \n\tType-0:SSH 1:SFTP");
        break;

    case TABLE_XZ_SEQUENCE:
        this->setWindowTitle(tr("Configure Sequence"));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("Name:"),-1));
        this->vNames.append(XZNameAttr::makeNameAttr(tr("Desc:"),-1));
        for(int i = 0 ; i < SEQUENCE_NUM; i ++)
        {
            this->vNames.append(XZNameAttr::makeNameAttr(QString(tr("Execute"))+ QString::number(i + 1),TABLE_XZ_EXECUTE));
        }
        break;
    default:
        break;
    }
\
}

/*To Customize the Text in LineEdit widget
 *The Information is saved in lisXXXinfo variable
 *read from the var and set it into the widget
*/
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
                //this->e[2]->setText(QString::number(iter->Host));
                this->c[2]->clear();
                this->c[2]->addItem("-");
                int focus = 0;
                for(QList<DB_XZ_HOSTINFO>::Iterator hiter = this->lisHostInfo.begin();
                    hiter != this->lisHostInfo.end();
                    hiter ++)
                {
                    this->c[2]->addItem(QString::number(hiter->Key) + '|' + hiter->Name);
                    if(hiter->Key == iter->Host)
                    {
                        //xzconfig.Log(LOG_INFO,QString("HostKey:") + QString::number(hiter->Key) + "ExecuteHost:" + QString::number(iter->Host));
                        focus = hiter - this->lisHostInfo.begin() + 1;
                    }
                }
                this->c[2]->setCurrentIndex(focus);
                //this->e[3]->setText(iter->Directory);
                this->e[3]->setText(QString::number(iter->Type));
                this->e[4]->setText(iter->Command);
                this->e[5]->setText(iter->Desc);
                break;
            }
        }
        break;
    case TABLE_XZ_SEQUENCE:
        for(QList<DB_XZ_SEQUENCE>::Iterator iter = this->lisSequence.begin();
            iter != this->lisSequence.end();
            iter ++)
        {
            if(iter->Key == this->m_selectedKey)
            {
                this->e[1]->setText(iter->Name);
                this->e[2]->setText(iter->Desc);
                for(int i = 0 ;i < SEQUENCE_NUM;i ++)
                    if(this->c[i + 3] != 0)
                    {
                        this->c[i + 3]->clear();
                        this->c[i + 3]->addItem("-");
                        int focus = 0;
                        for(QList<DB_XZ_EXECUTE>::Iterator hiter = this->lisExecute.begin();
                            hiter != this->lisExecute.end();
                            hiter ++)
                        {
                            this->c[i + 3]->addItem(QString::number(hiter->Key) + '|' + hiter->Name);
                            if(hiter->Key == iter->Exe[i])
                            {
                                focus = hiter - this->lisExecute.begin() + 1;
                            }
                        }
                        this->c[i + 3]->setCurrentIndex(focus);
                    }
                break;
            }
        }
        break;
    default:
        break;
    }
}
/*save the information in the LineEdit into the database*/
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
                iter->Name      = this->e[1]->text();
                if(this->c[2]->currentIndex() == 0)
                    iter->Host = 0;
                else
                    iter->Host = lisHostInfo[this->c[2]->currentIndex() - 1].Key;
                iter->Type      = this->e[3]->text().toInt();
                iter->Command   = this->e[4]->text();
                iter->Desc      = this->e[5]->text();

                this->datebase->modifyDB_XZ_EXECUTE(this->m_selectedKey,*iter);
                break;
            }
        }
        break;
    case TABLE_XZ_SEQUENCE:
        for(QList<DB_XZ_SEQUENCE>::Iterator iter = this->lisSequence.begin();
            iter != this->lisSequence.end();
            iter ++)
        {
            if(iter->Key == this->m_selectedKey)
            {
                iter->Name = this->e[1]->text();
                iter->Desc = this->e[2]->text();
                for(int i = 0; i < SEQUENCE_NUM; i++)
                    if(this->c[i+3])
                    {
                        if(this->c[i+3]->currentIndex() == 0)
                            iter->Exe[i] = 0;
                        else
                            iter->Exe[i] = lisExecute[this->c[i+3]->currentIndex() - 1].Key;
                    }
                this->datebase->modifyDB_XZ_SEQUENCE(this->m_selectedKey, *iter);
                break;
            }
        }
        break;
    default:
        break;
    }
}

/*create a new record in database*/
void XZHostInfo::taCreateNewRecord()
{
    DB_XZ_HOSTINFO tmpHostInfo;
    DB_XZ_EXECUTE tmpExecute;
    DB_XZ_SEQUENCE tmpSequence;

    switch(this->m_Table)
    {
    case TABLE_XZ_HOSTINFO:
        tmpHostInfo.Name = "new HostInfo";
        datebase->insertDB_XZ_HOSTINFO(tmpHostInfo);
        break;

    case TABLE_XZ_EXECUTE:
        tmpExecute.Name = "new Execute";
        tmpExecute.Type = 0;
        datebase->insertDB_XZ_EXECUTE(tmpExecute);
        break;
    case TABLE_XZ_SEQUENCE:
        tmpSequence.Name = "new Sequence";
        tmpSequence.Recid = 0;
        for(int i = 0 ; i < SEQUENCE_NUM ; i++)
            tmpSequence.Exe[i] = 0;
        datebase->insertDB_XZ_SEQUENCE(tmpSequence);
        break;
    default:
        break;
    }
}

/*Delete a record that not useful*/
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
    case TABLE_XZ_SEQUENCE:
        if(this->m_selectedKey >= 0)
        {
            datebase->deleteDB_XZ_SEQUENCE(this->m_selectedKey);
            this->taInitialList();
        }
        break;
    default:
        break;
    }
}


void XZHostInfo::taPushButton()
{
    switch(this->m_Table)
    {
    case TABLE_XZ_HOSTINFO:
		this->b[5] = new QPushButton("test");
		connect(this->b[5], SIGNAL(clicked()), this, SLOT(onClickTest()));
		break;
	case TABLE_XZ_EXECUTE:
		break;
    case TABLE_XZ_SEQUENCE:
        break;
	default:
		break;
	}	
}


void XZHostInfo::onClickTest()
{
    /*
	ssh2_t cssh;


	if(!cssh.connect(this->e[2]->text().toStdString(), 22, this->e[3]->text().toStdString(), this->e[4]->text().toStdString()))
	{
        msgbox.setText("Connect Successfully!");
        cssh.disconnect();
    }
    else
    {
        msgbox.setText("The connection is not good!");
    }
    msgbox.exec();
    */
    XZSSh ssh;
    QMessageBox msgbox;

    if(ssh.xzssh_connect(this->e[2]->text(),22,this->e[3]->text(), this->e[4]->text()))
    {
        xzconfig.Log(LOG_ERROR,ssh.xzssh_getErrMsg());
        msgbox.setText("The connection is not good\nErrno:" + QString::number(ssh.xzssh_getErrNo()) + "\nErrMsg:" + ssh.xzssh_getErrMsg());
    }
    else
    {
        msgbox.setText("Connect Successfully!");
    }
    ssh.xzssh_disconnect();
    msgbox.exec();
}

