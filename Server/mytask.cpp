// mytask.cpp

#include "mytask.h"
#include <QDebug>

#include <QSqlQuery>
#include <QSqlError>

#include <netsdk.h>

#include "../core.h"
#include <QStandardPaths>

// When the thread pool kicks up
// it's going to hit this run, and it's going to do this time consuming task.
// After it's done, we're going to send the results back to our main thread.
// This runs in the separate thread, and we do not have any control over this thread,
// but Qt does.
// This may just stay in the queue for several ms depending on how busy the server is.

MyTask::MyTask(quint32 wiegand, QString dvrip, quint8 bareerNo, bool bareerMode, long loginId):
    bWiegand(wiegand),
    bDvrip(dvrip),
    bBareerNo(bareerNo),
    bBareerMode(bareerMode),
    bLoginId(loginId)
{
    qDebug() << "MyTask()";
}

void MyTask::run()
{
    QSqlQuery query;
    query.exec(QString("SELECT `Active`.`in_time`,\
                       `Active`.`in_number`,\
                       FROM `Parking`.`Active`;\
               WHERE `Active`.`rf_id`=%1").arg(bWiegand));
            if(!query.exec()){
            qDebug()<<query.lastError().driverText();
            return;
}
            query.next();

            ////ENTER
            if(bBareerMode){
        if(query.isValid()){
            emit Result(Replies::WIEGAND_ALREADY_REGISTERED,
                        query.value("in_time").toDateTime(),
                        query.value("in_number").toUInt());
            return;
        }

        if(!snapshot()){
            emit Result(Replies::SNAPSHOT_FAIL);
            return;
        }

        query.prepare("CALL `Parking`.`RegisterCar`(:b_rf_id, :b_in_number,:b_img);");
        query.bindValue(":b_rf_id",bWiegand);
        query.bindValue(":b_in_number", bBareerNo);
        query.bindValue(":b_img", cFileName);

        if(!query.exec()){
            qDebug()<<query.lastError().driverText();
            return;
        }

        emit Result(Replies::WIEGAND_REGISTERED);
    }

    /////EXIT
    else {
        if(!query.isValid()){
            emit Result(Replies::WIEGAND_NOT_REGISTERED);
            return;
        }
        if(!snapshot()){
            emit Result(Replies::SNAPSHOT_FAIL);
            return;
        }
        query.prepare("CALL `Parking`.`MoveToHistory`(:b_rf_id, :b_out_number, :b_img_out);");
        query.bindValue(":b_rf_id",bWiegand);
        query.bindValue(":b_out_number", bBareerNo);
        query.bindValue(":b_img_out", cFileName);

        if(!query.exec()){
            qDebug()<<query.lastError().driverText();
            return;
        }

        query.next();
        emit Result(Replies::WIEGAND_DEACTIVATED,
                    query.value("in_time").toDateTime(),
                    query.value("in_number").toUInt(),
                    query.value("out_time").toDateTime(),
                    query.value("price").toDouble());
    }
}

bool MyTask::snapshot()
{
    QDateTime time = QDateTime::currentDateTime();

    QString fileName = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)+
            QString("/Parking/%1%2%3_%4%5%6_%7.bmp")
            .arg(time.date().year())
            .arg(time.date().month())
            .arg(time.date().day())
            .arg(time.time().hour())
            .arg(time.time().minute())
            .arg(time.time().second())
            .arg(bBareerNo);

    QByteArray ba = fileName.toLatin1();
    cFileName = ba.data();

    return H264_DVR_CatchPic(bLoginId,0,cFileName);
}
