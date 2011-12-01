#ifndef BINDABLE_H
#define BINDABLE_H

#include <QObject>
#include <QVariant>

#include <iostream>
#include "exceptions.h"
#include "util.h"

class AttributeObject : public QObject
{ // convenience class for pulling out properties
    Q_OBJECT
public:
    QString type() { return this->property("type").toString(); }
    bool event(QEvent* event);
    //QVariant getter(EntityP entity);
};
typedef AttributeObject* Attribute;

class Bindable : public QObject
{
    Q_OBJECT
public:
    explicit Bindable(QObject *parent = 0);
    virtual int assetType() { return AssetType::NULL_ASSET; }
    void addAttributes(QStringList attributes);

    //int numAttributes() const { return this->size(); }
    //Attribute attributeByIndex(int index) const { return this->at(index); }
    Attribute attributeByName(QString name) const {
        foreach(Attribute attribute, _attributes) {
            if (attribute->property("name").toString() == name)
                return attribute;
        }
        //std::cerr << "Cannot find attribute: " << name.toStdString() << std::endl;
        //throw KeyErrorException();
        return Attribute(0);
    }
    int attributeCount() { return _attributes.size(); }
    QList<Attribute> attributes() { return QList<Attribute>(_attributes); }
    Attribute at(int i) { return _attributes[i]; }


    // called when an attribute changes in the editor
    void toInstance(QString attribute, QVariant value) {}

    // called when an attribute changes on the instance
    void toEditor(QString attribute, QVariant value) {}

    //T Bindable::getter(Attribute attribute);
signals:

public slots:
private:
    Q_DISABLE_COPY(Bindable)
protected:
    QList<Attribute> _attributes;
    void removeAttribute(Attribute attribute) { _attributes.removeOne(attribute); }
};

#include <typeinfo>

template <class T>
T getBoundValue(Bindable* bindable, Attribute attribute) {
  T result;

  QString funcName = attribute->property("getter").toString();

  QObject* obj = qobject_cast<QObject*>(bindable);

  // only works on types registered using qMetaTypeRegister
  const int typeId = qMetaTypeId<T>();
  QString typeName(QMetaType::typeName(typeId));

  T retVal;
  if (!QMetaObject::invokeMethod(obj, funcName.toStdString().c_str(),
                                 Qt::DirectConnection,
                                 QReturnArgument<T>(typeName.toStdString().c_str(), retVal)))
      std::cerr << "Error getting attribute: " << attribute->property("getter").toString().toStdString() << std::endl;

  return retVal;
}

template <class T>
void setBoundValue(Bindable* bindable, Attribute attribute, T inValue) {
    QString funcName = attribute->property("setter").toString();
    QObject* obj = qobject_cast<QObject*>(bindable);

    // only works on types registered using qMetaTypeRegister
    const int typeId = qMetaTypeId<T>();
    QString typeName(QMetaType::typeName(typeId));

    if (!QMetaObject::invokeMethod(obj, funcName.toStdString().c_str(),
                                   Qt::DirectConnection,
                                   QArgument<T>(typeName.toStdString().c_str(), inValue)))
                                   //QReturnArgument<T>(typeName.toStdString().c_str(), retVal)))
        std::cerr << "Error setting attribute: " << attribute->property("setter").toString().toStdString() << std::endl;
}

#endif // BINDABLE_H
