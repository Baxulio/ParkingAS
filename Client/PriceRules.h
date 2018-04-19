#ifndef PRICERULES_H
#define PRICERULES_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class PriceRules;
}

class DatabaseManager;
class QAbstractButton;

class CustomModel: public QSqlTableModel
{
    Q_OBJECT
public:
    explicit CustomModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase())
        : QSqlTableModel(parent, db) {}

    QVariant data(const QModelIndex &idx, int role) const override
    {
        if (role == Qt::BackgroundRole && isDirty(idx))
            return QBrush(QColor(Qt::yellow));
        return QSqlTableModel::data(idx, role);
    }
};

class PriceRules : public QDialog
{
    Q_OBJECT

public:
    explicit PriceRules(QWidget *parent = 0);
    ~PriceRules();

private slots:
    void setCards(const QModelIndex &indx);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_addPrice_but_clicked();

    void on_remPrice_but_clicked();

    void on_addCard_but_clicked();

    void on_remCard_but_clicked();

private:
    Ui::PriceRules *ui;
    QSqlTableModel *priceModel;
    QSqlTableModel *cardsModel;
};

#endif // PRICERULES_H
