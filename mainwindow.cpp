#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xzconfig.h"
#include "xzdatebase.h"
#include "xzhostinfo.h"
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QTextEdit>
#include <QDir>
#include "xzssh.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("Auto Log Manager"));

    this->initialTextEdit();
    connect(&xzconfig,SIGNAL(output(QString)),this,SLOT(setoutput(QString)));
    xzconfig.Log(LOG_INFO,"Enter MainWindow!");

    //create connnection to databse
    db = new XZDatebase(this);
    db->openDB();

    //add status bar
    status = new QLabel("ready");
    this->statusBar()->addPermanentWidget(status);

    //create default directory
    QDir dir("recv");
    if(!dir.exists())
        dir.mkpath(dir.absolutePath());

    this->dock = new XZSeqDock(tr("Task List"),this,db);
    this->dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->dock->setAllowedAreas(Qt::LeftDockWidgetArea);
    this->addDockWidget(Qt::LeftDockWidgetArea, this->dock);

    xzconfig.Log(LOG_OUTPUT,tr("Cautions: Always click refresh after modified any configures!"));

    QMenu* menuconfig = this->menuBar()->addMenu("&Config");
    QAction* actihost = menuconfig->addAction("Configure &Host");
    connect(actihost,SIGNAL(triggered()),this,SLOT(hostInfo()));
    QAction* actiExec = menuconfig->addAction("Configure &Executable");
    connect(actiExec,SIGNAL(triggered()),this,SLOT(executable()));
    QAction* actiSeq = menuconfig->addAction("Configure &Sequence");
    connect(actiSeq, SIGNAL(triggered()),this,SLOT(sequence()));

    menuconfig->addAction(this->dock->toggleViewAction());

    QMenu* menuhelp = this->menuBar()->addMenu("&Help");
    QAction* actiabout = menuhelp->addAction("&About");
    connect(actiabout,SIGNAL(triggered()),this,SLOT(aboutInfo()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::hostInfo()
{
    xzconfig.Log(LOG_INFO,"Clicked Configure Host!");
    XZHostInfo* hostinfo = new XZHostInfo(db, this, TABLE_XZ_HOSTINFO);
    hostinfo->show();
}

void MainWindow::aboutInfo()
{
    xzconfig.Log(LOG_INFO,"Clicked about!");
    QMessageBox::about(0,"about","<h1>QtSSH</h1><a href=\"https://github.com/eriwoon/QtSSH\">https://github.com/eriwoon/QtSSH</a><p>Copyright (c) 2013 XIAO Zhen(eric.xiaozhen@huawei.com)</p><p>Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:</p><p>The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.</p><p>THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</p>");
}

void MainWindow::executable()
{
    xzconfig.Log(LOG_INFO,"Clicked Configure Executable!");
    XZHostInfo* hostinfo = new XZHostInfo(db, this, TABLE_XZ_EXECUTE);
    connect(hostinfo,SIGNAL(refreshItems()),this->dock,SLOT(setItems()));
    hostinfo->show();
}

void MainWindow::sequence()
{
    xzconfig.Log(LOG_INFO,"Clicked Configure Sequence!");
    XZHostInfo* hostinfo = new XZHostInfo(db, this, TABLE_XZ_SEQUENCE);
    hostinfo->show();
}

void MainWindow::initialTextEdit()
{
    this->textEdit = new QTextEdit();
    this->textEdit->setFontPointSize(10);
    this->textEdit->setReadOnly(true);
    this->setCentralWidget(this->textEdit);
}

void MainWindow::setoutput(QString str)
{
    this->textEdit->insertPlainText(str);
    this->textEdit->moveCursor(QTextCursor::End);
}

void MainWindow::clearOutput()
{
    this->textEdit->clear();
}

void  MainWindow::setStatusBarTip(QString str)
{
    this->status->setText(str);
}
