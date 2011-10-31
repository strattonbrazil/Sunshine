#ifndef MATERIAL_H
#define MATERIAL_H

#include <QSharedPointer>
#include <QColor>
#include <QStandardItemModel>
#include "attribute_editor.h"

class Material
{
public:
    //virtual MaterialAttributesP attributes() { return _attributes; }
    BindableP constantAttributes() { return _constantAttributes; }
    BindableP vertexAttributes() { return _vertexAttributes; }
protected:
    BindableP _constantAttributes;
    BindableP _vertexAttributes;
};
typedef QSharedPointer<Material> MaterialP;

class ShaderTreeModel : public QStandardItemModel
{
public:
    ShaderTreeModel();
    Qt::ItemFlags flags(const QModelIndex &index) const;

    //ShaderTreeModel(QObject* parent) : QStardardItemModel(parent) {}
    /*
    ShaderTreeModel() : QStardardItemModel() {}
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    */
};



class PhongMaterial : public Material
{
public:
    PhongMaterial();
};

#endif // MATERIAL_H
