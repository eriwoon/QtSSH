#ifndef XZHOSTINFO_H
#define XZHOSTINFO_H

#include "xzdatebase.h"
#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>

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
    XZHostInfo(XZDatebase* db,QDialog *parent);

private:
    XZDatebase* datebase;

    QVector<XZItemList> m_item;
    int m_selectedKey;
    QList<DB_XZ_HOSTINFO> lisHostInfo;

    QListWidget* listWidget;
    QLabel* l1;
    QLabel* l2;
    QLabel* l3;
    QLabel* l4;
    QLabel* l5;
    QLabel* l6;
    QPushButton* b1;
    QPushButton* b2;
    QPushButton* b3;
    QPushButton* b4;
    QLineEdit* e1;
    QLineEdit* e2;
    QLineEdit* e3;
    QLineEdit* e4;
    QLineEdit* e5;

    void InitialList();
    void InitializeWidget();
    void InitialConnection();
signals:

private slots:
    void ConnectedList(QListWidgetItem*);
    void onClickSave();
    void onClickNew();
    void onClickDelete();
};

#endif // XZHOSTINFO_H
