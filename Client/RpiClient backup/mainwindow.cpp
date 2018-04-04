#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "SettingsDialog.h"

#include <QSettings>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QLabel>

#include <QDateTime>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    bSettings(new SettingsDialog),
    bWiegand(new Wiegand),
    bsocket(new QTcpSocket(this)),
    label(new QLabel)
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(label);

    readSettings();
    initActionsConnections();

    in.setDevice(bsocket);
    in.setVersion(QDataStream::Qt_5_7);
    connect(bWiegand, &Wiegand::onReadyRead, this, &MainWindow::wiegandCallback);
    connect(bsocket, &QIODevice::readyRead, this, &MainWindow::readSocket);
    //    connect(bsocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
    //            this, &MainWindow::displaySocketError);
}

MainWindow::~MainWindow()
{
    delete label;
    delete bSettings;
    delete bWiegand;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //if (maybeSave()) {
    writeSettings();
    event->accept();
    //} else {
    //   event->ignore();
    //}
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Parking"),
                       tr("The <b>Parking</b> is an automated parking system "
                          "with the following functionalities:"
                          "<ul><li>Capture an incoming car</li>"
                          "<li>Work with Wiegand devices</li>"
                          "<li>Run on any platform</li>"
                          "<li>Controll the barrier<li>"
                          "<li>Print cheque respect to minutes<li></ul>"
                          "<p align='right'>Author: <b>Nurnazarov Bakhmanyor Yunuszoda</b></p> <a align='right' href="
                          "'http://bahman.byethost18.com'>[bahman.byethost18.com]</a>"));
}

void MainWindow::showStatusMessage(const QString &message)
{
    label->setText(message);
}

void MainWindow::makeConnection()
{
    ///////////
    //WIEGAND//
    ///////////
    if(!bWiegand->startWiegand(bSettings->wiegandSettings().gpio_0,
                               bSettings->wiegandSettings().gpio_1)){
        showStatusMessage("<font color='red'>Cannot Initialize Wiegand!");
        makeDisconnection();
        return;
    }

    //////////
    //SERVER//
    //////////
    bsocket->connectToHost(bSettings->serverSettings().host,bSettings->serverSettings().port);
    if(!bsocket->isOpen()){
        showStatusMessage("<font color='red'>Cannot connect to server!");
        makeDisconnection();
        return;
    }
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
}

void MainWindow::makeDisconnection()
{
    bWiegand->cancel();
    bsocket->abort();

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
}

void MainWindow::wiegandCallback(int bits, quint32 value)
{
    QMessageBox box("title",
                    "bits="+QString::number(bits)+"\nvalue="+QString::number(value),
                    QMessageBox::Information,
                    QMessageBox::Yes,
                    QMessageBox::No,
                    QMessageBox::Cancel | QMessageBox::Escape);
    box.exec();
    ui->wiegand_label->setText(QString::number(value));
}

void MainWindow::readSocket()
{
    in.startTransaction();

    int replyNumber;
    in >> replyNumber;

    switch (replyNumber) {
    case Replies::WIEGAND_DEACTIVATED:{

        QDateTime in_time;
        QDateTime out_time;
        quint8 in_number;
        double price;

        in>>in_time>>out_time>>in_number>>price;
        if(!in.commitTransaction())
            return;
        ui->enter_number_label->setText(QString::number(in_number));
        ui->enter_time_label->setText(in_time.toString());
        ui->exit_time_label->setText(out_time.toString());
        ui->exit_number_label->setText(ui->barrier_number_spin->text());
        ui->price_label->setText(QString("%1").arg(price));
        showStatusMessage("<font color='green'>WIEGAND ID IS DEACTIVATED");
        break;
    }
    case Replies::WIEGAND_NOT_REGISTERED:
        showStatusMessage("<font color='red'>WIEGAND ID IS NOT REGISTERED");
        break;
    case Replies::WIEGAND_REGISTERED:
    case Replies::WIEGAND_ALREADY_REGISTERED:
        showStatusMessage("<font color='green'>WIEGAND ID IS ALREADY REGISTERED");
        //openBareer();
        break;
    case Replies::SET_UP:
        showStatusMessage("<font color='green'>SOCKET SET UP");
        break;

    default:
        break;
    }
    if (!in.commitTransaction())
        return;
}

void MainWindow::initActionsConnections()
{
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, bSettings, &SettingsDialog::show);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionConnect,&QAction::triggered, this, &MainWindow::makeConnection);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::makeDisconnection);
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }

    if(!settings.contains("server_host")){
        return;
    }
    SettingsDialog::ServerSettings server;
    server.host = settings.value("server_host",QString()).toString();
    server.user = settings.value("server_user",QString()).toString();
    server.password = settings.value("server_password",QString()).toString();
    server.port = settings.value("server_port",quint32()).toUInt();

    bSettings->setServerSettings(server);

    SettingsDialog::DVRSettings dvr;
    dvr.host = settings.value("dvr_host",QString()).toString();
    dvr.user = settings.value("dvr_user",QString()).toString();
    dvr.password = settings.value("dvr_password",QString()).toString();
    dvr.port = settings.value("dvr_port",quint32()).toUInt();
    dvr.connectionMethod = settings.value("dvr_connectionMethod",quint8()).toUInt();

    bSettings->setDvrSettings(dvr);

    SettingsDialog::WiegandSettings wiegand;
    wiegand.gpio_0 = settings.value("wiegand_gpio_0",quint8()).toUInt();
    wiegand.gpio_1 = settings.value("wiegand_gpio_1",quint8()).toUInt();

    bSettings->setWiegandSettings(wiegand);
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());

    SettingsDialog::ServerSettings server = bSettings->serverSettings();
    settings.setValue("server_host", server.host);
    settings.setValue("server_user", server.user);
    settings.setValue("server_password", server.password);
    settings.setValue("server_port", server.port);

    SettingsDialog::DVRSettings dvr = bSettings->dvrSettings();
    settings.setValue("dvr_host",dvr.host);
    settings.setValue("dvr_user",dvr.user);
    settings.setValue("dvr_password",dvr.password);
    settings.setValue("dvr_port",dvr.port);
    settings.setValue("dvr_connectionMethod",dvr.connectionMethod);

    SettingsDialog::WiegandSettings wiegand = bSettings->wiegandSettings();
    settings.setValue("wiegand_gpio_0",wiegand.gpio_0);
    settings.setValue("wiegand_gpio_1",wiegand.gpio_1);
}

void MainWindow::on_print_button_clicked()
{

}
