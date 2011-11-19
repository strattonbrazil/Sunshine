#ifndef LIGHT_H
#define LIGHT_H

#include <QSharedPointer>
#include "transformable.h"
#include "attribute_editor.h"

class Light : public Transformable//, public Entity
{
public:
    virtual QList<Attribute> glslFragmentConstants();
    virtual QString glslFragmentBegin() = 0;
    virtual QString glslFragmentEnd() = 0;
protected:
    //EntityP _constantAttributes;
};
typedef QSharedPointer<Light> LightP;

class PointLight : public Light
{
public:
    PointLight();
    QList<Attribute> glslFragmentConstants();
    QString glslFragmentBegin();
    QString glslFragmentEnd();
};

class SpotLight : public Light
{
public:
    SpotLight();
    QString glslFragmentBegin();
    QString glslFragmentEnd();
};

class AmbientLight : public Light
{
public:
    AmbientLight();
    QString glslFragmentBegin();
    QString glslFragmentEnd();
};

#endif // LIGHT_H
