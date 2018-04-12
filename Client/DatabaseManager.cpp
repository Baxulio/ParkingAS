#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

void DatabaseManager::debugQuery(const QSqlQuery& query)
{
    if (query.lastError().type() == QSqlError::ErrorType::NoError) {
        qDebug() << "Query OK:"  << query.lastQuery();
    } else {
        qWarning() << "Query KO:" << query.lastError().text();
        qWarning() << "Query text:" << query.lastQuery();
    }
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
    mDatabase->setDatabaseName(path);
    mDatabase->setHostName(host);
    mDatabase->setPort(port);
    if(!mDatabase->open(login,password)){
        err = mDatabase->lastError();
    }
    return err;
}

void DatabaseManager::closeConnection()
{
    mDatabase->close();
}

DatabaseManager::DatabaseManager():
    mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL")))

{
}
