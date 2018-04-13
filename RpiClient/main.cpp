#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(rpiclientresources);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Baxulio");
    QCoreApplication::setApplicationName("ParkingAS");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    MainWindow w;
    w.show();

    return app.exec();
}
