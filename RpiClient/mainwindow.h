#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wiegand.hpp"
#include <QAbstractSocket>

namespace Ui {
class MainWindow;
}

class QTcpSocket;
class SettingsDialog;
class QLabel;
class QPrintDialog;
class QPrinter;

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

    QPrintDialog *bPrintDialog;
    QPrinter bPrinter;

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
    void wiegandCallback(int bits, quint32 value);

    void readSocket();
    void displaySocketError(QAbstractSocket::SocketError socketError);

    void print();
private:
    void setUpServer();
    void initActionsConnections();

    void readSettings();
    void writeSettings();
};

#endif // MAINWINDOW_H
