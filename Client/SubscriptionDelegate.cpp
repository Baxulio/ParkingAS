#include "SubscriptionDelegate.h"

SubscriptionDelegate::SubscriptionDelegate(QObject *parent):
    QItemDelegate (parent)
{
}

QWidget *SubscriptionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QCheckBox *editor = new QCheckBox(parent);
    connect(editor, &QCheckBox::clicked, [editor](bool b){
        b?editor->setText("Примените изменения для фиксации Абонемента!"):
          editor->setText("Примените изменения для отмены Абонемента!");
    });
    return editor;
}

void SubscriptionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    static_cast<QCheckBox*>(editor)->setChecked(index.model()->data(index, Qt::EditRole).toBool());
}

void SubscriptionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    model->setData(index,static_cast<QCheckBox*>(editor)->isChecked(),Qt::EditRole);
}

void SubscriptionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
