#include "light.h"
#include <aqsis/aqsis.h>
#include <aqsis/ri/ri.h>
#include <sunshine.h>
#include "render_util.h"

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

void PointLight::prepare(Scene* scene)
{

}

void PointLight::prepass(Scene* scene)
{

}

SpotLight::SpotLight()
{
    QString color("{ 'var' : 'lightColor', 'name' : 'Color', 'type' : 'color', 'value' : '#ffffff', 'glslFragmentConstant' : true }");
    QString intensity("{ 'var' : 'uniformLightIntensity', 'name' : 'Intensity', 'type' : 'float', 'min' : 0.0, 'max' : 1000.0, 'value' : 10.0, 'glslFragmentConstant' : true }");
    QString coneAngle("{ 'var' : 'coneAngle', 'name' : 'Cone Angle', 'type' : 'float', 'min' : 1.0, 'max' : 150.0, 'value' : 60.0, 'glslFragmentConstant' : true }");
    QString castShadows("{ 'var' : 'castShadows', 'name' : 'Casts Shadows', 'type' : 'bool', 'value' : true }");
    //QString shadowBias("{ 'var' : 'shadowBias', 'name' : 'Shadow Bias', 'type' : 'float', 'min' : -100.0, 'max' : 100.0, 'value' : 0.1 }");
    //QString spotDir("{ 'var' : 'spotDir', 'name' : 'Spot Direction', 'type' : 'vector3', 'getter' : 'spotDir', 'glslFragmentConstant' : true }");
    QString shadowResolution("{ 'var' : 'shadowResolution', 'name' : 'Shadow Resolution', 'type' : 'int', 'min' : 32, 'max' : 4096, 'value' : 1024 }");

    QStringList atts;
    atts << color << intensity << coneAngle << castShadows << shadowResolution;// << shadowBias;// << spotDir;

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

void SpotLight::prepare(Scene* scene)
{
    /*
    Attribute castShadows = attributeByName("Casts Shadows");
    if (castShadows && castShadows->property("value").isValid()) {

    }
    */
    //Attribute position = ;
    //position.property("value").toFloat();
    QVector3D position = getBoundValue<QVector3D>(this, attributeByName("Position"));
    QVector3D lookat = position + lookDir();
    float coneAngle = PI * attributeByName("Cone Angle")->property("value").value<float>() / 180.0f;
    RtFloat intensity = attributeByName("Intensity")->property("value").value<float>();
    RtPoint from = { position.x(), position.y(), position.z() };
    RtPoint to = { lookat.x(), lookat.y(), lookat.z() };
    QColor color = attributeByName("Color")->property("value").value<QColor>();
    RtColor c = { color.redF(), color.greenF(), color.blueF() };

    bool castsShadow = attributeByName("Casts Shadows")->property("value").value<bool>();

    if (castsShadow)
    {
        QString shadowPath = QString(getenv("AQSIS_TEXTURE_PATH")).split(":")[0] + "/" + scene->assetName(this) + ".shd";
        char cShadowPath[1000];

        strcpy(cShadowPath, shadowPath.toStdString().c_str());

        char *shadowPaths[]= { cShadowPath, RI_NULL };
    //    float shadowBias = attributeByName("Shadow Bias")->property("value").value<float>();

//RiOption("shadow", "bias", (RtPointer)&shadowBias, RI_NULL);

  //      std::cout << "shadow bias: " << shadowBias << std::endl;
        RiDeclare("shadowname", "uniform string");
        RiLightSource("shadowspot", "from", from, "to", to, "intensity", &intensity, "coneangle", &coneAngle, "lightcolor", &c, "shadowname", shadowPaths, RI_NULL);
    }
    else
        RiLightSource("spotlight", "from", from, "to", to, "intensity", &intensity, "coneangle", &coneAngle, "lightcolor", &c, RI_NULL);

    //LightSource "shadowspot" 1 "intensity" 50 "from" [1 5 0] "to" [0 0 0]
    //                              "shadowname" ["spot1.tx"]

}

void SpotLight::prepass(Scene* scene)
{

    // render shadow map
    bool castsShadow = attributeByName("Casts Shadows")->property("value").value<bool>();
    if (castsShadow) {
        QString shadowPath = QString(getenv("AQSIS_TEXTURE_PATH")).split(":")[0];

        //Scene* scene = SunshineUi::activeScene();
        QString picFile = shadowPath + "/" + scene->assetName(this) + ".z";
        QString texFile = shadowPath + "/" + scene->assetName(this) + ".shd";
        char picName[1000];
        char texName[1000];
        strcpy(picName, picFile.toStdString().c_str());
        strcpy(texName, texFile.toStdString().c_str());

        std::cout << "writing shadow map: " << picFile << std::endl;

        float shadowResolution = attributeByName("Shadow Resolution")->property("value").value<int>();

        RiBegin(RI_NULL);
        RiDisplay(picName, "zfile", "z", RI_NULL);
        RiFormat(shadowResolution, shadowResolution, 1);
        RiPixelSamples(1, 1);
        RtFloat jitterVal = 0;
        const char* midpoint = "midpoint";
        RiHider("hidden", "jitter", &jitterVal, "depthfilter", &midpoint, RI_NULL);
        RiSurface("null", RI_NULL);

        float coneAngle = attributeByName("Cone Angle")->property("value").value<float>();


        Camera camera;
        camera.setCenter(this->center());
        camera.setRotate(this->rotate());
        camera.setScale(this->scale());
        camera.setFOV(coneAngle*2);

        RenderUtil::renderScene(scene, &camera);

        RiMakeShadow(picName, texName, RI_NULL);

        RiEnd();



        std::cout << "converting " << picName << " to " << texName << std::endl;
        //
    }
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

void AmbientLight::prepare(Scene* scene)
{
    RtFloat intensity = attributeByName("Intensity")->property("value").value<float>();
    QColor color = attributeByName("Color")->property("value").value<QColor>();
            //getBoundValue<QVector3D>(this, attributeByName("Color"));

    RtColor c = { color.redF(), color.greenF(), color.blueF() };
    std::cout << color.redF() << " " << color.greenF() << " " << color.blueF() << std::endl;

    RiLightSource("ambientlight", "intensity", &intensity, "lightcolor", &c, RI_NULL);
}

void AmbientLight::prepass(Scene* scene)
{

}
