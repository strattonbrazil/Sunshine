#include "material.h"
#include "sunshine.h"

ShaderTreeModel::ShaderTreeModel()
{
    QStringList headers;
    headers << "" << "Material" << "Component";
    setHorizontalHeaderLabels(headers);
}

Qt::ItemFlags ShaderTreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);

    // only material editable
    if (index.column() == 1)
        defaultFlags |= Qt::ItemIsEditable;
    else
        defaultFlags ^= Qt::ItemIsEditable;

    return defaultFlags;
}

/*
QModelIndex ShaderTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) // sub-shaders
        return QModelIndex();
    else {
        QString mName = SunshineUi::activeScene()->materials()[row];
        return createIndex(row, column, new QString(mName));
    }
    return QModelIndex();
}

QModelIndex ShaderTreeModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int ShaderTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) // sub-shaders
        return 0;
    else {
        return SunshineUi::activeScene()->materials().size();
    }


}

int ShaderTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return 3;
}

QVariant ShaderTreeModel::data(const QModelIndex &index, int role=Qt::DisplayRole) const
{
    if (role == Qt::DisplayRole) {
        if (!index.parent().isValid()) {
            if (index.column() == 0) {
                return QVariant("frack");
            } else if (index.column() == 1) {
                //QString* name = (QString*)(index.internalPointer());
                return QVariant("whatever");
                //return QVariant(*name);
            } else if (index.column() == 2) {
                return QVariant("ack");
            }
        }
    }
    return QVariant();
}

QVariant ShaderTreeModel::headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole ) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        QList<QString> headers;
        headers << " " << "Name" << "Effect";
        return headers[section];
    }

    return QVariant();
}
*/

PhongMaterial::PhongMaterial()
{
    QString diffuseColor("{ 'var' : 'diffuseColor', 'name' : 'Diffuse Color', 'type' : 'color', 'value' : '#ff0000' }");
    QString diffuse("{ 'var' : 'diffuse', 'name' : 'Diffuse', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 1.0 }");
    QString specular("{ 'var' : 'specular', 'name' : 'Specular Power', 'type' : 'float', 'min' : 2.5, 'max' : 1000.0, 'value' : 5.0 }");
    QString reflectivity("{ 'var' : 'reflectivity', 'name' : 'Reflectivity', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 0.0 }");

    QStringList atts;
    atts << diffuseColor << diffuse << specular << reflectivity;

    Bindable* bindable = new Bindable();
    bindable->addAttributes(atts);
    _constantAttributes = BindableP(bindable);
}

#define PHONGMATERIAL_GLSL_FRAGMENT_CODE \
    "  gl_FragColor = vec4(lightColor*lightIntensity*clamp(dot(normal,lightDir),0,1),1);\n"

QString PhongMaterial::glslFragmentCode()
{
    return PHONGMATERIAL_GLSL_FRAGMENT_CODE;
}
