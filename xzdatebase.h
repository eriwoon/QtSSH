#ifndef XZDATEBASE_H
#define XZDATEBASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QFile>
#include <QString>
#include <QList>

class DB_XZ_HOSTINFO
{
public:
    int Key;
    QString IP;
    QString User;
    QString Passwd;
    QString Name;
    QString Desc;
};
class DB_XZ_EXECUTE
{
public:
    int Key;
    int Host;
    int Type;
    QString Command;
    QString Name;
    QString Desc;
};
class DB_XZ_CONFIGURE
{
public:
    int Key;
    QString AttributeName;
    QString AttributeValue;
};

//the number of the executes of the sequence
#define SEQUENCE_NUM 10

//the data structure of sequence
class DB_XZ_SEQUENCE
{
public:
    int Key;
    int Profile;
    int Recid;
    int Exe[SEQUENCE_NUM];
    QString Name;
    QString Desc;
};

class XZDatebase : public QObject
{
    Q_OBJECT
public:
    explicit XZDatebase(void* mwindow, QObject *parent = 0);

public:
    bool openDB();
    bool deleteDB();
    QSqlError lastError();

    //XZ_HostInfo
    bool insertDB_XZ_HOSTINFO(DB_XZ_HOSTINFO);
    bool deleteDB_XZ_HOSTINFO(int);
    bool getDB_XZ_HOSTINFO(QList<DB_XZ_HOSTINFO>&);
    bool modifyDB_XZHOSTINFO(int,DB_XZ_HOSTINFO);

    //XZ_Execute
    bool insertDB_XZ_EXECUTE(DB_XZ_EXECUTE);
    bool deleteDB_XZ_EXECUTE(int);
    bool getDB_XZ_EXECUTE(QList<DB_XZ_EXECUTE>&);
    bool modifyDB_XZ_EXECUTE(int, DB_XZ_EXECUTE);

    //XZ_Sequence
    bool insertDB_XZ_SEQUENCE(DB_XZ_SEQUENCE);
    bool deleteDB_XZ_SEQUENCE(int);
    bool getDB_XZ_SEQUENCE(QList<DB_XZ_SEQUENCE>&);
    bool modifyDB_XZ_SEQUENCE(int, DB_XZ_SEQUENCE);

    //XZ_CONFIGURE
    bool insertDB_XZ_CONFIGURE(DB_XZ_CONFIGURE);
    bool deleteDB_XZ_CONFIGURE(int);
    bool getDB_XZ_CONFIGURE(QList<DB_XZ_CONFIGURE>&);
    bool modifyDB_XZ_CONFIGURE(int, DB_XZ_CONFIGURE);

private:
    QSqlDatabase db;
    QString m_sDBName;

private:
    bool initializeDB();

public slots:

};

#endif // XZDATEBASE_H
