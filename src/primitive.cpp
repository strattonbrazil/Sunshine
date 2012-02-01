#include "primitive.h"

QScriptValue constructPrimitiveParts(QScriptContext *context, QScriptEngine *engine)
{
    if (!context->isCalledAsConstructor())
        return context->throwError(QScriptContext::SyntaxError, "please use the 'new' operator");

    //QIODevice *device = qobject_cast<QIODevice*>(context->argument(0).toQObject());
    //if (!device)
      //  return context->throwError(QScriptContext::TypeError, "please supply a QIODevice as first argument");

    // Create the C++ object
    PrimitiveParts *parts = new PrimitiveParts();
    PrimitivePartsPointer partsPointer(parts);

    // store the shared pointer in the script object that we are constructing
    return engine->newVariant(context->thisObject(), qVariantFromValue(partsPointer));
}

QScriptValue PrimitiveParts_setVertices(QScriptContext *context, QScriptEngine *engine)
{
    PrimitivePartsPointer ppPointer = qscriptvalue_cast<PrimitivePartsPointer>(context->thisObject());
    if (!ppPointer)
        return context->throwError(QScriptContext::TypeError, "this object is not an PrimitiveParts");

    // set vertices here
    QScriptValue vertices = context->argument(0);
    const int numVertices = vertices.property("length").toInteger();
    ppPointer->points.resize(numVertices);
    for (int i = 0; i < numVertices; i++) {
        QVector3D v = qScriptValueToValue<QVector3D>(vertices.property(i));
        ppPointer->points[i] = v;
    }

    return QScriptValue();
    //return ppPointer->reader->atEnd();
}

QScriptValue PrimitiveParts_setFaces(QScriptContext *context, QScriptEngine *engine)
{
    PrimitivePartsPointer ppPointer = qscriptvalue_cast<PrimitivePartsPointer>(context->thisObject());
    if (!ppPointer)
        return context->throwError(QScriptContext::TypeError, "this object is not an PrimitiveParts");

    // set faces here
    QScriptValue faces = context->argument(0);
    const int numFaces = faces.property("length").toInteger();
    ppPointer->faces.resize(numFaces);
    for (int i = 0; i < numFaces; i++) {
        QVariantList faceList = faces.property(i).toVariant().toList();
        for (int j = 0; j < faceList.size(); j++) {
            ppPointer->faces[i].append(faceList[j].toInt());
        }
    }

    return QScriptValue();
}

namespace primitive {
    PrimitiveParts cubePrimitive(float width, float height, float depth)
    {
        float hx = width / 2;
        float hy = height / 2;
        float hz = depth / 2;

        // create the vertices
        Point3 p0(hx,hy,hz);
        Point3 p1(hx,hy,-hz);
        Point3 p2(-hx,hy,-hz);
        Point3 p3(-hx,hy,hz);
        Point3 p4(hx,-hy,hz);
        Point3 p5(hx,-hy,-hz);
        Point3 p6(-hx,-hy,-hz);
        Point3 p7(-hx,-hy,hz);

        QList<int> f0 = QList<int>() << 0 << 1 << 2 << 3;
        QList<int> f1 = QList<int>() << 4 << 5 << 1 << 0;
        QList<int> f2 = QList<int>() << 6 << 2 << 1 << 5;
        QList<int> f3 = QList<int>() << 7 << 3 << 2 << 6;
        QList<int> f4 = QList<int>() << 7 << 4 << 0 << 3;
        QList<int> f5 = QList<int>() << 4 << 7 << 6 << 5;

        PrimitiveParts parts;
        parts.points = QVector<Point3>() << p0 << p1 << p2 << p3 << p4 << p5 << p6 << p7;
        parts.faces = QVector<QList<int> >() << f0 << f1 << f2 << f3 << f4 << f5;
        return parts;
    }

    PrimitiveParts planePrimitive(float width, float depth) {
        float hx = width / 2;
        float hz = depth / 2;

        // create the vertices
        Point3 p0(hx,0,hz);
        Point3 p1(hx,0,-hz);
        Point3 p2(-hx,0,-hz);
        Point3 p3(-hx,0,hz);

        QList<int> f0 = QList<int>() << 0 << 1 << 2 << 3;

        PrimitiveParts parts;
        parts.points = QVector<Point3>() << p0 << p1 << p2 << p3;
        parts.faces = QVector<QList<int> >() << f0;
        return parts;
    }
};
