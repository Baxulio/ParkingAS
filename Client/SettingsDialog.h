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

    struct PriceSettings {
        int price;
    };

    explicit SettingsDialog(ServerSettings server = ServerSettings{QString("ParkingServer"),QString("ParkingUser"),QString(""),quint32(3306)},
                            PriceSettings price = PriceSettings{int(0)},
                            QWidget *parent = nullptr);
    ~SettingsDialog();

    ServerSettings serverSettings() const {return bServerSettings;}
    void setServerSettings(const ServerSettings server);

    PriceSettings priceSettings() const {return bPriceSettings;}
    void setPriceSettings(const PriceSettings price);
private:
    Ui::SettingsDialog *ui = nullptr;
    ServerSettings bServerSettings;
    PriceSettings bPriceSettings;

private slots:
    void apply();

    void on_price_spin_valueChanged(int arg1);

private:
    void updateSettings();
signals:
    void priceChanged(int value);
};

#endif // SETTINGSDIALOG_H
