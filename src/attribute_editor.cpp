#include "attribute_editor.h"

Entity::Entity(QStringList attributes) {
    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("[ " + attributes.join(" , ") + " ]"); // In new versions it may need to look like engine.evaluate("(" + QString(result) + ")");

    QVariantList attsList;
    qScriptValueToSequence(sc, attsList);

    foreach (QVariant item, attsList) {
        //std::cout << item.typeName() << std::endl;
        QMap<QString,QVariant> attribute = item.toMap();

        _attributes << attribute;
    }
}


AttributeEditor::AttributeEditor(QWidget *parent) :
    QStandardItemModel(parent), _instance(0)
{

}

void AttributeEditor::update(EntityP instance)
{
    _instance = instance;
    clear();

    for (int i = 0; i < _instance->numAttributes(); i++) {
        QMap<QString,QVariant> attribute = _instance->attributeByIndex(i);

        QList<QStandardItem*> attributeRow;
        attributeRow << new QStandardItem(attribute["name"].toString());
        attributeRow << new QStandardItem(attribute["value"].toString());

        appendRow(attributeRow);
    }

    QStringList headers;
    headers << "Property" << "Value";
    setHorizontalHeaderLabels(headers);
}

/*
int AttributeEditor::rowCount(const QModelIndex &parent) const
{
    if (_instance != 0)
        return _instance->numAttributes();
    else
        return 0;
}

int AttributeEditor::columnCount(const QModelIndex &parent) const
{
    return 2;
}


QVariant AttributeEditor::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole)
        if (index.column() == 0)
            return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        else
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);


    if (role != Qt::DisplayRole)
        return QVariant();

    QMap<QString,QVariant> attribute = _instance->attributeByIndex(index.row());
    if (index.column() == 0)
        return attribute["name"];
    else
        return attribute["value"];
}
*/
/*
QVariant QAbstractItemModel::headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const {
    if (role == Qt::DisplayRole) {
        if (section == 0)
            return QVariant("Property");
        else
            return QVariant("Value");
    }
    return QVariant();
}
*/
