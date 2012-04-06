#ifndef MATERIAL_H
#define MATERIAL_H

#include <QColor>
#include <QStandardItemModel>
#include "attribute_editor.h"

struct AqsisMaterialInfo
{
    QString path;
    QStringList atts;
};

class Material : public Bindable
{
    Q_OBJECT
public:
    //static void registerMaterial(QString type);
//    static void registerNode(QString type);
    static void registerAqsisShader(QString path);
    static QStringList materialTypes() { return _materialTypes; }
    static Material* buildByType(QString type);
    int assetType() { return AssetType::MATERIAL_ASSET; }
    QString glslFragmentCode();
protected:
    static QStringList _materialTypes;
    static QHash<QString,AqsisMaterialInfo> _aqsisMaterials;
    QStringList _inputs;
};

class AqsisMaterial : public Material
{
    Q_OBJECT
public:
    AqsisMaterial(AqsisMaterialInfo info);
    QString glslFragmentCode() { return "<not implemented>"; }
    QString path() { return _info.path; }
private:
    AqsisMaterialInfo _info;
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
