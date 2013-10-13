#include "xzdatebase.h"
#include "xzconfig.h"
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QSqlRecord>
XZDatebase::XZDatebase(QObject *parent) :
    QObject(parent)
{
    m_sDBName = "config.db";
}

bool XZDatebase::openDB()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    
    if(QFile::exists(this->m_sDBName))
    {
        db.setDatabaseName(m_sDBName);
        db.open();
    }
    else
    {
        this->initializeDB();
    }

    QSqlQuery cur;
    cur.exec("SELECT SQLITE_VERSION()");
    QString str = "Open Database SQLITE : ";
    if(cur.next())
    {
        for(int i = 0 ; i < cur.record().count(); i++)
            str += cur.value(i).toString();
    }
    else
    {
        str += "failed!";
    }
    xzconfig.Log(LOG_INFO,str);

    return true;
}

QSqlError XZDatebase::lastError()
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
}

bool XZDatebase::deleteDB()
{
    // Close database
    db.close();

    // Remove created database binary file
    return QFile::remove(m_sDBName);
}

bool XZDatebase::initializeDB()
{
    xzconfig.Log(LOG_INFO,"Database is not exist, initialize it!");
    //initialize the Datebase, create the table structure.
    db.setDatabaseName(m_sDBName);
    if(db.open())
    {
        QSqlQuery cur;
        cur.exec("create TABLE XZ_PROFILE(Key INT, SequenceAmount integer, Name TEXT, Desc TEXT)");
        cur.exec("create TABLE XZ_SEQUENCE(key INT, ProfileKey integer, RecID INT, execute1 integer, execute2 integer, execute3 integer, execute4 integer, execute5 integer, execute6 integer, execute7 integer, execute8 integer, execute9 integer, execute10 integer, Name TEXT, Desc TEXT)");
        cur.exec("create TABLE IF NOT EXISTS XZ_EXECUTE(Key integer primary key asc autoincrement, Directory TEXT, Type integer, Command TEXT, Name TEXT, Desc TEXT)");
        cur.exec("create TABLE IF NOT EXISTS XZ_HOSTINFO(Key integer primary key asc autoincrement, IP TEXT, User TEXT, Passwd TEXT, Name TEXT, Desc TEXT)");
        return true;
    }
    else
        return false;
}

bool XZDatebase::insertDB_XZ_HOSTINFO(DB_XZ_HOSTINFO record)
{
    xzconfig.Log(LOG_INFO,"insert into XZ_HOSTINFO ...");
    QSqlQuery cur;
    cur.prepare("INSERT INTO XZ_HOSTINFO (IP, User, Passwd, Name, Desc) "
                  "VALUES (?, ?, ?, ?, ?)");
    //cur.addBindValue(record.Key);
    cur.addBindValue(record.IP);
    cur.addBindValue(record.User);
    cur.addBindValue(record.Passwd);
    cur.addBindValue(record.Name);
    cur.addBindValue(record.Desc);

    cur.exec();
    return true;
}

bool XZDatebase::deleteDB_XZ_HOSTINFO(int Key)
{
    QSqlQuery cur;
    QString str = "delete from XZ_HOSTINFO where Key = ";
    str += QString::number(Key);
    xzconfig.Log(LOG_INFO,str);

    if(cur.exec(str))
    {
        xzconfig.Log(LOG_INFO,(str + " : execute successfully!"));
    }
    else
    {
        xzconfig.Log(LOG_ERROR,(str + " : execute not success, error desc:" + cur.lastError().text()));
        return false;
    }

    return true;
}

bool XZDatebase::getDB_XZ_HOSTINFO(QList<DB_XZ_HOSTINFO>& lisHostInfo)
{
    xzconfig.Log(LOG_INFO,"select * from XZ_HOSTINFO");
    lisHostInfo.clear();

    QSqlQuery cur("select * from XZ_HOSTINFO");
    DB_XZ_HOSTINFO tmpHostInfo;
    while(cur.next())
    {
        tmpHostInfo.Key = cur.value(0).toInt();
        tmpHostInfo.IP = cur.value(1).toString();
        tmpHostInfo.User = cur.value(2).toString();
        tmpHostInfo.Passwd = cur.value(3).toString();
        tmpHostInfo.Name = cur.value(4).toString();
        tmpHostInfo.Desc = cur.value(5).toString();
        lisHostInfo.push_back(tmpHostInfo);
    }

    QString str;
    str = str + "get " + QString::number(lisHostInfo.size()) + " records!";
    xzconfig.Log(LOG_INFO,str);

    return true;
}

bool XZDatebase::modifyDB_XZHOSTINFO(int key, DB_XZ_HOSTINFO item)
{
    QSqlQuery cur;
    QString str("update XZ_HOSTINFO set IP = '");
    str = str + item.IP + "', User = '" + item.User + "', Passwd = '" + item.Passwd + "', Name = '" + item.Name + "', Desc = '" +item.Desc +"' where Key = " + QString::number(key);
    xzconfig.Log(LOG_INFO, str);
    if(!cur.exec(str))
    {
        xzconfig.Log(LOG_ERROR,db.lastError().text());
    }
    return true;
}

bool XZDatebase::insertDB_XZ_EXECUTE(DB_XZ_EXECUTE record)
{
    xzconfig.Log(LOG_INFO,"insert into XZ_EXECUTE ...");
    QSqlQuery cur;
    cur.prepare("INSERT INTO XZ_EXECUTE (Directory, Type, Command, Name, Desc) "
                  "VALUES (?, ?, ?, ?, ?)");
    //cur.addBindValue(record.Key);
    cur.addBindValue(record.Directory);
    cur.addBindValue(record.Type );
    cur.addBindValue(record.Command);
    cur.addBindValue(record.Name);
    cur.addBindValue(record.Desc);
    cur.exec();
    return true;
}
bool XZDatebase::deleteDB_XZ_EXECUTE(int Key)
{
    QSqlQuery cur;
    QString str = "delete from XZ_EXECUTE where Key = ";
    str += QString::number(Key);
    xzconfig.Log(LOG_INFO,str);
    if(cur.exec(str))
    {
        xzconfig.Log(LOG_INFO,(str + " : execute successfully!"));
    }
    else
    {
        xzconfig.Log(LOG_ERROR,(str + " : execute not success, error desc:" + cur.lastError().text()));
        return false;
    }
    return true;
}
bool XZDatebase::getDB_XZ_EXECUTE(QList<DB_XZ_EXECUTE>& lisExecute)
{
    xzconfig.Log(LOG_INFO,"select * from XZ_EXECUTE");
    lisExecute.clear();
    QSqlQuery cur("select * from XZ_EXECUTE");
    DB_XZ_EXECUTE tmpExecute;
    while(cur.next())
    {
        tmpExecute.Key = cur.value(0).toInt();
        tmpExecute.Directory = cur.value(1).toString();
        tmpExecute.Type = cur.value(2).toInt();
        tmpExecute.Command = cur.value(3).toString();
        tmpExecute.Name = cur.value(4).toString();
        tmpExecute.Desc = cur.value(5).toString();
        lisExecute.push_back(tmpExecute);
    }
    QString str;
    str = str + "get " + QString::number(lisExecute.size()) + " records!";
    xzconfig.Log(LOG_INFO,str);
    return true;
}
bool XZDatebase::modifyDB_XZ_EXECUTE(int key, DB_XZ_EXECUTE item)
{
    QSqlQuery cur;
    QString str("update XZ_EXECUTE set Directory = '");
    str = str + item.Directory + "', Type = " + item.Type + ", Command = '" + item.Command + "', Name = '" + item.Name + "', Desc = '" +item.Desc +"' where Key = " + QString::number(key);
    xzconfig.Log(LOG_INFO, str);
    if(!cur.exec(str))
    {
        xzconfig.Log(LOG_ERROR,db.lastError().text());
    }
    return true;
}

