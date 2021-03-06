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
#include <QItemSelectionModel>
#include "ProxyModel.h"

#include "PriceRules.h"
#include "FileDownloader.h"
#include "PasswordDialog.h"

#include <QPushButton>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QTextDocument>
#include <QTextStream>

#include <QUrl>
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
    //showStatusMessage("<font color='gray'>Disconnected!");
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
    if(ui->status_combo->currentIndex()==1){
        QUrl url(statusModel->data(statusModel->index(index.row(),4)).toString());
        FileDownloader *newDownloader = new FileDownloader(QUrl("http://"+bSettings->serverSettings().host+"/"+url.fileName()),this);
        connect(newDownloader, &FileDownloader::downloaded, [this, newDownloader](){
            QPixmap pix;
            pix.loadFromData(newDownloader->downloadedData());
            this->ui->enterSnapshot->setPixmap(pix);
            newDownloader->deleteLater();
        });
    }
    else
    {
        QUrl url(statusModel->data(statusModel->index(index.row(),7)).toString());

        FileDownloader *newDownloader = new FileDownloader(QUrl("http://"+bSettings->serverSettings().host+"/"+url.fileName()),this);
        connect(newDownloader, &FileDownloader::downloaded, [this, newDownloader](){
            QPixmap pix;
            pix.loadFromData(newDownloader->downloadedData());
            this->ui->enterSnapshot->setPixmap(pix);
            newDownloader->deleteLater();
        });

        url.setUrl(statusModel->data(statusModel->index(index.row(),8)).toString());

        newDownloader = new FileDownloader(QUrl("http://"+bSettings->serverSettings().host+"/"+url.fileName()),this);
        connect(newDownloader, &FileDownloader::downloaded, [this, newDownloader](){
            QPixmap pix;
            pix.loadFromData(newDownloader->downloadedData());
            this->ui->exitSnapshot->setPixmap(pix);
            newDownloader->deleteLater();
        });
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

//void MainWindow::on_in_from_dateTime_dateTimeChanged(const QDateTime &dateTime)
//{
//    proxyModel->setFilterIn_Time_From(dateTime==ui->in_from_dateTime->minimumDateTime()?QDateTime():dateTime);
//    this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
//    QDateTime t= dateTime;
//    ui->in_to_dateTime->setDateTime(QDateTime(t.date(),t.time().addSecs(60)));
//}

//void MainWindow::on_wiegand_id_spin_valueChanged(int arg1)
//{
//    proxyModel->setFilterRf_Id(arg1);
//    this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
//}

//void MainWindow::on_in_to_dateTime_dateTimeChanged(const QDateTime &dateTime)
//{
//    proxyModel->setFilterIn_Time_To(dateTime==ui->in_to_dateTime->maximumDateTime()?QDateTime():dateTime);
//    this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
//}

//void MainWindow::on_out_to_dateTime_dateTimeChanged(const QDateTime &dateTime)
//{
//    proxyModel->setFilterIn_Time_To(dateTime==ui->out_to_dateTime->maximumDateTime()?QDateTime():dateTime);
//    this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
//}

//void MainWindow::on_in_spin_valueChanged(int arg1)
//{
//    proxyModel->setFilterIn(arg1);
//    this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
//}

//void MainWindow::on_out_spin_valueChanged(int arg1)
//{
//    proxyModel->setFilterOut(arg1);
//    this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
//}

void MainWindow::on_status_combo_currentIndexChanged(int index)
{
    //if(ui->actionConnect->isEnabled())return;
    if(index){

        ui->out_spin->setEnabled(false);
        ui->out_group->setEnabled(false);
        ui->total_price_for_today_but->setVisible(false);
        ui->filtred_pric_but->setVisible(false);
        ui->reset_cards->setVisible(true);

        ui->total_sum_label->setText("");
        ui->archive_but->setVisible(false);
        statusModel->setTable("Active");
        proxyModel->in_col=3;
        ui->enterSnapshot->clear();
        ui->enterSnapshot->clear();
    }
    else {

        ui->out_spin->setEnabled(true);
        ui->out_group->setEnabled(true);
        ui->total_price_for_today_but->setVisible(true);
        ui->filtred_pric_but->setVisible(true);
        ui->reset_cards->setVisible(false);
        ui->archive_but->setVisible(true);

        statusModel->setTable("History");
        proxyModel->in_col=4;

        ui->enterSnapshot->clear();
        ui->enterSnapshot->clear();
    }
    if(!statusModel->select()){
        showStatusMessage(QString("<font color='red'>%1").arg(statusModel->lastError().text()));
        makeDisconnection();
        return;
    }
    proxyModel->setHeaders(/*here i could pass criteria*/);
    this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
}

//void MainWindow::on_out_from_dateTime_dateTimeChanged(const QDateTime &dateTime)
//{
//    proxyModel->setFilterIn_Time_From(dateTime==ui->out_from_dateTime->minimumDateTime()?QDateTime():dateTime);
//    QDateTime t= dateTime;
//    ui->out_to_dateTime->setDateTime(QDateTime(t.date(),t.time().addSecs(60)));
//    this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
//}

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

void MainWindow::on_total_price_for_today_but_clicked()
{
    on_clear_button_clicked();
    ui->in_from_dateTime->setDateTime(QDateTime(QDate::currentDate(),QTime(0,0,0)));
    ui->out_to_dateTime->setDateTime(QDateTime(QDate::currentDate(),QTime(23,59,59)));
    on_filterButton_clicked();
    on_filtred_pric_but_clicked();
    this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
}

double MainWindow::on_filtred_pric_but_clicked()
{
    double total = 0;
    int i=proxyModel->rowCount();
    for (i-1; i>=0; i--){
        total += proxyModel->data(proxyModel->index(i,5)).toDouble();
    }
    ui->total_sum_label->setText(QString(QString::number(total,'f',2)+" UZS"));
    QMessageBox::information(this,
                             "Отфильтрованная касса",
                             QString(QString::number(total,'f',2)+" UZS"));
    return total;
}

void MainWindow::on_reset_cards_clicked()
{
    PasswordDialog pd(this, "Вы уверены отменить все не выезжанные карты?");
    if(pd.exec()==QDialog::Accepted && pd.password()==bSettings->serverSettings().password){
        proxyModel->removeRows(0,proxyModel->rowCount());
        statusModel->submitAll();
        this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
    }
    else
        QMessageBox::information(this, "Предупреждение", "Не удалось выполнить операцию!");
}

void MainWindow::on_archive_but_clicked()
{
    PasswordDialog pd(this,"Вы уверены, что вы хотите архивировать историю?");
    if(pd.exec()==QDialog::Accepted && pd.password()==bSettings->serverSettings().password){

        QString str;

        int rows = statusModel->rowCount();
        int cols = statusModel->columnCount();
        for (int i=0; i<rows; i++){
            for(int j=0; j<cols; j++){
                str += statusModel->data(statusModel->index(i,j)).toString();
                str +=", ";
            }
            str+="\n";
        }
        QFile csvFile(QString("%1.csv").arg(QDate::currentDate().toString("dd_MM_yyyy")));
        if(csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate)){
            QTextStream out(&csvFile);
            out<<str;
            csvFile.close();
            statusModel->removeRows(0, rows);
            statusModel->submitAll();
        }
        this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
    }
    else
        QMessageBox::information(this, "Предупреждение", "Не удалось выполнить операцию!");
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(ui->tableView->hasFocus() && event->key() == Qt::Key_Delete){
        QModelIndexList list = ui->tableView->selectionModel()->selectedRows(5);
        foreach (QModelIndex indx, list) {
            proxyModel->removeRow(indx.row());
        }
        statusModel->submitAll();
        return;
    }
    return QMainWindow::keyPressEvent(event);
}

void MainWindow::on_selected_price_clicked()
{
    if(ui->status_combo->currentIndex()!=0)return;
    double temp = 0;
    foreach (QModelIndex indx, ui->tableView->selectionModel()->selectedRows(5)) {
        temp += proxyModel->data(indx).toDouble();
    }
    ui->total_sum_label->setText(QString("Выделенная сумма: <font color='red'>%1").arg(temp));
}

void MainWindow::on_filterButton_clicked()
{
    proxyModel->setIn(ui->in_spin->value());
    proxyModel->setOut(ui->out_spin->value());
    proxyModel->setIn_Time_From(ui->in_from_dateTime->dateTime());
    proxyModel->setOut_Time_From(ui->out_from_dateTime->dateTime());
    proxyModel->setIn_Time_To(ui->in_to_dateTime->dateTime());
    proxyModel->setOut_Time_To(ui->out_to_dateTime->dateTime());
    proxyModel->setRf_id(ui->wiegand_id_spin->value());
    proxyModel->invalidateFilterByMyself();
    this->showStatusMessage(QString("Количество строк в таблице: <font color='red'>%1").arg(proxyModel->rowCount()));
}
