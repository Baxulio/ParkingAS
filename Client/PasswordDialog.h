#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class PasswordDialog;
}

class PasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordDialog(QWidget *parent = 0, const QString &title = "");
    ~PasswordDialog();
    QString password() const;

private:
    Ui::PasswordDialog *ui;
};

#endif // PASSWORDDIALOG_H
