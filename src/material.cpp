#include "material.h"
#include "sunshine.h"

#include <QProcess>

QHash<QString,AqsisMaterialInfo> Material::_aqsisMaterials;
QStringList Material::_materialTypes;

const void REPLACE_LAST_TOKEN(QStringList &atts, QString token, QString replacement)
{
    // last param had no default value
    if (atts.size() > 0 && atts.last().contains(token)) {
        QString withoutToken = atts.last().replace(token, replacement);
        atts.removeLast();
        atts.append(withoutToken);
    }
}

void Material::registerAqsisShader(QString path)
{
    QStringList args;
    args << path;

    QProcess aqsltell;
    aqsltell.start("aqsltell", args);
    aqsltell.waitForFinished();
    QByteArray data = aqsltell.readAllStandardOutput();
    QString output(data);

    QStringList lines = output.split("\n");

    QRegExp parameterP("\"([a-zA-Z]+)\" \"([a-zA-Z ]+)\"");
    QRegExp defaultFloatP("Default value: ([-+]?[0-9]*\.?[0-9]+)");

    QHash<QString,QString> typeToJson;
    typeToJson["parameter uniform float"] = "float";
    typeToJson["parameter uniform color"] = "color";

    bool validSurface = false;
    QStringList atts;
    QString paramName;
    QString paramType;
    foreach(QString line, lines) {
        //std::cout << line << std::endl;
        if (line.contains("Cs"))
            std::cout << line << std::endl;
        if (line.startsWith("surface"))
            validSurface = true;
        if (parameterP.indexIn(line) != -1) {
            //REPLACE_LAST_TOKEN(atts, "DEFAULT_VALUE_TOKEN", "");

            paramName = parameterP.cap(1);
            paramType = parameterP.cap(2);

            if (typeToJson.contains(paramType)) {
                QString att = QString("{ 'var' : '%1', 'name' : '%1', 'type' : '%2', DEFAULT_VALUE_TOKEN}").arg(paramName, typeToJson[paramType]);
                atts << att;
            }
            else {
                std::cerr << "Unsupported type: " << paramType << " on " << paramName << std::endl;
            }
        }
        else if (paramType.compare("parameter uniform float") == 0 && defaultFloatP.indexIn(line) != -1) {
            REPLACE_LAST_TOKEN(atts, "DEFAULT_VALUE_TOKEN", QString("'value' : %1").arg(defaultFloatP.cap(1).toFloat()));
        }

        //tokens = line.split("\n")
    }
    REPLACE_LAST_TOKEN(atts, "DEFAULT_VALUE_TOKEN", "");

    //for (int i = 0; i < atts.count(); i++)
     //   std::cout << atts.at(i) << std::endl;

    QString att = QString("{ 'var' : 'baseColor', 'name' : 'Base Color', 'type' : 'color' }");
    atts.prepend(att);

    if (validSurface) {
        AqsisMaterialInfo info;
        info.path = path;
        info.atts = atts;
        QString shaderName = QFileInfo(path).baseName();
        _aqsisMaterials[shaderName] = info;

        _materialTypes.append(shaderName);
    }


    //return validSurface;
    //std::cout << output << std::endl;
}

Material* Material::buildByType(QString type) {
    if (_aqsisMaterials.contains(type)) {
        return new AqsisMaterial(_aqsisMaterials[type]);
    }
    return 0;
}

AqsisMaterial::AqsisMaterial(AqsisMaterialInfo info)
{
    _info = info;
    addAttributes(info.atts);
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
