#include "ProxyModel.h"
#include <QDebug>
ProxyModel::ProxyModel(QObject *parent):
    QSortFilterProxyModel (parent),
    rf_id(0),in(0),out(0)
{
}

void ProxyModel::setFilterRf_Id(const quint32 &id)
{
    rf_id=id;
    invalidateFilter();
}

void ProxyModel::setFilterIn(const quint8 &in)
{
    this->in=in;
    invalidateFilter();
}

void ProxyModel::setFilterOut(const quint8 &out)
{
    this->out=out;
    invalidateFilter();
}

void ProxyModel::setFilterIn_Time_From(const QDateTime &time)
{
    in_time_from=time;
    invalidateFilter();
}

void ProxyModel::setFilterIn_Time_To(const QDateTime &time)
{
    in_time_to=time;
    invalidateFilter();
}

void ProxyModel::setFilterOut_Time_From(const QDateTime &time)
{
    out_time_from=time;
    invalidateFilter();
}

void ProxyModel::setFilterOut_Time_To(const QDateTime &time)
{
    out_time_to=time;
    invalidateFilter();
}

void ProxyModel::setHeaders()
{
    for(int col=0; col<this->columnCount(); col++) {
        QString header = this->headerData(col,Qt::Horizontal,Qt::DisplayRole).toString();

        header=="rf_id"?setHeaderData(col,Qt::Horizontal,"Код карты"):
        header=="in_time"?setHeaderData(col,Qt::Horizontal,"Время въезда"):
        header=="in_number"?setHeaderData(col,Qt::Horizontal,"Въезд №"):
        header=="out_time"?setHeaderData(col,Qt::Horizontal,"Время выезда"):
        header=="out_number"?setHeaderData(col,Qt::Horizontal,"Выезд №"):
        header=="price"?setHeaderData(col,Qt::Horizontal,"Цена"):NULL;
    }
}

bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(rf_id && sourceModel()->data(sourceModel()->index(source_row, 1, source_parent)).toInt()!=rf_id)
        return false;

    if(in && sourceModel()->data(sourceModel()->index(source_row, in_col, source_parent)).toInt()!=in)
        return false;

    if(out && sourceModel()->data(sourceModel()->index(source_row, 5, source_parent)).toInt()!=out)
        return false;

    if(!dateInRange(sourceModel()->data(sourceModel()->index(source_row, 2, source_parent)).toDateTime(),1))
        return false;

    if(!dateInRange(sourceModel()->data(sourceModel()->index(source_row, 3, source_parent)).toDateTime(),0))
        return false;

    return true;
}

bool ProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    QString header = sourceModel()->headerData(source_column,Qt::Horizontal,Qt::DisplayRole).toString();

    if(header == "id" || header=="img" || header=="img_out")
        return false;
    return true;
}

bool ProxyModel::dateInRange(const QDateTime &dateTime, bool mode) const
{
    if(mode){
        return (!in_time_from.isValid() || dateTime>=in_time_from)
                && (!in_time_to.isValid() || dateTime<=in_time_to);
    }
    return (!out_time_from.isValid() || dateTime>=out_time_from)
            && (!out_time_to.isValid() || dateTime<=out_time_to);
}
