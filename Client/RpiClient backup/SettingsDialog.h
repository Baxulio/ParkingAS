#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    struct ServerSettings {
        QString host;
        QString user;
        QString password;
        quint32 port;
    };

    struct DVRSettings {
        QString host;
        QString user;
        QString password;
        quint32 port;
        quint8 connectionMethod;
    };
    struct WiegandSettings {
        quint8 gpio_0;
        quint8 gpio_1;
    };
    explicit SettingsDialog(ServerSettings server = ServerSettings{QString("ParkingServer"),QString("ParkingUser"),QString(""),quint32(3306)},
                            DVRSettings dvr = DVRSettings{QString("DVR1"), QString("DVRUser"),QString(""),quint32(34567),quint8(0)},
                            WiegandSettings wiegand = WiegandSettings{quint8(1),quint8(2)},
                            QWidget *parent = nullptr);
    ~SettingsDialog();

    ServerSettings serverSettings() const {return bServerSettings;}
    void setServerSettings(const ServerSettings server);

    DVRSettings dvrSettings() const {return bDVRSettings;}
    void setDvrSettings(const DVRSettings dvr);

    WiegandSettings wiegandSettings() const {return bWiegandSettings;}
    void setWiegandSettings(const WiegandSettings wiegand);

private:
    Ui::SettingsDialog *ui = nullptr;
    ServerSettings bServerSettings;
    DVRSettings bDVRSettings;
    WiegandSettings bWiegandSettings;

private slots:
    void apply();

private:
    void updateSettings();
};

#endif // SETTINGSDIALOG_H
