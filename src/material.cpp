#include "material.h"
#include "sunshine.h"

QList<QString> Material::materialTypes() {
    QList<QString> materialTypes;
    materialTypes << "Phong";
    return materialTypes;
}

Material* Material::buildByType(QString type) {
    if (type == "Phong")
        return new PhongMaterial();
    else
        return 0;
}

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

QList<Attribute> Material::glslFragmentConstants()
{
    QList<Attribute> constantAttributes;
    foreach(Attribute att, _attributes) {
        if (att->property("glslFragmentConstant").isValid())
            constantAttributes << att;
    }

    return constantAttributes;
}

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
