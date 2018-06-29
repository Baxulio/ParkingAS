#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <DatabaseManager.h>
#include <QPixmap>
namespace Ui {
class MainWindow;
}

class SettingsDialog;
class QLabel;
class QPushButton;
class ProxyModel;
class QSqlTableModel;

class MainWindow : public QMainWindow

{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    QLabel *label;
    QPushButton *refreshButton;
    DatabaseManager &bDb;
    SettingsDialog *bSettings;
    QSqlTableModel *statusModel;
    ProxyModel *proxyModel;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;    

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void about();
    void showStatusMessage(const QString &message);
    void makeConnection();
    void makeDisconnection();

    void print();
    void reloadSnapshot(const QModelIndex &index, const QModelIndex &prev);

    void on_status_combo_currentIndexChanged(int index);

//    void on_in_from_dateTime_dateTimeChanged(const QDateTime &dateTime);

//    void on_wiegand_id_spin_valueChanged(int arg1);

//    void on_in_to_dateTime_dateTimeChanged(const QDateTime &dateTime);

//    void on_out_to_dateTime_dateTimeChanged(const QDateTime &dateTime);

//    void on_in_spin_valueChanged(int arg1);

//    void on_out_spin_valueChanged(int arg1);

//    void on_out_from_dateTime_dateTimeChanged(const QDateTime &dateTime);

    void on_clear_button_clicked();

    void on_show_advanced_button_clicked(bool checked);

    void on_priceRules_triggered();

    void on_total_price_for_today_but_clicked();

    double on_filtred_pric_but_clicked();

    void on_reset_cards_clicked();

    void on_archive_but_clicked();

    void on_selected_price_clicked();

    void on_filterButton_clicked();

private:
    void initActionsConnections();

    void readSettings();
    void writeSettings();
    void updateTable();


};

#endif // MAINWINDOW_H
