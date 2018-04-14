#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>

#include <QPrinter>
#include <QPrintDialog>

#include <QTcpSocket>
#include <QLabel>
#include "SettingsDialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    SettingsDialog *bSettings;
    QLabel *label;

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
};

#endif // MAINWINDOW_H
