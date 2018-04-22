#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPrinter>
#include <QTcpSocket>
#include <QAbstractSocket>

#include "SettingsDialog.h"
#include "Core.h"
#include "wiringPi.h"

#include <QElapsedTimer>
#include <QLabel>

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
    QElapsedTimer timer;
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

    void on_actionPrinterSettings_triggered();

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
