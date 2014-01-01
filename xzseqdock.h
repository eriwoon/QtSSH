#ifndef XZSEQDOCK_H
#define XZSEQDOCK_H

#include <QDockWidget>
#include <QListWidget>
#include <QString>
#include "xzdatebase.h"
#include <QList>
#include <QPushButton>
#include "xzprocessexecute.h"
/*
 *used to set the side bar on main window.
*/
class XZSeqDock : public QDockWidget
{
    Q_OBJECT
public:
    XZSeqDock(const QString&,void*,XZDatebase*);
private:
    //initialize the widget to fill with child-widgets.
    void InitializeWidget(void *);
    //get information from executes by key
    DB_XZ_EXECUTE* getExecuteByKey(int);

    //variables
    QListWidget* seqlist;
    QListWidget* seqDesc;
    XZDatebase* db;
    XZProcessExecute* process;

    QPushButton* start;
    QPushButton* stop;
    QPushButton* refresh;
    QPushButton* clear;

    QList<DB_XZ_HOSTINFO> lisHostInfo;
    QList<DB_XZ_EXECUTE> lisExecute;
    QList<DB_XZ_SEQUENCE> lisSequence;

    //var used to send to thread
    QList<DB_XZ_EXECUTE>* execute;

    int row;

private slots:
    void setItems();
    void onChooseSequence(QListWidgetItem * , QListWidgetItem *);
    void onClickStart();
    void onChangeCurrentExecuteNo(int);
};

#endif // XZSEQDOCK_H
