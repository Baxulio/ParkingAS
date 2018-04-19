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
#include <QSqlTableModel>
#include "ProxyModel.h"

#include "PriceRules.h"

#include <QPushButton>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QTextDocument>
#include <QTextStream>

#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    label(new QLabel(this)),
    refreshButton(new QPushButton("Обновить", this)),
    bDb(DatabaseManager::instance()),
    bSettings(new SettingsDialog(this)),
    statusModel(new QSqlTableModel(this)),
    proxyModel(new ProxyModel(this))
{
    ui->setupUi(this);

    ui->statusBar->addWidget(refreshButton);
    ui->statusBar->addWidget(label);

    readSettings();
    initActionsConnections();

    statusModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    proxyModel->setSourceModel(statusModel);
    ui->tableView->setModel(proxyModel);

    connect(refreshButton, &QPushButton::clicked, [this](){
        on_status_combo_currentIndexChanged(ui->status_combo->currentIndex());
    });
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::reloadSnapshot);

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
    QSqlError err=bDb.connect("Parking",
                              bSettings->serverSettings().host,
                              bSettings->serverSettings().user,
                              bSettings->serverSettings().password,
                              bSettings->serverSettings().port);
    if(err.type() != QSqlError::NoError){
        showStatusMessage(QString("<font color='red'>%1").arg(err.text()));
        return;
    }
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    showStatusMessage("<font color='green'>Successfully connected!");

    on_status_combo_currentIndexChanged(ui->status_combo->currentIndex());
}

void MainWindow::makeDisconnection()
{
    bDb.closeConnection();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    showStatusMessage("<font color='gray'>Successfully Disconnected!");
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
    QPrinter bPrinter;
    QPrintDialog dialog(&bPrinter, NULL);
    if (dialog.exec() == QDialog::Accepted) {
        document.print(&bPrinter);
    }
}

void MainWindow::reloadSnapshot(const QModelIndex &index, const QModelIndex &prev)
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

void MainWindow::initActionsConnections()
{
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, bSettings, &SettingsDialog::show);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionConnect,&QAction::triggered, this, &MainWindow::makeConnection);
    connect(ui->actionDisconnect,&QAction::triggered, this, &MainWindow::makeDisconnection);
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

    bool c = settings.value("advanced_filters",bool()).toBool();
    ui->show_advanced_button->setChecked(c);
    ui->advanced_options_frame->setVisible(c);
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

    settings.setValue("mode",ui->status_combo->currentIndex());
    settings.setValue("advanced_filters",ui->show_advanced_button->isChecked());
}

void MainWindow::on_in_from_dateTime_dateTimeChanged(const QDateTime &dateTime)
{
    proxyModel->setFilterIn_Time_From(dateTime==ui->in_from_dateTime->minimumDateTime()?QDateTime():dateTime);
    QDateTime t= dateTime;
    ui->in_to_dateTime->setDateTime(QDateTime(t.date(),t.time().addSecs(60)));
}

void MainWindow::on_wiegand_id_spin_valueChanged(int arg1)
{
    proxyModel->setFilterRf_Id(arg1);
}

void MainWindow::on_in_to_dateTime_dateTimeChanged(const QDateTime &dateTime)
{
    proxyModel->setFilterIn_Time_To(dateTime==ui->in_to_dateTime->maximumDateTime()?QDateTime():dateTime);
}

void MainWindow::on_out_to_dateTime_dateTimeChanged(const QDateTime &dateTime)
{
    proxyModel->setFilterIn_Time_To(dateTime==ui->out_to_dateTime->maximumDateTime()?QDateTime():dateTime);
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
    //if(ui->actionConnect->isEnabled())return;
    if(index){
        statusModel->setTable("Active");
        proxyModel->in_col=3;
    }
    else {
        statusModel->setTable("History");
        proxyModel->in_col=4;
    }
    if(!statusModel->select()){
        showStatusMessage(QString("<font color='red'>%1").arg(statusModel->lastError().text()));
        makeDisconnection();
    }
    proxyModel->setHeaders(/*here i could pass criteria*/);
}

void MainWindow::on_out_from_dateTime_dateTimeChanged(const QDateTime &dateTime)
{
    proxyModel->setFilterIn_Time_From(dateTime==ui->out_from_dateTime->minimumDateTime()?QDateTime():dateTime);
    QDateTime t= dateTime;
    ui->out_to_dateTime->setDateTime(QDateTime(t.date(),t.time().addSecs(60)));
}

void MainWindow::on_clear_button_clicked()
{
    ui->wiegand_id_spin->setValue(0);
    ui->in_spin->setValue(0);
    ui->out_spin->setValue(0);
    ui->in_from_dateTime->setDateTime(ui->in_from_dateTime->minimumDateTime());
    ui->in_to_dateTime->setDateTime(ui->in_to_dateTime->maximumDateTime());
    ui->out_from_dateTime->setDateTime(ui->out_from_dateTime->minimumDateTime());
    ui->out_to_dateTime->setDateTime(ui->out_to_dateTime->maximumDateTime());
}

void MainWindow::on_show_advanced_button_clicked(bool checked)
{
    ui->advanced_options_frame->setVisible(checked);
}

void MainWindow::on_priceRules_triggered()
{
    if(ui->actionConnect->isEnabled())
    {
        showStatusMessage("Can't operate! First connect!");
        return;
    }
    PriceRules priceRules(this);
    priceRules.setModal(true);
    priceRules.exec();
}
