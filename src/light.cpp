#include "light.h"

QList<Attribute> Light::glslFragmentConstants()
{
    QList<Attribute> constantAttributes;
    foreach(Attribute att, _attributes) {
        if (att->property("glslFragmentConstant").isValid())
            constantAttributes << att;
    }

    return constantAttributes;
}

PointLight::PointLight()
{
    QString color("{ 'var' : 'lightColor', 'name' : 'Color', 'type' : 'color', 'value' : '#ffffff', 'glslFragmentConstant' : true }");
    QString intensity("{ 'var' : 'uniformLightIntensity', 'name' : 'Intensity', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 1.0, 'glslFragmentConstant' : true }");
    QString castShadows("{ 'var' : 'castShadows', 'name' : 'Casts Shadows', 'type' : 'bool', 'value' : true }");

    QStringList atts;
    atts << color << intensity << castShadows;

    addAttributes(atts);

    Attribute position = attributeByName("Position");
    position->setProperty("glslFragmentConstant", "lightPos");
    //renameAttributeVar("position", "lightPos");
}

QList<Attribute> PointLight::glslFragmentConstants()
{
    Attribute depthMapAttr1(new AttributeObject);
    depthMapAttr1->setProperty("var", "depthMapP");
    depthMapAttr1->setProperty("type", "sampler2DShadow");
    depthMapAttr1->setProperty("glslFragmentConstant", "depthMapP");

    Attribute depthMapAttr2(new AttributeObject);
    depthMapAttr2->setProperty("var", "depthMapN");
    depthMapAttr2->setProperty("type", "sampler2DShadow");
    depthMapAttr2->setProperty("glslFragmentConstant", "depthMapN");

    Attribute lightToWorldAttr(new AttributeObject);
    lightToWorldAttr->setProperty("var", "worldToLight");
    lightToWorldAttr->setProperty("type", "mat4");
    lightToWorldAttr->setProperty("glslFragmentConstant", "worldToLight");

    QList<Attribute> atts;
    foreach(Attribute attribute, Light::glslFragmentConstants()) {
        atts << attribute;
    }
    atts << depthMapAttr1 << depthMapAttr2 << lightToWorldAttr;

    return atts;
}

QString PointLight::glslFragmentBegin()
{
    QString lightFragCode;
    lightFragCode += "  vec3 lightDir = normalize(lightPos - worldPos);\n";
    Attribute castShadows = attributeByName("Casts Shadows");
    if (castShadows && castShadows->property("value").isValid()) {
        bool casting = castShadows->property("value").value<bool>();
        if (casting) {
            // add dual-parab mapping here...
            //
            lightFragCode += "  float zScale = zShadowFar;\n";
            lightFragCode += "  float zBias = 0.0;\n";

            // calculate first lookup
            lightFragCode += "  vec4 d0 = vec4(0,0,1,0);\n";
            lightFragCode += "  vec4 P0 = (worldToLight * vec4(worldPos,1.0));\n";
            lightFragCode += "  P0 = P0 / P0.w;\n";
            lightFragCode += "  float alpha = 0.5 + (P0.z/zScale);\n";
            lightFragCode += "  float lengthP0 = length(P0);\n";
            lightFragCode += "  P0 = P0 / lengthP0;\n";
            lightFragCode += "  P0 = P0 + d0;\n";
            lightFragCode += "  P0.x = P0.x / P0.z;\n";
            lightFragCode += "  P0.y = P0.y / P0.z;\n";
            lightFragCode += "  P0.z = (lengthP0-zShadowNear)/(zShadowFar-zShadowNear) + zBias;\n";
            lightFragCode += "  P0.w = 1.0;\n";
            lightFragCode += "  vec3 hemiShadowLookup0 = 0.5 + 0.5*P0.xyz;\n";

            // calculate second lookup
            lightFragCode += "  vec4 d1 = vec4(0,0,-1,0);\n";
            lightFragCode += "  vec4 P1 = (worldToLight * vec4(worldPos,1.0));\n";
            lightFragCode += "  P1 = P1 / P1.w;\n";
            lightFragCode += "  float lengthP1 = length(P1);\n";
            lightFragCode += "  P1 = P1 / lengthP1;\n";
            lightFragCode += "  P1 = P1 + d1;\n";
            lightFragCode += "  P1.x = P1.x / P1.z;\n";
            lightFragCode += "  P1.y = P1.y / P1.z;\n";
            lightFragCode += "  P1.z = (lengthP1-zShadowNear)/(zShadowFar-zShadowNear) + zBias;\n";
            lightFragCode += "  P1.w = 1.0;\n";
            lightFragCode += "  vec3 hemiShadowLookup1 = 0.5 + 0.5*P1.xyz;\n";

            lightFragCode += "  float res = 0.5;\n";
            lightFragCode += "  if (alpha >= 0.5) { // user 1st hemisphere\n";
            lightFragCode += "    res = texture(depthMapP, hemiShadowLookup0);\n";
            lightFragCode += "  } else { // use 2nd hemisphere\n";
            lightFragCode += "    res = texture(depthMapN, hemiShadowLookup1);\n";
            lightFragCode += "  }\n";

            lightFragCode += "  float lightIntensity = uniformLightIntensity * res;\n";
            lightFragCode += "  vec3 lightVec = lightPos - worldPos;\n";
            lightFragCode += "  //lightIntensity = lightIntensity * shadowCube(depthCubeMap, vec4(normalize(lightVec), (length(lightVec)-zShadowNear)/(zShadowFar-zShadowFar))).x;\n";
            lightFragCode += "  //lightIntensity = lightIntensity * shadowCube(depthCubeMap, vec4(vec3(0,1,0), -1000)).x;\n";
        }
    }
    return lightFragCode;
}

QString PointLight::glslFragmentEnd()
{
    QString lightFragCode;
    Attribute castShadows = attributeByName("Casts Shadows");
    if (castShadows && castShadows->property("value").isValid()) {
        bool casting = castShadows->property("value").value<bool>();
        if (casting) {
        }
    }
    return lightFragCode;
}

SpotLight::SpotLight()
{
    QString color("{ 'var' : 'lightColor', 'name' : 'Color', 'type' : 'color', 'value' : '#ffffff', 'glslFragmentConstant' : true }");
    QString intensity("{ 'var' : 'uniformLightIntensity', 'name' : 'Intensity', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 1.0, 'glslFragmentConstant' : true }");
    QString coneAngle("{ 'var' : 'coneAngle', 'name' : 'Cone Angle', 'type' : 'float', 'min' : 1.0, 'max' : 150.0, 'value' : 60.0, 'glslFragmentConstant' : true }");
    QString castShadows("{ 'var' : 'castShadows', 'name' : 'Casts Shadows', 'type' : 'bool', 'value' : true }");
    QString spotDir("{ 'var' : 'spotDir', 'name' : 'Spot Direction', 'type' : 'vector3', 'getter' : 'spotDir', 'glslFragmentConstant' : true }");

    QStringList atts;
    atts << color << intensity << coneAngle << castShadows << spotDir;

    addAttributes(atts);

    Attribute position = attributeByName("Position");
    position->setProperty("glslFragmentConstant", "lightPos");
}

QString SpotLight::glslFragmentBegin()
{
    QString lightFragCode;
    lightFragCode += "  vec3 lightDir = normalize(lightPos - worldPos);\n";
    lightFragCode += "  float spotEffect = dot(spotDir, -lightDir);\n";
    Attribute castShadows = attributeByName("Casts Shadows");
    if (castShadows && castShadows->property("value").isValid()) {
        //bool casting = castShadows->property("value").value<bool>();
        //if (casting)
        lightFragCode += "  float lightIntensity = 0.0;\n";
        lightFragCode += "  if (spotEffect > cos(radians(coneAngle*0.5))) {\n";
        lightFragCode += "    lightIntensity = 1.0;\n";
        lightFragCode += "  }\n";
    }
    return lightFragCode;
}

QString SpotLight::glslFragmentEnd()
{
    QString lightFragCode;
    /*
    Attribute castShadows = attributeByName("Casts Shadows");
    if (castShadows && castShadows->property("value").isValid()) {
        bool casting = castShadows->property("value").value<bool>();
        if (casting) {
            lightFragCode += "  vec3 lightVec = lightPos - worldPos;\n";
            lightFragCode += "  lightIntensity = lightIntensity * shadowCube(depthCubeMap, vec4(normalize(lightVec), length(lightVec))).x;";
        }
    }
    */
    return lightFragCode;
}

AmbientLight::AmbientLight()
{
    QString color("{ 'var' : 'lightColor', 'name' : 'Color', 'type' : 'color', 'value' : '#eeeeff', 'glslFragmentConstant' : true }");
    QString intensity("{ 'var' : 'lightIntensity', 'name' : 'Intensity', 'type' : 'float', 'min' : 0.0, 'max' : 1.0, 'value' : 0.2, 'glslFragmentConstant' : true }");

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
