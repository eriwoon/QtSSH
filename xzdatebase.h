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
    QString Directory;
    int Type;
    QString Command;
    QString Name;
    QString Desc;
};

class XZDatebase : public QObject
{
    Q_OBJECT
public:
    explicit XZDatebase(QObject *parent = 0);

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

private:
    QSqlDatabase db;
    QString m_sDBName;

private:
    bool initializeDB();

signals:

public slots:

};

#endif // XZDATEBASE_H
