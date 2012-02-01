#include "material.h"
#include "sunshine.h"

/*
QList<QString> Material::materialTypes() {
    QList<QString> materialTypes;
    materialTypes << "Phong";
    return materialTypes;
}
*/

Material* Material::buildByType(QString type) {

}

#define PHONGMATERIAL_GLSL_FRAGMENT_CODE \
    "  vec3 diffuseColor = vec3(1,0,0);\n" \
    "  gl_FragColor = vec4(diffuseColor*lightColor*lightIntensity*clamp(dot(normal,lightDir),0,1),1);\n" \
    "//  gl_FragColor.a = max(gl_FragColor.x, max(gl_FragColor.y, gl_FragColor.z));\n"

QString Material::glslFragmentCode()
{
    // handle diffuse color by getting all sub diffuses and layering them here
    //

    // handle

    return PHONGMATERIAL_GLSL_FRAGMENT_CODE;
}

ScriptMaterial::ScriptMaterial(QStringList inputs)
{
    _inputs = inputs;
}

ShaderTreeModel::ShaderTreeModel()
{
    QStringList headers;
    headers << "Material" << "Component" << "";
    setHorizontalHeaderLabels(headers);
}

void ShaderTreeModel::contextMenu(const QPoint &p)
{

}

Qt::ItemFlags ShaderTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        //std::cout << index.column() << std::endl;
        //if (index.column() == 0)
        //    return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
        //else
            return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
    }

    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);
    defaultFlags |= Qt::ItemIsDropEnabled;

    // only material editable
    if (index.column() == NAME_COLUMN)
        defaultFlags |= Qt::ItemIsEditable;
    else
        defaultFlags ^= Qt::ItemIsEditable;

    return defaultFlags;
}

Qt::DropActions ShaderTreeModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

bool ShaderTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return FALSE;
    beginRemoveRows(parent, row, row+count-1);
    for (int i = 0; i < count; ++i) {
        QList<QStandardItem*> items = this->takeRow(row);
        foreach (QStandardItem* item, items) {
            delete item;
        }
    }
    endRemoveRows();
    return TRUE;
}

void ShaderTreeModel::addMaterial(QString name, Material* material)
{
    QList<QStandardItem*> items;
    items << new QStandardItem(name);
    items << new QStandardItem("-");
    items << new QStandardItem("-");

    // find the selected material and add it underneath
    QStandardItem* item = this->itemFromIndex(SunshineUi::selectedMaterialIndex());
    if (item != 0) {
        item->appendRow(items);
        SunshineUi::expandMaterialIndex(SunshineUi::selectedMaterialIndex(), true);
    }
    else
        this->appendRow(items);
}

/*
QList<Attribute> Material::glslFragmentConstants()
{
    QList<Attribute> constantAttributes;
    foreach(Attribute att, _attributes) {
        if (att->property("glslFragmentConstant").isValid())
            constantAttributes << att;
    }

    return constantAttributes;
}
*/

/*
Shader* Shader::buildByType(QString type)
{
    if (type == "Phong")
        return new PhongNode();
    else
        return 0;
}
*/

PhongNode::PhongNode()
{
    QStringList atts;
    atts << "{ 'var' : 'diffuseColor', 'name' : 'Diffuse Color', 'type' : 'color', 'value' : '#ff00af', 'glslFragmentConstant' : true, 'shaderType' : 'input'  }";
    atts << "{ 'var' : 'diffuse', 'name' : 'Diffuse', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 1.0, 'shaderType' : 'input' }";
    atts << "{ 'var' : 'specular', 'name' : 'Specular Power', 'type' : 'float', 'min' : 2.5, 'max' : 1000.0, 'value' : 5.0, 'shaderType' : 'input' }";

    addAttributes(atts);
}

QList<Attribute> PhongNode::inputs()
{
    QList<Attribute> inputs;
    for (int i = 0; i < attributeCount(); i++) {
        Attribute attr = this->at(i);
        if (attr->property("shaderType").isValid()) {
            if (attr->property("shaderType").toString() == "input")
                inputs.append(attr);
        }
    }
    return inputs;
}

/*
PhongMaterial::PhongMaterial()
{
    QString diffuseColor("{ 'var' : 'diffuseColor', 'name' : 'Diffuse Color', 'type' : 'color', 'value' : '#ff00af', 'glslFragmentConstant' : true  }");
    QString diffuse("{ 'var' : 'diffuse', 'name' : 'Diffuse', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 1.0 }");
    QString specular("{ 'var' : 'specular', 'name' : 'Specular Power', 'type' : 'float', 'min' : 2.5, 'max' : 1000.0, 'value' : 5.0 }");
    QString reflectivity("{ 'var' : 'reflectivity', 'name' : 'Reflectivity', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 0.0 }");

    QStringList atts;
    atts << diffuseColor << diffuse << specular << reflectivity;

    addAttributes(atts);
}

#define PHONGMATERIAL_GLSL_FRAGMENT_CODE \
    "  gl_FragColor = vec4(diffuseColor*lightColor*lightIntensity*clamp(dot(normal,lightDir),0,1),1);\n" \
    "//  gl_FragColor.a = max(gl_FragColor.x, max(gl_FragColor.y, gl_FragColor.z));\n"

QString PhongMaterial::glslFragmentCode()
{
    return PHONGMATERIAL_GLSL_FRAGMENT_CODE;
}
*/
