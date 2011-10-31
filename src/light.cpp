#include "light.h"

PointLight::PointLight()
{
    QString color("{ 'var' : 'lightColor', 'name' : 'Color', 'type' : 'color', 'value' : '#ffffff', 'glslFragmentConstant' : true }");
    QString intensity("{ 'var' : 'lightIntensity', 'name' : 'Intensity', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 1.0, 'glslFragmentConstant' : true }");

    QStringList atts;
    atts << color << intensity;

    addAttributes(atts);
}

#define POINTLIGHT_GLSL_FRAGMENT_BEGIN \
    "  vec3 lightDir = normalize(worldPos - lightPos);\n"


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
