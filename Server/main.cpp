#include <QCoreApplication>

#include "DatabaseManager.h"
#include "myserver.h"

const QString DATABASE = "Parking";
const QString HOST = "localhost";
const QString USER = "Server";
const QString PASSWORD = "gss2018";
const int PORT = 3306;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Baxulio");
    QCoreApplication::setApplicationName("ParkingAS");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    DatabaseManager &dbm = DatabaseManager::instance();
    QSqlError error = dbm.connect(DATABASE,HOST,USER,PASSWORD,PORT);
    if(error.type() != QSqlError::NoError){
        return EXIT_FAILURE;
    }

    // Create an instance of a server and then start it.
    MyServer server;
    server.startServer();

    return a.exec();
}
