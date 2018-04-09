// myclient.h

#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QThreadPool>
#include "mytask.h"
#include "netsdk.h"
#include <QDateTime>

class MyClient : public QObject
{
    Q_OBJECT
public:
    explicit MyClient(QObject *parent = 0);
    ~MyClient();

    void setSocket(qintptr Descriptor);


    void CALLBACK fDisConnectBackCallFunc(LONG lLoginID, char *pchDVRIP,
                                LONG nDVRPort, DWORD dwUser);

signals:

public slots:
    void connected();
    void disconnected();
    void readyRead();

    // make the server fully ascynchronous
    // by doing time consuming task
    void TaskResult(int Number,
                    QDateTime in_time=QDateTime(),
                    quint8 in_number=0,
                    QDateTime out_time=QDateTime(),
                    double price=0);

private:
    QTcpSocket *socket;
    QString dvrip;    //IP of DVR
    quint8 bareerNo;  //bareer Number
    bool bareerMode;  //ENTER or EXIT
    long loginId;
private:
    bool setDVR();
};

#endif // MYCLIENT_H
