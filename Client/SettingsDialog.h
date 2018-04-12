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

    bool isPriceEdited() const {return priceEdited;}
    void setPriceEdited(bool b){priceEdited=b;}

private:
    Ui::SettingsDialog *ui = nullptr;
    ServerSettings bServerSettings;
    PriceSettings bPriceSettings;

    bool priceEdited;

private slots:
    void apply();

private:
    void updateSettings();
};

#endif // SETTINGSDIALOG_H
