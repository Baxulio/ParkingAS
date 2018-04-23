// mytask.cpp

#include "mytask.h"
#include <QDebug>

#include <QSqlQuery>
#include <QSqlError>

#include <netsdk.h>

#include "Core.h"
//#include <QStandardPaths>

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
        emit Result(Replies::INVALID);
        qDebug()<<query.lastError().text();
        return;
    }
    query.next();

    QDateTime b_out_time = QDateTime::currentDateTime();

    ////ENTER
    if(bBareerMode){
        if(query.isValid()){
            emit Result(Replies::WIEGAND_ALREADY_REGISTERED,
                        query.value("in_time").toDateTime(),
                        query.value("in_number").toUInt());
            return;
        }
//        if(!snapshot(b_out_time)){
//            emit Result(Replies::SNAPSHOT_FAIL);
//            return;
//        }

        query.prepare("INSERT INTO `Parking`.`Active`(`rf_id`, `in_time`, `in_number`, `img`) "
                      "VALUES(:b_rf_id,:b_in_time,:b_in_number,:b_img);");
        query.bindValue(":b_rf_id",bWiegand);
        query.bindValue(":b_in_time",b_out_time);
        query.bindValue(":b_in_number", bBareerNo);
        query.bindValue(":b_img", cFileName);

        if(!query.exec()){
            emit Result(Replies::INVALID);
            qDebug()<<query.lastError().text();
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
                emit Result(Replies::INVALID);
                qDebug()<<query.lastError().text();
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

//        if(!snapshot(b_out_time)){
//            emit Result(Replies::SNAPSHOT_FAIL);
//            return;
//        }

        int b_id = query.value("id").toInt();
        QDateTime b_in_time = query.value("in_time").toDateTime();
        quint8 b_in_number = query.value("in_number").toUInt();
        QString b_img = query.value("img").toString();

        query.prepare("SELECT Price.price_formula, Cards.last_period FROM Price INNER JOIN Cards ON Price.id = Cards.id WHERE Cards.code = :b_wiegand");
        query.bindValue(":b_wiegand", bWiegand);
        query.exec();
        query.next();
        if(!query.isValid()){
            query.exec("SELECT price_formula FROM Price WHERE car_type = 'Другое'");
            query.next();
        }
        else if(query.value("last_period").isNull()){
            emit Result(Replies::WIEGAND_IS_MONTHLY,b_in_time,b_in_number,b_out_time);
            return;
        }
        double price = calculate_formula(query.value("price_formula").toString(),
                                         b_in_time.secsTo(b_out_time));

        QSqlDatabase::database().transaction();
        //////////////////////////////
        /// MOVE TO HISTORY //////////
        /// //////////////////////////
        ///
        query.prepare("INSERT INTO `Parking`.`History`(`rf_id`, `in_time`, `out_time`, `in_number`, `out_number`, `price`, `img`, `img_out`) "
                      "VALUES(:b_rf_id, :b_in_time, :b_out_time, :b_in_number, :b_out_number, :b_price, :b_img, :b_img_out);");
        query.bindValue(":b_rf_id",bWiegand);
        query.bindValue(":b_in_time",b_in_time);
        query.bindValue(":b_out_time",b_out_time);
        query.bindValue(":b_in_number",b_in_number);
        query.bindValue(":b_out_number", bBareerNo);
        query.bindValue(":b_img",b_img);
        query.bindValue(":b_img_out", cFileName);
        query.bindValue(":b_price", price);
        query.exec();

        query.prepare("DELETE FROM `Parking`.`Active` WHERE `id`=:b_id;");
        query.bindValue(":b_id",b_id);
        query.exec();
        ///
        ///////////////////////////////
        /// END MOVE TO HISTORY ///////
        /// ///////////////////////////
        if(!QSqlDatabase::database().commit()){
            emit Result(Replies::INVALID);
            qDebug()<<query.lastError().text();
            return;
        }
        emit Result(Replies::WIEGAND_DEACTIVATED,b_in_time,b_in_number,b_out_time,price);
    }
}

bool MyTask::snapshot(const QDateTime &time)
{
    QString fileName = /*QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)*/
            QString("/home/bahman/Pictures/Parking/%1%2%3_%4%5%6_%7.jpeg")
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

double MyTask::calculate_formula(const QString &formula, const quint64 &secs)
{
    double mins = secs/60;
    if(!formula.contains(':'))
        return mins*formula.toDouble()/60;

    QStringList pair;
    QStringList priceList = formula.split(';',QString::SkipEmptyParts);
    foreach (QString str, priceList) {
        pair = str.split(':',QString::SkipEmptyParts);
        if(mins<=QString(pair[0]).toDouble()){
            return QString(pair[1]).toDouble();
        }
    }
    return QString(pair[1]).toDouble();
}
