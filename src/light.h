#ifndef LIGHT_H
#define LIGHT_H

#include "transformable.h"
#include "attribute_editor.h"

namespace LightType {
    enum { LIGHT, POINT_LIGHT, SPOT_LIGHT, AMBIENT_LIGHT };
}

class Light : public Transformable//, public Entity
{
    Q_OBJECT
public:
    Light() { _selected = false; }
    virtual QList<Attribute> glslFragmentConstants();
    virtual QString glslFragmentBegin() = 0;
    virtual QString glslFragmentEnd() = 0;
    virtual int lightType() = 0;
    int assetType() { return AssetType::LIGHT_ASSET; }
    bool isSelected() { return _selected; }
    void setSelected(bool s) { _selected = s; }
protected:
    bool _selected;
    //EntityP _constantAttributes;
};

class PointLight : public Light
{
public:
    PointLight();
    QList<Attribute> glslFragmentConstants();
    QString glslFragmentBegin();
    QString glslFragmentEnd();
    int lightType() { return LightType::POINT_LIGHT; }
};

class SpotLight : public Light
{
public:
    SpotLight();
    QString glslFragmentBegin();
    QString glslFragmentEnd();
    int lightType() { return LightType::SPOT_LIGHT; }
};

class AmbientLight : public Light
{
public:
    AmbientLight();
    QString glslFragmentBegin();
    QString glslFragmentEnd();
    int lightType() { return LightType::AMBIENT_LIGHT; }
};

#endif // LIGHT_H
