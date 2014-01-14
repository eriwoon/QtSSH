#ifndef XZPROCESSEXECUTE_H
#define XZPROCESSEXECUTE_H
#include <xzdatebase.h>
#include <QList>
#include "xzssh.h"
#include <QThread>

class XZProcessExecute : public QThread
{
    Q_OBJECT
private:
    XZProcessExecute();
public:
    XZProcessExecute(void*);
    ~XZProcessExecute();

    void process(QList<DB_XZ_EXECUTE>*, QList<DB_XZ_HOSTINFO>*);

private:
    XZSSh* cssh;
    QList<DB_XZ_EXECUTE>* lisExecute;
    QList<DB_XZ_HOSTINFO>* lisHostInfo;

    //iterupt during the program running
    int stop_flag;

    void run();

public slots:
    void onClickStop();

signals:
    void ChangeStatus(QString);
    void CurrentExecuteNo(int);
};

#endif // XZPROCESSEXECUTE_H
