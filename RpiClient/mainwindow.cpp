#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QMessageBox>

#include <QDateTime>

#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    bSettings(new SettingsDialog),
    bsocket(new QTcpSocket(this)),
    label(new QLabel(this))
{
    ui->setupUi(this);

    ui->statusBar->addPermanentWidget(label);

    readSettings();
    initActionsConnections();

    connectingGIF.setFileName(":/images/notConnected.gif");
    waitingGIF.setFileName(":/images/wave.gif");

    //connect(&timer, &QTimer::timeout, this, &MainWindow::setWaitingState);
    connect(bsocket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    connect(bsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displaySocketError(QAbstractSocket::SocketError)));
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
    //////////
    //SERVER//
    //////////
    bsocket->connectToHost(bSettings->serverSettings().host,bSettings->serverSettings().port);
    if(!bsocket->waitForConnected(5000)){
        return;
    }

    setUpServer();

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
}

void MainWindow::makeDisconnection()
{
    bsocket->abort();

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);

    ui->icon_label->setText("<p align=\"center\"><img src=\":/images/warning.png\"/><br/></p><p align=\"center\"><span style=\" font-size:36pt; font-weight:600; color:#ca7b29;\">НЕТ ПОДКЛЮЧЕНИЯ!</span></p>");
    ui->info_label->setText("<span style=\"font-size:26pt; color:#7e7e7e;\">Не удалось наладить сеть!");

    //makeConnection();
}

void MainWindow::wiegandCallback(quint32 value)
{
    QByteArray Buffer;
    QDataStream out(&Buffer,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);

    out<<false<<value;

    bsocket->write(Buffer);
    if(!bsocket->waitForReadyRead(5000)){
        bsocket->flush();
        ui->icon_label->setText("<p align=\"center\"><img src=\":/images/warning.png\"/><br/></p><p align=\"center\"><span style=\" font-size:36pt; font-weight:600; color:#ca7b29;\">НЕУДАЧА!</span></p>");
        ui->info_label->setText("<span style=\"font-size:26pt; color:#7e7e7e;\">Повторите попытку !");
        return;
    }
    ui->info_label->setText(QString("<p><span style=\" font-size:26pt; color:#7e7e7e;\">Код карты : </span><span style=\" font-size:26pt; font-weight:600; color:#7e7e7e;\">%1</span></p>").arg(value));
}

void MainWindow::readSocket()
{
    QByteArray arr=bsocket->readAll();

    QDataStream in(&arr,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_5);

    if(in.status()==QDataStream::ReadCorruptData){
        bsocket->flush();
        ui->icon_label->setText("<p align=\"center\"><img src=\":/images/warning.png\"/><br/></p><p align=\"center\"><span style=\" font-size:36pt; font-weight:600; color:#ca7b29;\">НЕУДАЧА!</span></p>");
        ui->info_label->setText("<span style=\"font-size:26pt; color:#7e7e7e;\">Повторите попытку !");
        return;
    }
    int replyNumber;
    in >> replyNumber;

    switch (replyNumber)
    {
    //ENTER mode
    case Replies::WIEGAND_REGISTERED:{
        waitingGIF.stop();
        ui->icon_label->setText("<p align=\"center\"><img src=\":/images/149_check_ok-512.png\"/><br/></p><p align=\"center\"><span style=\" font-size:36pt; font-weight:600; color:#8dc63f;\">УСПЕШНО!</span></p>");
        openBareer();
        return;
    }
    case Replies::WIEGAND_ALREADY_REGISTERED:{
        QDateTime in_time;
        quint8 in_number;

        in>>in_time>>in_number;
        ui->icon_label->setText("<p align=\"center\"><img src=\":/images/149_check_ok-512.png\"/><br/></p><p align=\"center\"><span style=\" font-size:36pt; font-weight:600; color:#8dc63f;\">УЖЕ ЗАРЕГИСТРИРОВАН!</span></p>");
        ui->in_time_label->setText(QString("<body><p><span style=\" font-size:26pt; color:#7e7e7e;\">Время въезда : </span><span style=\" font-size:26pt; font-weight:600; color:#7e7e7e;\">%1</span></p>").arg(in_time.toString()));
        ui->in_out_number_label->setText(QString("<body><p><span style=\" font-size:26pt; color:#7e7e7e;\">Въезд № : </span><span style=\" font-size:26pt; font-weight:600; color:#7e7e7e;\">%1</span></p>").arg(in_number));
        openBareer();
        return;
    }
        //EXIT mode
    case Replies::WIEGAND_DEACTIVATED:
    case Replies::WIEGAND_ALREADY_DEACTIVATED:{
        QDateTime in_time;
        QDateTime out_time;
        quint8 in_number;
        double price;


        in>>in_time>>in_number>>out_time>>price;

        int diff = in_time.time().secsTo(out_time.time());
        int hours = diff/3600;
        int minutes = (diff%3600)/60;
        int secs = (diff%3600)%60;
        QTime time(hours,minutes,secs);

        ui->icon_label->setText(QString("<p align=\"center\"><span style=\" font-size:72pt;\">%1</span></p>"
                                "<p align=\"center\"><span style=\" font-size:100pt; color:#8dc63f;\">%2 </span><span style=\" font-size:48pt; color:#b6b6b6;\">(UZS)</span></p>"
                                "<p align=\"center\"><br/></p><p align=\"center\"><span style=\" font-size:36pt; font-weight:600; color:#8dc63f;\">ДОБРОГО ВАМ ПУТИ!</span></p>")
                                .arg(time.toString()).arg(price));



        ui->in_time_label->setText(QString("<body><p><span style=\" font-size:26pt; color:#7e7e7e;\">Время въезда : </span><span style=\" font-size:26pt; font-weight:600; color:#7e7e7e;\">%1</span></p>").arg(in_time.toString()));
        ui->out_time_label->setText(QString("<body><p><span style=\" font-size:26pt; color:#7e7e7e;\">Время выезда : </span><span style=\" font-size:26pt; font-weight:600; color:#7e7e7e;\">%1</span></p>").arg(out_time.toString()));
        ui->in_out_number_label->setText(QString("<body><p><span style=\" font-size:26pt; color:#7e7e7e;\">Въезд № : </span><span style=\" font-size:26pt; font-weight:600; color:#7e7e7e;\">%1</span></p>").arg(in_number));

        print();
        openBareer();
        return;
    }
    case Replies::WIEGAND_NOT_REGISTERED:{
        ui->icon_label->setText("<p align=\"center\"><img src=\":/images/warning.png\"/><br/></p><p align=\"center\"><span style=\" font-size:36pt; font-weight:600; color:#ca7b29;\">ОШИБКА!</span></p>");
        ui->info_label->setText("<span style=\"font-size:26pt; color:#7e7e7e;\">Карта не зафиксирована при въезде!");
        return;
    }
        //Setting up
    case Replies::SET_UP:{
        ui->icon_label->setText("<p align=\"center\"><img src=\":/images/149_check_ok-512.png\"/><br/></p><p align=\"center\"><span style=\" font-size:36pt; font-weight:600; color:#ca7b29;\">ПОДКЛЮЧЕНО!</span></p>");
        ui->info_label->setText("<span style=\"font-size:26pt; color:#7e7e7e;\">Приложите карту!");
        return;
    }
    case Replies::DVR_ERROR:{
        showStatusMessage("<font color='red'>DVR Error");
        makeDisconnection();
        return;
    }
    case Replies::SNAPSHOT_FAIL:{
        bsocket->flush();
        ui->icon_label->setText("<p align=\"center\"><img src=\":/images/warning.png\"/><br/></p><p align=\"center\"><span style=\" font-size:36pt; font-weight:600; color:#ca7b29;\">НЕУДАЧА!</span></p>");
        ui->info_label->setText("<span style=\"font-size:26pt; color:#7e7e7e;\">Не удалось снять на камеру!");
        return;
    }
    case Replies::INVALID:
    default:{
        showStatusMessage("Undefined error!");
        makeDisconnection();
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
    QString text = QString("-----------------------------------\n"
                           "Tashkent Trade Center %1\n\n"
                           "Карта: %2\n"
                           "От:    %3\n"
                           "До:    %4\n"
                           "Время: %5\n"
                           "Цена:  %6\n"
                           "\n"
                           "--------------------- Global Defence\n"
                           "\n")
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

void MainWindow::setWaitingState()
{
    if(connectingGIF.state()==QMovie::MovieState::Running)
        connectingGIF.stop();

    ui->icon_label->setMovie(&waitingGIF);
    waitingGIF.start();
    ui->info_label->setText("<span style=\"font-size:26pt; color:#7e7e7e;\">Приложите карту !");
}

void MainWindow::initActionsConnections()
{
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, bSettings, &SettingsDialog::show);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionConnect,&QAction::triggered, this, &MainWindow::makeConnection);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::makeDisconnection);
    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::print);
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

    ui->bareer_label->setText(QString("<p><span style=\" font-size:14pt; color:#7e7e7e;\">Въезд № : "
                                      "</span><span style=\" font-size:14pt; font-weight:600; color:#7e7e7e;\">%1</span></p>").
                              arg(mode.bareerNumber));
    SettingsDialog::DVRSettings dvr;
    dvr.host = settings.value("dvr_host",QString()).toString();
    dvr.user = settings.value("dvr_user",QString()).toString();
    dvr.password = settings.value("dvr_password",QString()).toString();
    dvr.port = settings.value("dvr_port",quint32()).toUInt();
    dvr.connectionMethod = settings.value("dvr_connectionMethod",quint8()).toUInt();

    bSettings->setDvrSettings(dvr);
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
}

void MainWindow::openBareer()
{
    digitalWrite(BAREER_PIN,HIGH);
    delay(500);
    digitalWrite(BAREER_PIN,LOW);
}

void MainWindow::setUpServer()
{
    QByteArray Buffer;
    QDataStream out(&Buffer,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);

    out<<true
      <<bSettings->modeSettings().bareerNumber
     <<bSettings->modeSettings().mode
    <<bSettings->dvrSettings().host;

    bsocket->write(Buffer);
    if(!bsocket->waitForReadyRead(5000)){
        makeDisconnection();
        return;
    }
}
