#include "PasswordDialog.h"
#include "ui_PasswordDialog.h"

PasswordDialog::PasswordDialog(QWidget *parent, const QString &title) :
    QDialog(parent),
    ui(new Ui::PasswordDialog)
{
    ui->setupUi(this);
    ui->label->setText(title);
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

QString PasswordDialog::password() const
{
    return ui->lineEdit->text();
}
