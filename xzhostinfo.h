#ifndef XZHOSTINFO_H
#define XZHOSTINFO_H

#include "xzdatebase.h"
#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>

#define NUM_LABEL 7
#define NUM_LINEEDIT 6
#define NUM_PUSHBUTTON 5

enum{TABLE_XZ_HOSTINFO = 0,
    TABLE_XZ_EXECUTE};

class XZItemList
{
public:
    QListWidgetItem* item;
    int key;
};

class XZHostInfo : public QDialog
{
    Q_OBJECT
public:
    //explicit XZHostInfo(QDialog *parent = 0);
    XZHostInfo(XZDatebase* db, int Table = TABLE_XZ_HOSTINFO, QDialog *parent = 0);

private:
    XZDatebase* datebase;
    int m_Table;

    QVector<XZItemList> m_item;
    int m_selectedKey;
    QList<DB_XZ_HOSTINFO> lisHostInfo;
    QList<DB_XZ_EXECUTE> lisExecute;

    QListWidget* listWidget;
    QLabel* l[NUM_LABEL], *Headlabel, *TailLabel;
    QPushButton* b[NUM_PUSHBUTTON];
    QLineEdit* e[NUM_LINEEDIT];

    void InitializeWidget();
    void InitialConnection();

    //table related
    void taSetLabelName();
    void taSetLineEditText();
    void taGetLineEditText();
    void taInitialList();
    void taCreateNewRecord();
    void taDeleteOldRecord();

signals:

private slots:
    void ConnectedList(QListWidgetItem*);
    void onClickSave();
    void onClickNew();
    void onClickDelete();
};

#endif // XZHOSTINFO_H
