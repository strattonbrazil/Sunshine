#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "util.h"
//#include <QScriptEngine>
//#include <QScriptContext>

class PrimitiveParts {
public:
    QVector<Point3> points;
    QVector<QList<int> > faces;
};
typedef QSharedPointer<PrimitiveParts> PrimitivePartsPointer;
//Q_DECLARE_METATYPE(PrimitivePartsPointer);

//QScriptValue constructPrimitiveParts(QScriptContext *context, QScriptEngine *engine);
//QScriptValue PrimitiveParts_setVertices(QScriptContext *context, QScriptEngine *);
//QScriptValue PrimitiveParts_setFaces(QScriptContext *context, QScriptEngine *engine);
/*
 {
     XmlStreamReaderPointer reader = qscriptvalue_cast<XmlStreamReaderPointer>(context->thisObject());
     if (!reader)
         return context->throwError(QScriptContext::TypeError, "this object is not an XmlStreamReader");
     return reader->atEnd();
 }
 */

/*
QScriptValue PrimitiveParts_ctor(QScriptContext *context, QScriptEngine *engine)
{
    //int x = context->argument(0).toInt32();
    //int y = context->argument(1).toInt32();
    return engine->toScriptValue(PrimitiveParts());
}
*/

/*
static QScriptValue getSetPrimitiveParts(QScriptContext *context, QScriptEngine *engine)
{
    QScriptValue callee = context->callee();
    if (context->argumentCount() == 1) // writing?
        callee.setProperty("value", context->argument(0));
    return callee.property("value");
}
*/

namespace primitive {
    PrimitiveParts cubePrimitive(float width, float height, float depth);
    PrimitiveParts planePrimitive(float width, float depth, int uSegments, int vSegments);
    PrimitiveParts spherePrimitive(float radius);
};


#endif // PRIMITIVE_H
