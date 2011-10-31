#ifndef ATTRIBUTEEDITOR_H
#define ATTRIBUTEEDITOR_H

#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <iostream>
#include "exceptions.h"
#include "util.h"
#include "bindable.h"

class AttributeEditor : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit AttributeEditor(QWidget *parent = 0);
    void update(BindableP instance);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Attribute attribute(QString name) { return _instance->attributeByName(name); }
    BindableP instance() { return _instance; }
signals:

public slots:
private:
    BindableP _instance;
};

#include <QSpinBox>
#include <QStyledItemDelegate>

class AttributeItemDelegate : public QStyledItemDelegate
{
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // ATTRIBUTEEDITOR_H
