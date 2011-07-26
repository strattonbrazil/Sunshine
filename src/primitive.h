#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "util.h"

struct PrimitiveParts {
    QVector<Point3> points;
    QVector<QList<int> > faces;
};

namespace primitive {
    PrimitiveParts cubePrimitive(float width, float height, float depth);
};


#endif // PRIMITIVE_H
