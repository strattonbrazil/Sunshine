#ifndef MATERIAL_H
#define MATERIAL_H

#include <QColor>
#include <QStandardItemModel>
#include "attribute_editor.h"

class Material : public Bindable
{
    Q_OBJECT
public:
    // maybe move to a factory class later
    static QList<QString> materialTypes();
    static Material* buildByType(QString type);

    //virtual MaterialAttributesP attributes() { return _attributes; }
    virtual QList<Attribute> glslFragmentConstants();// { return constantAttributes; }
    //Bindable* vertexAttributes() { return _vertexAttributes; }
    virtual QString glslFragmentCode() = 0;
    int assetType() { return AssetType::MATERIAL_ASSET; }

protected:
    //Bindable* constantAttributes;
    //Bindable* _vertexAttributes;
};

class ShaderTreeModel : public QStandardItemModel
{
    Q_OBJECT
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
    QString glslFragmentCode();
};

#endif // MATERIAL_H
