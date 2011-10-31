#ifndef LIGHT_H
#define LIGHT_H

#include <QSharedPointer>
#include "transformable.h"
#include "attribute_editor.h"

class Light : public Transformable//, public Entity
{
public:
    QList<Attribute> glslFragmentConstants() {
        QList<Attribute> constantAttributes;
        foreach(Attribute att, _attributes) {
            if (att->property("glslFragmentConstant").isValid())
                constantAttributes << att;
        }

        return constantAttributes;
    }
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
    QString glslFragmentBegin();
    QString glslFragmentEnd();
};

#endif // LIGHT_H
