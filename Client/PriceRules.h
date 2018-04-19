#ifndef PRICERULES_H
#define PRICERULES_H

#include <QDialog>

namespace Ui {
class PriceRules;
}

class QSqlTableModel;
class DatabaseManager;

class PriceRules : public QDialog
{
    Q_OBJECT

public:
    explicit PriceRules(QWidget *parent = 0);
    ~PriceRules();

private slots:
    void on_checkBox_clicked(bool checked);
    void setCards(const QModelIndex &cur, const QModelIndex &prev);

private:
    Ui::PriceRules *ui;
    QSqlTableModel *priceModel;
    QSqlTableModel *cardsModel;
};

#endif // PRICERULES_H
