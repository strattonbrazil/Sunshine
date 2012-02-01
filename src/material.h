#ifndef MATERIAL_H
#define MATERIAL_H

#include <QColor>
#include <QStandardItemModel>
#include "attribute_editor.h"

/*
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
*/



class Material : public Bindable
{
    Q_OBJECT
public:
    //static void registerMaterial(QString type);
//    static void registerNode(QString type);
    static Material* buildByType(QString type);
    int assetType() { return AssetType::MATERIAL_ASSET; }
    QString glslFragmentCode();
protected:
    QStringList _inputs;
};

class ScriptMaterial : public Material
{
    Q_OBJECT
public:
    ScriptMaterial(QStringList inputs);
};

class ShaderTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    ShaderTreeModel();
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void addMaterial(QString name, Material* material);
    Qt::DropActions supportedDropActions() const;
    bool removeRows(int row, int count, const QModelIndex &parent);
    static const int NAME_COLUMN = 0;
public slots:
    void contextMenu(const QPoint &p);
};

class PhongNode : public Material
{
public:
    PhongNode();
    QList<Attribute> inputs();// { return _inputs; }
    QList<Attribute> outputs() { return QList<Attribute>(); }
private:
    QList<Attribute> _inputs;
};

/*
class PhongMaterial : public Material
{
public:
    PhongMaterial();
    QString glslFragmentCode();
};
*/

#endif // MATERIAL_H
