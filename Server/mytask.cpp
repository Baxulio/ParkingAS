// mytask.cpp

#include "mytask.h"
#include <QDebug>

#include <QSqlQuery>
#include <QSqlError>

#include <netsdk.h>

#include "Core.h"
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
    cFileName="/home/bahman/Pictures/Parking/201846_21223.jpeg";
}

void MyTask::run()
{
    QSqlQuery query;
    query.prepare("SELECT `Active`.`id`,`Active`.`in_time`,`Active`.`in_number`,`Active`.`img` "
                  "FROM `Parking`.`Active` WHERE `Active`.`rf_id`=:rf_id;");
    query.bindValue(":rf_id",bWiegand);
    if(!query.exec()){
        qDebug()<<query.lastError().databaseText();
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

//        if(!snapshot()){
//            emit Result(Replies::SNAPSHOT_FAIL);
//            return;
//        }

        query.prepare("INSERT INTO `Parking`.`Active`(`rf_id`, `in_time`, `in_number`, `img`) "
                      "VALUES(:b_rf_id,:b_in_time,:b_in_number,:b_img);");
        query.bindValue(":b_rf_id",bWiegand);
        query.bindValue(":b_in_time",QDateTime::currentDateTime());
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
            if(!query.exec("SELECT in_time, in_number, out_time, price "
                           "FROM History "
                           "WHERE out_time >= DATE_SUB(NOW() , INTERVAL 1 MINUTE)")){
                qDebug()<<query.lastError().driverText();
                return;
            }
            query.next();
            if(!query.isValid()){
                emit Result(Replies::WIEGAND_NOT_REGISTERED);
                return;
            }
            emit Result(Replies::WIEGAND_ALREADY_DEACTIVATED,
                        query.value("in_time").toDateTime(),
                        query.value("in_number").toUInt(),
                        query.value("out_time").toDateTime(),
                        query.value("price").toDouble());
            return;
        }
//        if(!snapshot()){
//            emit Result(Replies::SNAPSHOT_FAIL);
//            return;
//        }

        QSqlDatabase::database().transaction();
        //////////////////////////////
        /// MOVE TO HISTORY //////////
        /// //////////////////////////
        ///
        int b_id = query.value("id").toInt();
        QDateTime b_in_time = query.value("in_time").toDateTime();
        quint8 b_in_number = query.value("in_number").toUInt();
        QString b_img = query.value("img").toString();

        query.exec("SELECT `price_per_hour` FROM `Parking`.`Price`");
        query.next();

        double price = query.value("price").toDouble();
        quint64 secs = b_in_time.secsTo(QDateTime::currentDateTime());
        if(secs/60<=10)
            price=0;
        else {
            secs/=3600;
            price += secs<=5?(secs-1)*1000:
                             4000;
        }
        query.prepare("INSERT INTO `Parking`.`History`(`rf_id`, `in_time`, `out_time`, `in_number`, `out_number`, `price`, `img`, `img_out`) "
                      "VALUES(:b_rf_id, :b_in_time, :b_out_time, :b_in_number, :b_out_number, :b_price, :b_img, :b_img_out);");
        query.bindValue(":b_rf_id",bWiegand);
        query.bindValue(":b_in_time",b_in_time);
        query.bindValue(":b_out_time",QDateTime::currentDateTime());
        query.bindValue(":b_in_number",b_in_number);
        query.bindValue(":b_out_number", bBareerNo);
        query.bindValue(":b_img",b_img);
        query.bindValue(":b_img_out", cFileName);
        query.bindValue(":b_price", price);
        query.exec();

        int b_last_insert_id=query.lastInsertId().toInt();

        query.prepare("DELETE FROM `Parking`.`Active` WHERE `id`=:b_id;");
        query.bindValue(":b_id",b_id);
        query.exec();
        ///
        ///////////////////////////////
        /// END MOVE TO HISTORY ///////
        /// ///////////////////////////
        if(!QSqlDatabase::database().commit()){
            emit Result(Replies::INVALID);
            qDebug()<<QSqlDatabase::database().lastError().driverText();
            return;
        }
        query.prepare("SELECT `in_time`, `out_time`, `in_number`, `price`"
                      "FROM `Parking`.`History`"
                      "WHERE `id` = :b_last_insert_id;");
        query.bindValue(":b_last_insert_id",b_last_insert_id);
        query.exec();
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
            QString("/Parking/%1%2%3_%4%5%6_%7.jpeg")
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
