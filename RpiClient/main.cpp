#include "mainwindow.h"
#include <QApplication>
#include "WiegandWiring.h"

#define D0_PIN 29
#define D1_PIN 28
#define BAREER_PIN 37

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(rpiclientresources);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Baxulio");
    QCoreApplication::setApplicationName("ParkingAS");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    /* defaults */
    options.d0pin = D0_PIN;
    options.d1pin = D1_PIN;
    options.debug = DEBUG_MODE;
    options.bareerPin = BAREER_PIN;

    if(setup_wiegand_timeout_handler()!=0 &&
            gpio_init(options.d0pin, options.d1pin, options.bareerPin))
        return EXIT_FAILURE;

    wiegand_sequence_reset();

    MainWindow w;
    w.show();

    return app.exec();
}
