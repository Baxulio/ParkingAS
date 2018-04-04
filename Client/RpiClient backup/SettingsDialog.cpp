#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(SettingsDialog::ServerSettings server,
                               SettingsDialog::DVRSettings dvr,
                               SettingsDialog::WiegandSettings wiegand,
                               QWidget *parent)
    : QDialog(parent),
      ui(new Ui::SettingsDialog),
      bServerSettings(server),
      bDVRSettings(dvr),
      bWiegandSettings(wiegand)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &SettingsDialog::apply);
}

void SettingsDialog::apply(){
    updateSettings();
    hide();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setServerSettings(const SettingsDialog::ServerSettings server)
{
    bServerSettings = server;
    ui->server_hostname_line_edit->setText(bServerSettings.host);
    ui->server_password_line_edit->setText(bServerSettings.password);
    ui->server_port_spin_box->setValue(bServerSettings.port);
    ui->server_login_line_edit->setText(bServerSettings.user);
}

void SettingsDialog::setDvrSettings(const SettingsDialog::DVRSettings dvr)
{
    bDVRSettings=dvr;
    ui->dvr_method_comboBox->setCurrentIndex(bDVRSettings.connectionMethod);
    ui->dvr_hostname_line_edit->setText(bDVRSettings.host);
    ui->dvr_password_line_edit->setText(bDVRSettings.password);
    ui->dvr_port_spin->setValue( bDVRSettings.port);
    ui->dvr_login_line_edit->setText(bDVRSettings.user);
}

void SettingsDialog::setWiegandSettings(const SettingsDialog::WiegandSettings wiegand)
{
    bWiegandSettings=wiegand;
    ui->gpio_0_spin->setValue(bWiegandSettings.gpio_0);
    ui->gpio_1_spin->setValue(bWiegandSettings.gpio_1);
}

void SettingsDialog::updateSettings()
{
    bServerSettings.host = ui->server_hostname_line_edit->text();
    bServerSettings.password = ui->server_password_line_edit->text();
    bServerSettings.port = ui->server_port_spin_box->value();
    bServerSettings.user = ui->server_login_line_edit->text();

    bDVRSettings.connectionMethod = ui->dvr_method_comboBox->currentIndex();
    bDVRSettings.host = ui->dvr_hostname_line_edit->text();
    bDVRSettings.password = ui->dvr_password_line_edit->text();
    bDVRSettings.port = ui->dvr_port_spin->value();
    bDVRSettings.user = ui->dvr_login_line_edit->text();

    bWiegandSettings.gpio_0 = ui->gpio_0_spin->value();
    bWiegandSettings.gpio_1 = ui->gpio_1_spin->value();
}
