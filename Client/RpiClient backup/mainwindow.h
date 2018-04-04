#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wiegand.hpp"
#include <QTcpSocket>
#include <QDataStream>
namespace Ui {
class MainWindow;
}

class SettingsDialog;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    SettingsDialog *bSettings;
    QLabel *label;
    Wiegand *bWiegand;
    QTcpSocket *bsocket;
    QDataStream in;

public:
    enum Replies{
        WIEGAND_NOT_REGISTERED=-1,
        WIEGAND_ALREADY_REGISTERED,
        SET_UP,
        WIEGAND_REGISTERED,
        WIEGAND_DEACTIVATED,
        SNAPSHOT_FAIL
    };
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
    void wiegandCallback(int bits, quint32 value);
    void readSocket();

    void on_print_button_clicked();

private:
    void initActionsConnections();

    void readSettings();
    void writeSettings();
};

#endif // MAINWINDOW_H
