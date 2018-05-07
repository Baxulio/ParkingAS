#include "PriceRules.h"
#include "ui_PriceRules.h"

#include "DatabaseManager.h"
#include <QAbstractButton>
#include "SubscriptionDelegate.h"

PriceRules::PriceRules(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PriceRules),
    priceModel(new CustomModel(this)),
    cardsModel(new CustomModel(this))
{
    ui->setupUi(this);

    priceModel->setTable("Price");
    cardsModel->setTable("Cards");

    SubscriptionDelegate *delegate = new SubscriptionDelegate(ui->code_table);
    ui->code_table->setItemDelegateForColumn(3,delegate);
    ui->code_table->setItemDelegateForColumn(4,delegate);

    priceModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    cardsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    priceModel->setHeaderData(1, Qt::Horizontal, "Тип автомобиля", Qt::DisplayRole);
    priceModel->setHeaderData(2, Qt::Horizontal, "Формула цены", Qt::DisplayRole);
    cardsModel->setHeaderData(2, Qt::Horizontal, "Код карты", Qt::DisplayRole);
    cardsModel->setHeaderData(3, Qt::Horizontal, "Абонемент [1:Фиксирован; 0:Отменен]", Qt::DisplayRole);
    cardsModel->setHeaderData(4, Qt::Horizontal, "Грядущий месяц [1:Фиксирован; 0:Отменен]", Qt::DisplayRole);

    ui->price_table->setModel(priceModel);
    ui->price_table->hideColumn(0);

    ui->code_table->setModel(cardsModel);
    ui->code_table->hideColumn(0);
    ui->code_table->hideColumn(1);

    connect(ui->price_table, &QTableView::clicked, this, &PriceRules::setCards);
    priceModel->select();
}

PriceRules::~PriceRules()
{
    delete ui;
}

void PriceRules::setCards(const QModelIndex &indx)
{
    if(priceModel->data(priceModel->index(indx.row(),1)).toString()=="Другое"){
        ui->code_table->setEnabled(false);
        return;
    }
    ui->code_table->setEnabled(true);
    cardsModel->setFilter(QString("priceId=%1").arg(priceModel->data(priceModel->index(indx.row(),0)).toInt()));
    cardsModel->select();
}

void PriceRules::on_buttonBox_clicked(QAbstractButton *button)
{
    switch(ui->buttonBox->buttonRole(button)){
    case QDialogButtonBox::ApplyRole :
        cardsModel->submitAll();
        priceModel->submitAll();
        break;
    case QDialogButtonBox::AcceptRole :
        cardsModel->submitAll();
        priceModel->submitAll();
        close();
        break;
    case QDialogButtonBox::ResetRole :
        //cardsModel->revertAll();
        priceModel->select();
        break;
    default:
        close();
    }

}

void PriceRules::on_addPrice_but_clicked()
{
    if(!priceModel)
        return;

    QModelIndex insertIndex = ui->price_table->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    priceModel->insertRow(row);
    insertIndex = priceModel->index(row, 1);
    ui->price_table->setCurrentIndex(insertIndex);
    ui->price_table->edit(insertIndex);
}

void PriceRules::on_remPrice_but_clicked()
{
    if(!priceModel)
        return;

    QModelIndexList currentSelection = ui->price_table->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        //if (priceModel.data(priceModel->index(currentSelection.at(i).row(),1)).toString()=="Другое")
        if(ui->code_table->isEnabled())
            priceModel->removeRow(currentSelection.at(i).row());
    }

}

void PriceRules::on_addCard_but_clicked()
{
    if(!cardsModel)
        return;

    QModelIndex insertIndex = ui->code_table->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();

    int priceId = priceModel->data(priceModel->index(ui->price_table->currentIndex().row(),0)).toInt();

    cardsModel->insertRow(row);
    insertIndex = cardsModel->index(row, 1);
    cardsModel->setData(insertIndex,priceId);
    ui->code_table->setCurrentIndex(insertIndex);
    ui->code_table->edit(insertIndex);
}

void PriceRules::on_remCard_but_clicked()
{
    if(!cardsModel)
        return;
    QModelIndexList currentSelection = ui->code_table->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0)
            continue;
        cardsModel->removeRow(currentSelection.at(i).row());
    }
}
