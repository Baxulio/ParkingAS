#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "SettingsDialog.h"

#include <QSettings>

#include <QDesktopWidget>
#include <QCloseEvent>

#include <QLabel>
#include <QMessageBox>

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include "ProxyModel.h"

#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QTextDocument>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    bSettings(new SettingsDialog),
    label(new QLabel(this)),
    mDb(DatabaseManager::instance()),
    sourceModel(new QSqlTableModel(this)),
    proxyModel(new ProxyModel(this))
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(label);

    bPrintDialog = new QPrintDialog(&bPrinter,this);

    readSettings();
    initActionsConnections();

    proxyModel->setSourceModel(sourceModel);
    ui->tableView->setModel(proxyModel);

    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::reloadSnapshot);

    makeConnection();
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
    QSqlError err=mDb.connect("Parking",bSettings->serverSettings().host,bSettings->serverSettings().user,bSettings->serverSettings().password,bSettings->serverSettings().port);
    if(err.type() != QSqlError::NoError){
        showStatusMessage(QString("<font color='red'>%1").arg(err.text()));
        return;
    }

    ui->actionConnect->setEnabled(false);
    showStatusMessage("<font color='green'>Successfully connected!");

    emit on_status_combo_currentIndexChanged(ui->status_combo->currentIndex());
}

void MainWindow::makeDisconnection()
{
    mDb.closeConnection();
    ui->actionConnect->setEnabled(true);
}

void MainWindow::print()
{
    QString strStream;
    QTextStream out(&strStream);

    const int rowCount = ui->tableView->model()->rowCount();
    const int columnCount = ui->tableView->model()->columnCount();

    out <<  "<html>\n"
            "<head>\n"
            "<meta Content=\"Text/html; charset=Windows-1251\">\n"
         <<  QString("<title>%1 : %2</title>\n").arg("Parking Automated System").arg(QDateTime::currentDateTime().toString())
          <<  "</head>\n"
              "<body bgcolor=#ffffff link=#5000A0>\n"
              "<table border=1 cellspacing=0 cellpadding=2>\n";

    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int column = 0; column < columnCount; column++)
        if (!ui->tableView->isColumnHidden(column))
            out << QString("<th>%1</th>").arg(ui->tableView->model()->headerData(column, Qt::Horizontal).toString());
    out << "</tr></thead>\n";

    // data table
    for (int row = 0; row < rowCount; row++) {
        out << "<tr>";
        for (int column = 0; column < columnCount; column++) {
            if (!ui->tableView->isColumnHidden(column)) {
                QString data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
            }
        }
        out << "</tr>\n";
    }
    out <<  "</table>\n"
            "</body>\n"
            "</html>\n";

    QTextDocument document;
    document.setHtml(strStream);

    document.print(&bPrinter);
}

void MainWindow::reloadSnapshot(const QModelIndex &index)
{
    QString path;
    QPixmap pix;

    if(ui->status_combo->currentIndex()==1){
        path = bSettings->serverSettings().host+proxyModel->data(proxyModel->index(index.row(),3)).toString();
        pix.load(path);
        ui->enterSnapshot->setPixmap(pix);
    }
    else
    {
        path = proxyModel->data(proxyModel->index(index.row(),7)).toString();
        pix.load(path);
        ui->enterSnapshot->setPixmap(pix);

        path = proxyModel->data(proxyModel->index(index.row(),8)).toString();
        pix.load(path);
        ui->exitSnapshot->setPixmap(pix);
    }
}

void MainWindow::onPriceChanged(int value)
{
    if(!bSettings->isHidden())return;

    QSqlQuery query;
    if(!query.exec(QString("INSERT INTO `Parking`.`Price`(`price_per_hour`)"
                           "VALUES (%1);").arg(value)))
        showStatusMessage(QString("<font color = 'red'>%1").arg(query.lastError().driverText()));

}

void MainWindow::initActionsConnections()
{
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, bSettings, &SettingsDialog::show);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionConnect,&QAction::triggered, this, &MainWindow::makeConnection);
    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::print);
    connect(ui->actionPrinterSettings, &QAction::triggered, [this](){bPrintDialog->exec();});
    connect(bSettings, &SettingsDialog::priceChanged, this, &MainWindow::onPriceChanged);
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

    ui->status_combo->setCurrentIndex(settings.value("mode",int()).toInt());

    if(!settings.contains("server_host")){
        return;
    }
    SettingsDialog::ServerSettings server;
    server.host = settings.value("server_host",QString()).toString();
    server.user = settings.value("server_user",QString()).toString();
    server.password = settings.value("server_password",QString()).toString();
    server.port = settings.value("server_port",quint32()).toUInt();

    bSettings->setServerSettings(server);

    SettingsDialog::PriceSettings price;
    price.price = settings.value("price",int()).toInt();
    bSettings->setPriceSettings(price);
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

    SettingsDialog::PriceSettings price = bSettings->priceSettings();
    settings.setValue("price",price.price);

    settings.setValue("mode",ui->status_combo->currentIndex());
}

void MainWindow::on_in_from_dateTime_dateTimeChanged(const QDateTime &dateTime)
{
    proxyModel->setFilterIn_Time_From(dateTime);
    ui->in_to_dateTime->setDateTime(dateTime);
}

void MainWindow::on_wiegand_id_spin_valueChanged(int arg1)
{
    proxyModel->setFilterRf_Id(arg1);
}

void MainWindow::on_in_to_dateTime_dateTimeChanged(const QDateTime &dateTime)
{
    proxyModel->setFilterIn_Time_To(dateTime);
}

void MainWindow::on_out_to_dateTime_dateTimeChanged(const QDateTime &dateTime)
{
    proxyModel->setFilterOut_Time_To(dateTime);
}

void MainWindow::on_in_spin_valueChanged(int arg1)
{
    proxyModel->setFilterIn(arg1);
}

void MainWindow::on_out_spin_valueChanged(int arg1)
{
    proxyModel->setFilterOut(arg1);
}

void MainWindow::on_status_combo_currentIndexChanged(int index)
{
    if(ui->actionConnect->isEnabled())return;
    index?sourceModel->setTable("Active"):sourceModel->setTable("History");

    if(!sourceModel->select()){
        showStatusMessage(QString("<font color='red'>%1").arg(sourceModel->lastError().driverText()));
        makeDisconnection();
        return;
    }
    proxyModel->setHeaders();
}

void MainWindow::on_out_from_dateTime_dateTimeChanged(const QDateTime &dateTime)
{
    proxyModel->setFilterOut_Time_From(dateTime);
    ui->out_to_dateTime->setDateTime(dateTime);
}

void MainWindow::on_clear_button_clicked()
{
    ui->wiegand_id_spin->setValue(0);
    ui->in_spin->setValue(0);
    ui->out_spin->setValue(0);
    ui->in_from_dateTime->setDateTime(QDateTime());
//    ui->in_to_dateTime->setDateTime(QDateTime());
//    ui->out_to_dateTime->setDateTime(QDateTime());
    ui->out_from_dateTime->setDateTime(QDateTime());
}
