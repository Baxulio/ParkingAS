// mytask.h

#ifndef MYTASK_H
#define MYTASK_H

#include <QRunnable>
#include <QObject>
#include <QRunnable>

#include <QDateTime>

// Q_OBJECT missing in the original file generated by class wizard.
// because we set this class with base class QRunnable
// with no inheritance in the class wizard
// We do not have this. So, we cannot use signal/slot
// But we need them.
// Thus, we should use multiple inheritance: QObject inserted here

class MyTask : public QObject, public QRunnable
{

    Q_OBJECT
public:

    MyTask(quint32 wiegand, QString dvrip, quint8 bareerNo, bool bareerMode, long loginId);

signals:
    // notify to the main thread when we're done
    void Result(int Number,
                QDateTime in_time=QDateTime(),
                quint8 in_number=0,
                QDateTime out_time=QDateTime(),
                double price=0);

protected:
    void run();

private:
    quint32 bWiegand;
    QString bDvrip;
    quint8 bBareerNo;
    bool bBareerMode;
    long bLoginId;

    char *cFileName;
private:
    bool snapshot();
    double calculate_formula(const QString &formula, const quint64 &secs);
};

#endif // MYTASK_H
