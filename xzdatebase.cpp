#include "xzdatebase.h"
#include "xzconfig.h"
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QSqlRecord>
#include "mainwindow.h"

XZDatebase::XZDatebase(void *mwindow, QObject *parent) :
    QObject(parent)
{
    XZOUTPUT

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
        cur.exec("create TABLE IF NOT EXISTS XZ_PROFILE(key integer primary key asc autoincrement, SequenceAmount integer, Name TEXT, Desc TEXT)");
        cur.exec("create TABLE IF NOT EXISTS XZ_SEQUENCE(key integer primary key asc autoincrement, ProfileKey integer, RecID INT, execute1 integer, execute2 integer, execute3 integer, execute4 integer, execute5 integer, execute6 integer, execute7 integer, execute8 integer, execute9 integer, execute10 integer, Name TEXT, Desc TEXT)");
        cur.exec("create TABLE IF NOT EXISTS XZ_EXECUTE(Key integer primary key asc autoincrement, Host integer, Directory TEXT, Type integer, Command TEXT, Name TEXT, Desc TEXT)");
        cur.exec("create TABLE IF NOT EXISTS XZ_HOSTINFO(Key integer primary key asc autoincrement, IP TEXT, User TEXT, Passwd TEXT, Name TEXT, Desc TEXT)");
        cur.exec("create TABLE IF NOT EXISTS XZ_CONFIGURE(Key integeer primary key asc autoincrement, AttributeName TEXT, Attributevalue TEXT)");
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
    cur.prepare("INSERT INTO XZ_EXECUTE (Host, Type, Command, Name, Desc) "
                  "VALUES (?, ?, ?, ?, ?)");
    //cur.addBindValue(record.Key);
    //cur.addBindValue(record.Directory);
    cur.addBindValue(record.Host);
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
        tmpExecute.Host = cur.value(1).toInt();
        //tmpExecute.Directory = cur.value(2).toString();
        tmpExecute.Type = cur.value(3).toInt();
        tmpExecute.Command = cur.value(4).toString();
        tmpExecute.Name = cur.value(5).toString();
        tmpExecute.Desc = cur.value(6).toString();
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
    QString str("update XZ_EXECUTE set ");// set Directory = '");
    str = str //+ item.Directory
            + " Host = " + QString::number(item.Host)
            + ", Type = " + QString::number(item.Type)
            + ", Command = '" + item.Command
            + "', Name = '" + item.Name
            + "', Desc = '" +item.Desc
            + "' where Key = " + QString::number(key);
    xzconfig.Log(LOG_INFO, str);
    if(!cur.exec(str))
    {
        xzconfig.Log(LOG_ERROR,db.lastError().text());
    }
    return true;
}

bool XZDatebase::insertDB_XZ_SEQUENCE(DB_XZ_SEQUENCE record)
{
    xzconfig.Log(LOG_INFO,"insert into XZ_SEQUENCE ...");
    QSqlQuery cur;
    cur.prepare("INSERT INTO XZ_SEQUENCE (ProfileKey, RecID, execute1, execute2, execute3, execute4, execute5, execute6, execute7, execute8, execute9, execute10, Name, Desc) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    //cur.addBindValue(record.Key);
    cur.addBindValue(record.Profile);
    cur.addBindValue(record.Recid);
    for(int i = 0 ;i < SEQUENCE_NUM; i++)
        cur.addBindValue(record.Exe[i]);
    cur.addBindValue(record.Name);
    cur.addBindValue(record.Desc);
    cur.exec();
    return true;
}
bool XZDatebase::deleteDB_XZ_SEQUENCE(int Key)
{
    QSqlQuery cur;
    QString str = "delete from XZ_SEQUENCE where Key = ";
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
bool XZDatebase::getDB_XZ_SEQUENCE(QList<DB_XZ_SEQUENCE>& lisSequence)
{
    xzconfig.Log(LOG_INFO,"select * from XZ_SEQUENCE");
    lisSequence.clear();
    QSqlQuery cur("select * from XZ_SEQUENCE");
    DB_XZ_SEQUENCE tmpSequence;
    while(cur.next())
    {
        tmpSequence.Key = cur.value(0).toInt();
        tmpSequence.Profile = cur.value(1).toInt();
        tmpSequence.Recid = cur.value(2).toInt();
        for(int i = 0 ; i < SEQUENCE_NUM; i++)
            tmpSequence.Exe[i] = cur.value(i + 3).toInt();
        tmpSequence.Name = cur.value(13).toString();
        tmpSequence.Desc = cur.value(14).toString();
        lisSequence.push_back(tmpSequence);
    }
    QString str;
    str = str + "get " + QString::number(lisSequence.size()) + " records!";
    xzconfig.Log(LOG_INFO,str);
    return true;
}
bool XZDatebase::modifyDB_XZ_SEQUENCE(int key, DB_XZ_SEQUENCE item)
{
    QSqlQuery cur;
    QString str("update XZ_SEQUENCE set ProfileKey = ");
    str = str + QString::number(item.Profile)
            + ", Recid = " + QString::number(item.Recid);

    for(int i = 0; i < SEQUENCE_NUM; i++)
        str = str + ", execute" + QString::number(i + 1) + " = " + QString::number(item.Exe[i]);

    str = str + ", Name = '" + item.Name
            + "', Desc = '" + item.Desc
            + "' where Key = " + QString::number(key);
    xzconfig.Log(LOG_INFO, str);
    if(!cur.exec(str))
    {
        xzconfig.Log(LOG_ERROR,db.lastError().text());
    }
    return true;
}

bool XZDatebase::insertDB_XZ_CONFIGURE(DB_XZ_CONFIGURE record)
{
    xzconfig.Log(LOG_INFO,"insert into XZ_CONFIGURE ...");
    QSqlQuery cur;
    cur.prepare("INSERT INTO XZ_CONFIGURE (AttributeName, AttributeValue) "
                  "VALUES (?, ?)");
    cur.addBindValue(record.AttributeName);
    cur.addBindValue(record.AttributeValue);
    cur.exec();
    return true;
}
bool XZDatebase::deleteDB_XZ_CONFIGURE(int Key)
{
    QSqlQuery cur;
    QString str = "delete from XZ_CONFIGURE where Key = ";
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
bool XZDatebase::getDB_XZ_CONFIGURE(QList<DB_XZ_CONFIGURE>& lisConfigure)
{
    xzconfig.Log(LOG_INFO,"select * from XZ_CONFIGURE");
    lisConfigure.clear();
    QSqlQuery cur("select * from XZ_CONFIGURE");
    DB_XZ_CONFIGURE tmp;
    while(cur.next())
    {
        tmp.AttributeName = cur.value(0).toString();
        tmp.AttributeValue = cur.value(1).toString();
        lisConfigure.push_back(tmp);
    }
    xzconfig.Log(LOG_INFO,QString("get ") + QString::number(lisConfigure.size()) + " records!");
    return true;
}
bool XZDatebase::modifyDB_XZ_CONFIGURE(int key, DB_XZ_CONFIGURE item)
{
    QSqlQuery cur;
    QString str = QString("update XZ_CONFIGURE set ")
            + " AttributeName = '" + item.AttributeName
            + "', AttributeValue = '" + item.AttributeValue
            + "' where Key = " + QString::number(key);
    xzconfig.Log(LOG_INFO, str);
    if(!cur.exec(str))
    {
        xzconfig.Log(LOG_ERROR,db.lastError().text());
    }
    return true;
}
