#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "util.h"
//#include <PythonQt.h>
//#include <PythonQtCppWrapperFactory.h>

class PrimitiveParts {
public:
    QVector<Point3> points;
    QVector<QList<int> > faces;
};

namespace primitive {
    PrimitiveParts cubePrimitive(float width, float height, float depth);
    PrimitiveParts planePrimitive(float width, float depth);
};


#endif // PRIMITIVE_H
