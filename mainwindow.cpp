#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xzconfig.h"
#include "xzdatebase.h"
#include "xzhostinfo.h"
#include <QMenu>
#include <QAction>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    xzconfig.Log(LOG_INFO,"Enter MainWindow!");
    this->setWindowTitle("Auto Log Manager");

    QMenu* menufile = this->menuBar()->addMenu("&File");

    QMenu* menuconfig = this->menuBar()->addMenu("&Config");
    QAction* actihost = menuconfig->addAction("Configure &Host");
    connect(actihost,SIGNAL(triggered()),this,SLOT(hostInfo()));

    QMenu* menuhelp = this->menuBar()->addMenu("&Help");
    QAction* actiabout = menuhelp->addAction("&About");
    connect(actiabout,SIGNAL(triggered()),this,SLOT(aboutInfo()));

    this->statusBar()->setStatusTip("Ready");

    datebase = new XZDatebase();
    //datebase->deleteDB();
    datebase->openDB();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::hostInfo()
{
    xzconfig.Log(LOG_INFO,"Clicked Configure Host!");
    XZHostInfo* hostinfo = new XZHostInfo(datebase, NULL);
    hostinfo->show();
}

void MainWindow::aboutInfo()
{
    xzconfig.Log(LOG_INFO,"Clicked about!");
}
