#include "myclient.h"

#include <QDataStream>

#include "Core.h"

#include <QSqlQuery>
#include <QSqlError>

//#include <QDebug>

MyClient::MyClient(QObject *parent) :
    QObject(parent),
    dvrip("")
{
    QThreadPool::globalInstance()->setMaxThreadCount(10);

    H264_DVR_Init(
                (fDisConnect)&MyClient::fDisConnectBackCallFunc,
                1);
}

MyClient::~MyClient()
{
    H264_DVR_Cleanup();
}

void MyClient::setSocket(qintptr descriptor)
{
    // make a new socket
    socket = new QTcpSocket(this);
    //qDebug() << "A new socket created!";

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    socket->setSocketDescriptor(descriptor);

    //qDebug() << " Client connected at " << descriptor;
}

void MyClient::fDisConnectBackCallFunc(long lLoginID, char *pchDVRIP, long nDVRPort, unsigned long dwUser)
{
    H264_DVR_Logout(lLoginID);
}

// asynchronous - runs separately from the thread we created
void MyClient::connected()
{
    //qDebug() << "Client connected event";
}

// asynchronous
void MyClient::disconnected()
{
    H264_DVR_Logout(loginId);
    //qDebug() << "Client disconnected";
}

// Our main thread of execution
// This happening via main thread
// Our code running in our current thread not in another QThread
// That's why we're getting the thread from the pool

void MyClient::readyRead()
{
    //qDebug() << "MyClient::readyRead()";

    QByteArray arr=socket->readAll();
    QDataStream in(&arr,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_5);

    in.startTransaction();

    bool mode;
    in>>mode;

    if(mode){ //SETUP_MODE
        in>>bareerNo;
        in>>bareerMode;
        in>>dvrip;

        if(!in.commitTransaction())
            return;
        if(!setDVR()){
            emit TaskResult(Replies::DVR_ERROR);
        }
        else emit TaskResult(Replies::SET_UP);

        return;
    }

    quint32 wiegand;
    in>>wiegand;

    if(!in.commitTransaction())
        return;
    QSqlQuery query;

    // Time consumer
    MyTask *mytask = new MyTask(wiegand,
                                dvrip,
                                bareerNo,
                                bareerMode,
                                loginId);
    mytask->setAutoDelete(true);

    // using queued connection
    connect(mytask, &MyTask::Result,
            this, &MyClient::TaskResult, Qt::QueuedConnection);

//    qDebug() << "Starting a new task using a thread from the QThreadPool";

    // QThreadPool::globalInstance() returns global QThreadPool instance
    QThreadPool::globalInstance()->start(mytask);
}

void MyClient::TaskResult(int Number, QDateTime in_time, quint8 in_number, QDateTime out_time, double price)
{
    QByteArray Buffer;
    QDataStream out(&Buffer,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);

    out<<Number;

    if(Number == Replies::WIEGAND_DEACTIVATED || Number == Replies::WIEGAND_ALREADY_DEACTIVATED){
        out<<in_time;
        out<<in_number;
        out<<out_time;
        out<<price;
    }
    else if(Number == Replies::WIEGAND_ALREADY_REGISTERED){
        out<<in_time;
        out<<in_number;
    }
    else if(Number == Replies::WIEGAND_IS_MONTHLY)
    {
        out<<in_time;
        out<<in_number;
        out<<out_time;
    }

    socket->write(Buffer);
}


bool MyClient::setDVR()
{
    //qDebug()<<mode<<" "<<bareerNo<<" "<<bareerMode<<" "<<dvrip<<"\n";
    H264_DVR_DEVICEINFO OutDev;
    int nEroor = 0;
    H264_DVR_SetConnectTime(3000, 1);
    //		H264_DVR_SetLocalBindAddress("10.2.55.25");

    char *charDVRip = dvrip.toLatin1().data();

//    qDebug()<<charDVRip;

    loginId = H264_DVR_Login(charDVRip, 34567, "admin", "",
                             &OutDev, &nEroor,SocketStyle::TCPSOCKET);

    if ( loginId <= 0 )
    {
        int nErr = H264_DVR_GetLastError();

        if(nErr == H264_DVR_PASSWORD_NOT_VALID)
        {
//            qDebug()<<"Error: password error";
        }
        else
        {
//            qDebug()<<"Error: not found";
        }
        return false;
    }
//    qDebug()<<"Camera Connected";
    return true;
}

// After a task performed a time consuming task,
// we grab the result here, and send it to client
