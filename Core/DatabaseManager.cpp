#include "DatabaseManager.h"

#include <QSqlDatabase>

void DatabaseManager::debugQuery(const QSqlQuery& query)
{
    if (query.lastError().type() != QSqlError::ErrorType::NoError
            || query.lastError().number()==2006) {
        instance().bDatabase->close();
        instance().bDatabase->open();
        //        qWarning() << "Query KO:" << query.lastError().text();
        //        qWarning() << "Query text:" << query.lastQuery();
    }
    //    else {
    //        qDebug() << "Query OK:"  << query.lastQuery();
    //    }
}

DatabaseManager&DatabaseManager::instance()
{
    static DatabaseManager singleton;
    return singleton;
}

DatabaseManager::~DatabaseManager()
{
    closeConnection();
}

QSqlError DatabaseManager::connect(const QString &path, const QString &host, const QString &login, const QString &password, int port)
{
    QSqlError err;
    bDatabase->setDatabaseName(path);
    bDatabase->setHostName(host);
    bDatabase->setPort(port);
    if(!bDatabase->open(login,password)){
        err = bDatabase->lastError();
    }
    return err;
}

void DatabaseManager::closeConnection()
{
    bDatabase->close();
}

DatabaseManager::DatabaseManager():
    bDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL")))
{
    bDatabase->setConnectOptions("MYSQL_OPT_RECONNECT=true");
}
