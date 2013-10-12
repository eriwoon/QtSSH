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

class XZDatebase : public QObject
{
    Q_OBJECT
public:
    explicit XZDatebase(QObject *parent = 0);

public:
    bool openDB();
    bool deleteDB();
    QSqlError lastError();
    bool insertDB_XZ_HOSTINFO(DB_XZ_HOSTINFO);
    bool deleteDB_XZ_HOSTINFO(int);
    bool getDB_XZ_HOSTINFO(QList<DB_XZ_HOSTINFO>&);
    bool modifyDB_XZHOSTINFO(int,DB_XZ_HOSTINFO);

private:
    QSqlDatabase db;
    QString m_sDBName;

private:
    bool initializeDB();

signals:

public slots:

};

#endif // XZDATEBASE_H
