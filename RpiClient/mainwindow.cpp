#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "SettingsDialog.h"
#include <QTcpSocket>

#include <QSettings>

#include <QDesktopWidget>
#include <QCloseEvent>

#include <QLabel>
#include <QMessageBox>

#include <QDateTime>
#include "../core.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    bSettings(new SettingsDialog),
    bWiegand(new Wiegand(this)),
    bsocket(new QTcpSocket(this)),
    label(new QLabel(this)),
    bPrintDialog(new QPrintDialog(&bPrinter,this))
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(label);

    readSettings();
    initActionsConnections();

    connect(bWiegand, &Wiegand::onReadyRead, this, &MainWindow::wiegandCallback);
    connect(bsocket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    connect(bsocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &MainWindow::displaySocketError);
}

MainWindow::~MainWindow()
{
    delete bSettings;
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

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);

    setUpServer();
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
    ui->wiegand_label->setText(QString::number(value));
    if(bSettings->modeSettings().mode){
        ui->enter_number_label->setText(QString::number(bSettings->modeSettings().bareerNumber));
        ui->enter_time_label->setText(QDateTime::currentDateTime().toString());
    }
    else {
        ui->exit_number_label->setText(QString::number(bSettings->modeSettings().bareerNumber));
    }
    QByteArray Buffer;
    QDataStream out(&Buffer,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<false<<value;

    bsocket->write(Buffer);
    bsocket->waitForReadyRead();
}

void MainWindow::readSocket()
{
    QByteArray arr=bsocket->readAll();
    QDataStream in(&arr,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_7);

    in.startTransaction();

    int replyNumber;
    in >> replyNumber;

    switch (replyNumber)
    {
    //Setting up
    case Replies::DVR_ERROR:{
        if(!in.commitTransaction())
            return;
        showStatusMessage("<font color='red'>DVR Error");
        makeDisconnection();
        return;
    }
    case Replies::SET_UP:{
        if(!in.commitTransaction())
            return;
        showStatusMessage("<font color='green'>Successfully Set UP");
        return;
    }

        //ENTER mode
    case Replies::WIEGAND_ALREADY_REGISTERED:{
        QDateTime in_time;
        quint8 in_number;
        in>>in_time>>in_number;

        if(!in.commitTransaction())
            return;

        ui->enter_time_label->setText(in_time.toString());
        ui->enter_number_label->setText(QString::number(in_number));

        showStatusMessage("<font color='green'>WIEGAND ID is already registered!");

        if(bWiegand->openBareer())
            showStatusMessage("<font color='green'>Bareer opened!");

        return;
    }
    case Replies::SNAPSHOT_FAIL:{
        if(!in.commitTransaction())
            return;
        showStatusMessage("<font color='red'>Snapshot failed! Please try again!");
        return;
    }
    case Replies::WIEGAND_REGISTERED:{
        if(!in.commitTransaction())
            return;
        showStatusMessage("<font color='green'>Successfully registered!");
        if(bWiegand->openBareer())
            showStatusMessage("<font color='green'>Bareer opened!");
        return;
    }

        //EXIT mode
    case Replies::WIEGAND_NOT_REGISTERED:{
        if(!in.commitTransaction())
            return;
        showStatusMessage("<font color='red'>WIEGAND ID is not registered!");
        return;
    }
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
        ui->exit_number_label->setText(QString::number(bSettings->modeSettings().bareerNumber));
        ui->price_label->setText(QString("%1").arg(price));

        int diff = in_time.time().secsTo(out_time.time());
        ui->duration_label->setText(QTime(diff/(3600),
                                          (diff%(diff/(3600)))/60,
                                          diff%((diff%(diff/(3600)))/60)).toString());

        showStatusMessage("<font color='green'>WIEGAND ID is deactivated");

        print();
        return;
    }
    }
}

void MainWindow::displaySocketError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        showStatusMessage("<font color='red'>Server closed!");
        break;
    case QAbstractSocket::HostNotFoundError:
        showStatusMessage("<font color='red'>The host was not found. Please check the host name and port settings!");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        showStatusMessage("<font color='red'>The connection was refused by the peer!");
        break;
    default:
        showStatusMessage(QString("<font color='red'>%1!").arg(bsocket->errorString()));
    }
    makeDisconnection();
}

void MainWindow::print()
{
    QString text = QString("Tashkent Trade Center %1\n"
                           "Карта: %2\n"
                           "От: %3\n"
                           "До: %4\n"
                           "Время: %5\n"
                           "Цена: %6")
            .arg(bSettings->modeSettings().bareerNumber)
            .arg(ui->wiegand_label->text())
            .arg(ui->enter_time_label->text())
            .arg(ui->exit_time_label->text())
            .arg(ui->duration_label->text())
            .arg(ui->price_label->text());

    QPainter painter;
    painter.begin(&bPrinter);
    painter.drawText(100, 100, 500, 500, Qt::AlignLeft|Qt::AlignTop, text);
    painter.end();
}

void MainWindow::initActionsConnections()
{
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, bSettings, &SettingsDialog::show);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionConnect,&QAction::triggered, this, &MainWindow::makeConnection);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::makeDisconnection);
    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::print);
    connect(ui->actionPrinterSettings, &QAction::triggered, [this](){bPrintDialog->exec();});
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

    SettingsDialog::ModeSettings mode;
    mode.mode = settings.value("mode",bool()).toBool();
    mode.bareerNumber = settings.value("bareer_number",quint8()).toUInt();

    bSettings->setModeSettings(mode);

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

    if(mode.mode){
        ui->duration_label->setVisible(false);
        ui->price_label->setVisible(false);
        ui->label_5->setVisible(false);
    }
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

    SettingsDialog::ModeSettings mode = bSettings->modeSettings();
    settings.setValue("mode", mode.mode);
    settings.setValue("bareer_number",mode.bareerNumber);

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

void MainWindow::setUpServer()
{
    QByteArray Buffer;
    QDataStream out(&Buffer,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<true
      <<bSettings->modeSettings().bareerNumber
     <<bSettings->modeSettings().mode
    <<bSettings->dvrSettings().host;

    bsocket->write(Buffer);
    bsocket->waitForReadyRead();
}
