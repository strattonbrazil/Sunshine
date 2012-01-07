#include "bindable.h"
#include <QStringList>
#include <QScriptEngine>
#include <QDynamicPropertyChangeEvent>

/*
T Bite::getter(Attribute attribute) {
  T result;

  return T;
}
*/

bool AttributeObject::event(QEvent *event)
{
    return QObject::event(event);
}

Bindable::Bindable(QObject *parent) :
    QObject(parent)
{
}


/*
QVariant AttributeObject::getter(Bindable* Bindable)
{
    QString funcName = property("getter").toString();

    Bindable *BindableRef = Bindable.data();
    PointLight* lObj = static_cast<PointLight*>(BindableRef);
    //Light* obj = qobject_cast<Light>(BindableRef);
    QObject* obj = qobject_cast<QObject*>(lObj);

    std::cout << lObj->center() << std::endl;

    Point3 retVal;
    if (!QMetaObject::invokeMethod(obj, "center",
                                   Qt::DirectConnection,
                                   Q_RETURN_ARG(Point3, retVal)))
        std::cerr << "Error getting attribute: " << property("name").toString() << std::endl;

    std::cout << retVal << std::endl;

    return QVariant(retVal);
}
*/

void Bindable::addAttributes(QStringList attributes) {
    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("[ " + attributes.join(" , ") + " ]"); // In new versions it may need to look like engine.evaluate("(" + QString(result) + ")");

    QVariantList attsList;
    qScriptValueToSequence(sc, attsList);

    foreach (QVariant item, attsList) {
        //std::cout << item.typeName() << std::endl;
        QMap<QString,QVariant> attribute = item.toMap();
        Attribute obj(new AttributeObject);
        QMapIterator<QString, QVariant> i(attribute);
        while (i.hasNext()) {
            i.next();
            obj->setProperty(i.key().toStdString().c_str(), i.value());
        }

        _attributes << obj;
    }
}




