#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "util.h"

class PrimitiveParts {
public:
    PrimitiveParts();
    QVector<Point3> points;
    QVector<QList<int> > faces;
};

class PrimitivePartsWrapper : public QObject
{
    Q_OBJECT
public:
    PrimitiveParts* new_PrimitiveParts();
};

namespace primitive {
    PrimitiveParts cubePrimitive(float width, float height, float depth);
};


#endif // PRIMITIVE_H
