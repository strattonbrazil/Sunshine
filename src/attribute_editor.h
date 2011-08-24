#ifndef ATTRIBUTEEDITOR_H
#define ATTRIBUTEEDITOR_H

#include <QTableView>
#include <QScriptEngine>
#include <QScriptValue>
#include <iostream>
#include "exceptions.h"

class Entity
{
public:
    Entity(QStringList attributes);

    int numAttributes() const { return _attributes.size(); }
    QMap<QString,QVariant> attributeByIndex(int index) const { return _attributes[index]; }

    // called when an attribute changes in the editor
    void toInstance(QString attribute, QVariant value) {}

    // called when an attribute changes on the instance
    void toEditor(QString attribute, QVariant value) {}

    QVariant operator[](QString name) {
        for (int i = 0; i < _attributes.length(); i++) {
            QMap<QString,QVariant> attribute = _attributes[i];

            //std::cout << attribute["var"].toString() .toStdString() << " vs " << name.toStdString() << std::endl;
            if (attribute["var"].toString() == name)
                return attribute["value"];
        }

        std::cerr << "Cannot find attribute: " << name.toStdString() << std::endl;
        throw KeyErrorException();
    }

private:
    QList<QMap<QString, QVariant> > _attributes;
};
typedef QSharedPointer<Entity> EntityP;

class AttributeEditor : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AttributeEditor(QWidget *parent = 0);
    void update(EntityP instance) {
        _instance = instance;
    }
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const {
        if (role == Qt::DisplayRole) {
            if (section == 0)
                return QVariant("Property");
            else
                return QVariant("Value");
        }
        return QVariant();
    }
signals:

public slots:
private:
    EntityP _instance;
};

#endif // ATTRIBUTEEDITOR_H
