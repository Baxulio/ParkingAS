#include <QCoreApplication>
#include "myserver.h"
#include <QSqlDatabase>
#include <QSqlQuery>

static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("Parking");
    db.setHostName("localhost");
    db.setPort(3306);
    if (!db.open("Parking", "")) {
        return false;
    }
//    QSqlQuery query;
//    query.exec("SELECT Db FROM mysql.db WHERE Db ='ParkingAS'");
//    query.next();
//    if(query.value("Db")==""){
//        QString str("");

//        query.exec(str);
//    }
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Baxulio");
    QCoreApplication::setApplicationName("ParkingAS");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    if(!createConnection())
        return EXIT_FAILURE;

    // Create an instance of a server and then start it.
    MyServer server;
    server.startServer();

    return a.exec();
}
