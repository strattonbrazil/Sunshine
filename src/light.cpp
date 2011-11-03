#include "light.h"

PointLight::PointLight()
{
    QString color("{ 'var' : 'lightColor', 'name' : 'Color', 'type' : 'color', 'value' : '#ffffff', 'glslFragmentConstant' : true }");
    QString intensity("{ 'var' : 'lightIntensity', 'name' : 'Intensity', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 1.0, 'glslFragmentConstant' : true }");

    QStringList atts;
    atts << color << intensity;

    addAttributes(atts);

    Attribute position = attributeByName("Position");
    position->setProperty("glslFragmentConstant", "lightPos");
    //renameAttributeVar("position", "lightPos");
}

#define POINTLIGHT_GLSL_FRAGMENT_BEGIN \
    "  vec3 lightDir = normalize(lightPos - worldPos);\n"


QString PointLight::glslFragmentBegin()
{
    return POINTLIGHT_GLSL_FRAGMENT_BEGIN;
}

#define POINTLIGHT_GLSL_FRAGMENT_END \
    ""

QString PointLight::glslFragmentEnd()
{
    return POINTLIGHT_GLSL_FRAGMENT_END;
}

AmbientLight::AmbientLight()
{
    QString color("{ 'var' : 'lightColor', 'name' : 'Color', 'type' : 'color', 'value' : '#ffffff', 'glslFragmentConstant' : true }");
    QString intensity("{ 'var' : 'lightIntensity', 'name' : 'Intensity', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 0.1, 'glslFragmentConstant' : true }");

    QStringList atts;
    atts << color << intensity;

    addAttributes(atts);

    Attribute position = attributeByName("Position");
    removeAttribute(position);
    //QString glslFragmentBegin();
    //QString glslFragmentEnd();
}

QString AmbientLight::glslFragmentBegin()
{
    return "  vec3 lightDir = normal;\n";
}

QString AmbientLight::glslFragmentEnd()
{
    return "// there;\n";
}
