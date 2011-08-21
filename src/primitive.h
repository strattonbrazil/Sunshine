#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "util.h"
#include <PythonQt.h>
#include <PythonQtCppWrapperFactory.h>

class PrimitiveParts {
public:
    QVector<Point3> points;
    QVector<QList<int> > faces;
};

class PrimitivePartsWrapper : public QObject
{
    Q_OBJECT
public:
    PrimitivePartsWrapper(PrimitiveParts* obj) { _ptr = obj; }
private:
    PrimitiveParts* _ptr;
};

class PrimitivePartsConstructor : public QObject {
  Q_OBJECT
public slots:
  // add a constructor
  PrimitiveParts* new_PrimitiveParts() { return new PrimitiveParts(); }

  // add a destructor
  void delete_PrimitiveParts(PrimitiveParts* o) { delete o; }
};

// a factory that can create wrappers for CustomObject2
class CustomFactory : public PythonQtCppWrapperFactory
{
public:
    virtual QObject* create(const QByteArray& name, void *ptr) {
        std::cout << "here" << std::endl;
        if (name == "PrimitiveParts") {
            return new PrimitivePartsWrapper((PrimitiveParts*)ptr);
        }
        return NULL;
    }
};

namespace primitive {
    PrimitiveParts cubePrimitive(float width, float height, float depth);
};


#endif // PRIMITIVE_H
