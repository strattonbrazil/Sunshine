#include "primitive.h"

#if 0
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
#endif

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

    PrimitiveParts planePrimitive(float width, float depth, int uSegments, int vSegments) {
        PrimitiveParts parts;

        const float UDELTA = width / uSegments;
        const float VDELTA = depth / vSegments;

        const float HALF_WIDTH = width * 0.5f;
        const float HALF_DEPTH = depth * 0.5f;

        for (int i = 0; i <= vSegments; i++)
        {
            for (int j = 0; j <= uSegments; j++)
            {

                Point3 p(j*UDELTA - HALF_WIDTH, 0, i*VDELTA - HALF_DEPTH);
                parts.points.append(p);
            }
        }

        for (int i = 0; i < vSegments; i++)
        {
            for (int j = 0; j < uSegments; j++)
            {
                int c1 = j+i*vSegments;
                int c2 = c1+1;
                int c3 = c2 + uSegments + 1;
                int c4 = c1 + uSegments + 1;

                QList<int> face;
                face << c1 << c2 << c3 << c4;
                parts.faces << face;
            }
        }

        return parts;
    }

    PrimitiveParts spherePrimitive(float radius)
    {
        PrimitiveParts parts;
        parts.points = QVector<Point3>();
        parts.points << Point3(0,0,1);

        const int U_SEGMENTS = 3;
        const int V_SEGMENTS = 3;

        for (int i = 1; i < V_SEGMENTS; i++)
        {
            float phi = PI * ((float)i / V_SEGMENTS);
            for (int j = 0; j < U_SEGMENTS; j++)
            {
                float theta = 2 * PI * ((float)j / U_SEGMENTS);
                float x = radius * cos(theta) * sin(phi);
                float y = radius * sin(theta) * sin(phi);
                float z = radius * cos(phi);

                parts.points << Point3(x,y,z);
            }

        }

        parts.points << Point3(0,0,-1);

        // add the bottom row
        for (int i = 0; i < U_SEGMENTS; i++)
        {
            QList<int> face = QList<int>() << 0 << (i+1) << (i+2) % U_SEGMENTS;
            parts.faces << face;
        }

        // add the middle rows
        for (int i = 0; i < V_SEGMENTS-2; i++)
        {
            for (int j = 0; j < U_SEGMENTS; j++)
            {
                QList<int> face = QList<int>();
                int base = U_SEGMENTS*i+j+1;
                face << base;
                face << base + U_SEGMENTS;

                if (j == U_SEGMENTS - 1) { // wrap
                    face << base + 1;
                    face << base + 1 - U_SEGMENTS;
                }
                else {
                    face << base + U_SEGMENTS + 1;
                    face << base + 1;
                }
                parts.faces << face;
            }
        }

        // add the top row
        const int NUM_POINTS = parts.points.count() - 1;
        for (int i = 0; i < U_SEGMENTS; i++)
        {
            int base = NUM_POINTS - U_SEGMENTS;
            QList<int> face;
            if (i == U_SEGMENTS - 1)
                face << base+i << NUM_POINTS << base;
            else
                face << base+i << NUM_POINTS << base+1+i;
            parts.faces << face;
        }

        /*
        std::cout << "vertices: " << std::endl;
        for (int i = 0; i < parts.points.count(); i++)
        {
            std::cout << i << ": " << parts.points.at(i) << std::endl;
        }
        std::cout << std::endl;

        std::cout << "faces: " << std::endl;
        for (int i = 0; i < parts.faces.count(); i++)
        {
            QList<int> face = parts.faces.at(i);
            for (int j = 0; j < face.count(); j++)
                std::cout << face.at(j) << " ";
            std::cout << std::endl;
        }
        */

        return parts;
    }
};
