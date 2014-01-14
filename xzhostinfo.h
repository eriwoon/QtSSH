#ifndef XZHOSTINFO_H
#define XZHOSTINFO_H

#include "xzdatebase.h"
#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>
#include "xzssh.h"
#include <QFormLayout>
#include <QVector>
#include <QComboBox>

#define NUM_LABEL 20
#define NUM_LINEEDIT 20
#define NUM_PUSHBUTTON 10
#define NUM_COMBOBOX 20

enum{TABLE_XZ_HOSTINFO = 0,
    TABLE_XZ_EXECUTE,
    TABLE_XZ_SEQUENCE};

class XZItemList
{
public:
    QListWidgetItem* item;
    int key;
};

class XZNameAttr
{
public:
    XZNameAttr():Attr(0){}
    XZNameAttr(QString s,int A): Name(s),Attr(A){}
    static XZNameAttr makeNameAttr(QString s, int A){return XZNameAttr(s,A);}
    QString Name;
    int Attr;
};


//the configure window, including
class XZHostInfo : public QDialog
{
    Q_OBJECT
public:
    //explicit XZHostInfo(QDialog *parent = 0);
    XZHostInfo(XZDatebase* db, void* mwindow, int Table = TABLE_XZ_HOSTINFO,  QDialog *parent = 0);
    ~XZHostInfo();
private:
    XZDatebase* datebase;
    int m_Table;

    QVector<XZItemList> m_item;
    int m_selectedKey;
    QList<DB_XZ_HOSTINFO> lisHostInfo;
    QList<DB_XZ_EXECUTE> lisExecute;
    QList<DB_XZ_SEQUENCE> lisSequence;

    QVector<XZNameAttr> vNames;

    QListWidget* listWidget;
    QLabel* l[NUM_LABEL], *Headlabel, *TailLabel;
    QPushButton* b[NUM_PUSHBUTTON];
    QLineEdit* e[NUM_LINEEDIT];
    QComboBox* c[NUM_COMBOBOX];

    //Create widget and set layout
    void InitializeWidget();
    void InitialConnection();

    //table related
    void taSetLabelName();
    void taSetLineEditText();
    void taGetLineEditText();
    void taInitialList();
    void taCreateNewRecord();
    void taDeleteOldRecord();
    void taPushButton();

signals:
    void refreshItems();

private slots:
    void ConnectedList(QListWidgetItem*);
    void onClickSave();
    void onClickNew();
    void onClickDelete();
    void onClickTest();
};

#endif // XZHOSTINFO_H
