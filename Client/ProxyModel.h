#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QDateTime>
#include <QSortFilterProxyModel>

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ProxyModel(QObject *parent = 0);

    void setFilterRf_Id(const quint32 &id);
    void setFilterIn(const quint8 &in);
    void setFilterOut(const quint8 &out);
    void setFilterIn_Time_From(const QDateTime &time);
    void setFilterIn_Time_To(const QDateTime &time);
    void setFilterOut_Time_From(const QDateTime &time);
    void setFilterOut_Time_To(const QDateTime &time);
    void setHeaders();

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;
private:
    bool dateInRange(const QDateTime &dateTime, bool mode) const;

    QDateTime in_time_from;
    QDateTime in_time_to;
    QDateTime out_time_from;
    QDateTime out_time_to;

    quint32 rf_id;
    quint8 in;
    quint8 out;
};

#endif // PROXYMODEL_H
