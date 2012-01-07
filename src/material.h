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

class Shader;

class ShaderConnection
{
public:
    Shader* fromNode;
    QString fromAttribute;
    Shader* toNode;
    QString toAttribute;
};

inline bool operator==(const ShaderConnection &c1, const ShaderConnection &c2)
{
    return c1.fromNode == c2.fromNode &&
            c1.fromAttribute == c2.fromAttribute &&
            c1.toNode == c2.toNode &&
            c1.toAttribute == c2.toAttribute;
}

inline uint qHash(const ShaderConnection &key)
{
    return qHash(key.fromNode) + qHash(key.toNode);
}

class Shader : public Bindable
{
    Q_OBJECT
public:
    Shader() : zOrder(-1) {}
    Shader* buildByType(QString type);
    virtual QList<Attribute> inputs() = 0;
    virtual QList<Attribute> outputs() = 0;
    QPoint position() { return workspacePos; }
    QList<ShaderConnection> nodeLinks;
    QPoint workspacePos;
    QSize nodeSize;
    int zOrder;
};

class Material : public Shader
{
    Q_OBJECT
public:
    static Material* buildByType(QString type);
    int assetType() { return AssetType::MATERIAL_ASSET; }
};



class ShaderTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    ShaderTreeModel();
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void addMaterial(QString name, Material* material);
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
