#ifndef SUBSCRIPTIONDELEGATE_H
#define SUBSCRIPTIONDELEGATE_H

#include <QCheckBox>
#include <QItemDelegate>

class SubscriptionDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    SubscriptionDelegate(QObject *parent = 0);

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // SUBSCRIPTIONDELEGATE_H
