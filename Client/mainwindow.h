#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPrinter>
#include <QPrintDialog>
#include <DatabaseManager.h>
#include <QSqlTableModel>

namespace Ui {
class MainWindow;
}

class SettingsDialog;
class QLabel;
class ProxyModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    SettingsDialog *bSettings;
    QLabel *label;
    QPrintDialog *bPrintDialog;
    QPrinter bPrinter;
    DatabaseManager& mDb;
    QSqlTableModel *sourceModel;
    ProxyModel *proxyModel;


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void about();
    void showStatusMessage(const QString &message);
    void makeConnection();
    void makeDisconnection();

    void print();
    void reloadSnapshot(const QModelIndex &index);
    void onPriceChanged(int value);

    void on_in_from_dateTime_dateTimeChanged(const QDateTime &dateTime);

    void on_wiegand_id_spin_valueChanged(int arg1);

    void on_in_to_dateTime_dateTimeChanged(const QDateTime &dateTime);

    void on_out_to_dateTime_dateTimeChanged(const QDateTime &dateTime);

    void on_in_spin_valueChanged(int arg1);

    void on_out_spin_valueChanged(int arg1);

    void on_status_combo_currentIndexChanged(int index);

    void on_out_from_dateTime_dateTimeChanged(const QDateTime &dateTime);

    void on_clear_button_clicked();

private:
    void initActionsConnections();

    void readSettings();
    void writeSettings();
};

#endif // MAINWINDOW_H
