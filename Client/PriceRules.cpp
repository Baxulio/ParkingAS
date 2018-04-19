#include "PriceRules.h"
#include "ui_PriceRules.h"

#include "DatabaseManager.h"
#include <QSqlTableModel>

PriceRules::PriceRules(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PriceRules),
    priceModel(new QSqlTableModel(this)),
    cardsModel(new QSqlTableModel(this))
{
    ui->setupUi(this);

    priceModel->setTable("Price");
    cardsModel->setTable("Cards");

    ui->price_table->setModel(priceModel);
    ui->code_table->setModel(cardsModel);

    connect(ui->price_table, &QTableView::clicked, this, &PriceRules::setCards);
}

PriceRules::~PriceRules()
{
    delete ui;
}

void PriceRules::on_checkBox_clicked(bool checked)
{
    ui->formula_form->setEnabled(checked);
    if(checked)
        priceModel->select();
}

void PriceRules::setCards(const QModelIndex &indx)
{
    cardsModel->setFilter(QString("WHERE id=%1").arg(indx.internalId()));
    cardsModel->select();

}
