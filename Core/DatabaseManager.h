#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <memory>
#include <QString>

class QSqlQuery;
class QSqlDatabase;
class QSqlError;

class DatabaseManager
{
public:
    static void debugQuery(const QSqlQuery& query);

    static DatabaseManager& instance();
    ~DatabaseManager();

    QSqlError connect(const QString& path = "Parking" ,const QString& host = "localhost", const QString& login = "Parking", const QString& password = "", int port = 3306);
    void closeConnection();
protected:
    DatabaseManager();
    DatabaseManager& operator=(const DatabaseManager& rhs);

public:
    std::unique_ptr<QSqlDatabase> bDatabase;
};

#endif // DATABASEMANAGER_H
