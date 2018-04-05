#include "ProxyModel.h"

ProxyModel::ProxyModel(QObject *parent):
    QSortFilterProxyModel (parent)
{

}

bool ProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
}

bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
}
