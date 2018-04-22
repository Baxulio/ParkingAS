#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPrinter>
#include <QTcpSocket>
#include <QAbstractSocket>

#include "SettingsDialog.h"
#include "Core.h"
#include "wiringPi.h"

#include <QLabel>
#include <QTimer>
#include <QMovie>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    SettingsDialog *bSettings;

    QTcpSocket *bsocket;
    QDataStream in;

    QPrinter bPrinter;
    QLabel *label;
    QTimer timer;

    QMovie connectingGIF;
    QMovie waitingGIF;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void about();
    void makeConnection();
    void makeDisconnection();

    void readSocket();
    void displaySocketError(QAbstractSocket::SocketError socketError);

    void print();

public slots:
    void wiegandCallback(quint32 value);
    void showStatusMessage(const QString &message);

private:
    void setUpServer();
    void initActionsConnections();

    void readSettings();
    void writeSettings();

    void openBareer();

};

#endif // MAINWINDOW_H
