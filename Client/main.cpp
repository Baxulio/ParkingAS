#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(clientresources);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Baxulio");
    QCoreApplication::setApplicationName("ParkingAS");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    app.setStyle("Fusion");
    MainWindow w;
    w.show();


    return app.exec();
}
