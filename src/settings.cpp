#include "settings.h"
#include <QVBoxLayout>
#include <QHeaderView>

SettingsWidget::SettingsWidget()
{
    this->setMinimumWidth(300);
    this->setMaximumWidth(300);

    //QGridLayout* layout = new QGridLayout();
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    QString xres("{ 'var' : 'xres', 'name' : 'Image Width', 'type' : 'int', 'min' : 1, 'max' : 4096, 'value' : 800 }");
    QString yres("{ 'var' : 'yres', 'name' : 'Image Height', 'type' : 'int', 'min' : 1, 'max' : 4096, 'value' : 600 }");

    QStringList atts;
    atts << xres << yres;

    /*
    Bindable* bindable = new Bindable();
    bindable->addAttributes(atts);
    _instance = Bindable*(bindable);

    _editor = new AttributeEditor(this);
    _editor->update(_instance);

    QTableView* view = new QTableView();
    view->verticalHeader()->hide();
    view->setModel(_editor);

    layout->addWidget(view);
    */
}

#if 0
/*
#include <QScriptValue>
#include <QScriptEngine>
#include <QGridLayout>
#include <QLabel>
#include <QDate>
#include <QDateTime>
#include <QLocale>
//#include "qtpropertybrowser.h"
#include <qtpropertybrowser/QtAbstractPropertyManager>
#include <qtpropertybrowser/QtIntPropertyManager>
#include <qtpropertybrowser/qtpropertymanager.h>
#include <qtpropertybrowser/qtvariantproperty.h>
#include <qtpropertybrowser/qttreepropertybrowser.h>

#include <iostream>
using namespace std;

SettingsWidget::SettingsWidget(QString nameSpace, QString atts)
{
    this->setMinimumWidth(400);
    this->setMaximumWidth(400);

    //QGridLayout* layout = new QGridLayout();
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate(atts); // In new versions it may need to look like engine.evaluate("(" + QString(result) + ")");
    if (engine.hasUncaughtException()) {
        int line = engine.uncaughtExceptionLineNumber();
        std::cerr << "uncaught exception at line" << line << ":" << sc.toString().toStdString() << std::endl;
        return;
    }

    //std::cout << sc.toString().toStdString() << std::endl;
    QVariantList attsList;
    qScriptValueToSequence(sc, attsList);

    QtVariantPropertyManager *variantManager = new QtVariantPropertyManager();
    //QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
    //            QString::number(3) + QLatin1String(" Group Property"));

    QHash<QString,QtProperty*> groups;
    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory();

    QtTreePropertyBrowser *variantEditor = new QtTreePropertyBrowser();
    variantEditor->setFactoryForManager(variantManager, variantFactory);
    variantEditor->setPropertiesWithoutValueMarked(true);
    variantEditor->setRootIsDecorated(false);

    foreach (QVariant item, attsList) {
        //std::cout << item.typeName() << std::endl;
        QMap<QString,QVariant> attribute = item.toMap();

        //layout->addWidget(new QLabel(att["name"].toString()), row, 0);
        //std::cout << row["name"].toString().toStdString() << std::endl;
        QtVariantProperty* item = NULL;
        if (attribute["type"].toString() == "int") {
            item = variantManager->addProperty(QVariant::Int, attribute["name"].toString());
            if (attribute.contains("default")) item->setValue(attribute["default"].toInt());
            item->setAttribute(QLatin1String("minimum"), attribute["min"].toInt());
            item->setAttribute(QLatin1String("maximum"), attribute["max"].toInt());

            _vars[attribute["var"].toString()] = item;
            //item->setAttribute(QLatin1String("singleStep"), 10);
        } else {
            std::cerr << "Unknown attribute type: " << attribute["type"].toString().toStdString() << std::endl;
            continue;
        }

        if (!attribute.contains("group"))
            attribute["group"] = "General";

        if (!groups.contains(attribute["group"].toString()))
            groups[attribute["group"].toString()] = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
                                                                     attribute["group"].toString());
        groups[attribute["group"].toString()]->addSubProperty(item);
    }

    foreach (QtProperty* group, groups) {
        variantEditor->addProperty(group);
    }

    /*
    for (int i = 0; i < items.size(); i++) {
        QHash<QString,QVariant> thing = items.at(i);
        std::cout << thing.size() << std::endl;
    }
    */

            /*
    QVariantList arr = sc.
    foreach (QVariant item, arr) {
        QHash<QString,QVariant> hash = item.toHash();
        std::cout << hash.size() << std::endl;
    }
            */




/*

    int i = 0;
    QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
                QString::number(i++) + QLatin1String(" Group Property"));

    QtVariantProperty *item = variantManager->addProperty(QVariant::Bool, QString::number(i++) + QLatin1String(" Bool Property"));
    item->setValue(true);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Int, QString::number(i++) + QLatin1String(" Int Property"));
    item->setValue(20);
    item->setAttribute(QLatin1String("minimum"), 0);
    item->setAttribute(QLatin1String("maximum"), 100);
    item->setAttribute(QLatin1String("singleStep"), 10);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Double, QString::number(i++) + QLatin1String(" Double Property"));
    item->setValue(1.2345);
    item->setAttribute(QLatin1String("singleStep"), 0.1);
    item->setAttribute(QLatin1String("decimals"), 3);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::String, QString::number(i++) + QLatin1String(" String Property"));
    item->setValue("Value");
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::String, QString::number(i++) + QLatin1String(" String Property (Password)"));
    item->setAttribute(QLatin1String("echoMode"), QLineEdit::Password);
    item->setValue("Password");
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Date, QString::number(i++) + QLatin1String(" Date Property"));
    item->setValue(QDate::currentDate().addDays(2));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Time, QString::number(i++) + QLatin1String(" Time Property"));
    item->setValue(QTime::currentTime());
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::DateTime, QString::number(i++) + QLatin1String(" DateTime Property"));
    item->setValue(QDateTime::currentDateTime());
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::KeySequence, QString::number(i++) + QLatin1String(" KeySequence Property"));
    item->setValue(QKeySequence(Qt::ControlModifier | Qt::Key_Q));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Char, QString::number(i++) + QLatin1String(" Char Property"));
    item->setValue(QChar(386));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Locale, QString::number(i++) + QLatin1String(" Locale Property"));
    item->setValue(QLocale(QLocale::Polish, QLocale::Poland));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Point, QString::number(i++) + QLatin1String(" Point Property"));
    item->setValue(QPoint(10, 10));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::PointF, QString::number(i++) + QLatin1String(" PointF Property"));
    item->setValue(QPointF(1.2345, -1.23451));
    item->setAttribute(QLatin1String("decimals"), 3);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Size, QString::number(i++) + QLatin1String(" Size Property"));
    item->setValue(QSize(20, 20));
    item->setAttribute(QLatin1String("minimum"), QSize(10, 10));
    item->setAttribute(QLatin1String("maximum"), QSize(30, 30));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::SizeF, QString::number(i++) + QLatin1String(" SizeF Property"));
    item->setValue(QSizeF(1.2345, 1.2345));
    item->setAttribute(QLatin1String("decimals"), 3);
    item->setAttribute(QLatin1String("minimum"), QSizeF(0.12, 0.34));
    item->setAttribute(QLatin1String("maximum"), QSizeF(20.56, 20.78));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Rect, QString::number(i++) + QLatin1String(" Rect Property"));
    item->setValue(QRect(10, 10, 20, 20));
    topItem->addSubProperty(item);
    item->setAttribute(QLatin1String("constraint"), QRect(0, 0, 50, 50));

    item = variantManager->addProperty(QVariant::RectF, QString::number(i++) + QLatin1String(" RectF Property"));
    item->setValue(QRectF(1.2345, 1.2345, 1.2345, 1.2345));
    topItem->addSubProperty(item);
    item->setAttribute(QLatin1String("constraint"), QRectF(0, 0, 50, 50));
    item->setAttribute(QLatin1String("decimals"), 3);

    item = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),
                    QString::number(i++) + QLatin1String(" Enum Property"));
    QStringList enumNames;
    enumNames << "Enum0" << "Enum1" << "Enum2";
    item->setAttribute(QLatin1String("enumNames"), enumNames);
    item->setValue(1);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QtVariantPropertyManager::flagTypeId(),
                    QString::number(i++) + QLatin1String(" Flag Property"));
    QStringList flagNames;
    flagNames << "Flag0" << "Flag1" << "Flag2";
    item->setAttribute(QLatin1String("flagNames"), flagNames);
    item->setValue(5);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::SizePolicy, QString::number(i++) + QLatin1String(" SizePolicy Property"));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Font, QString::number(i++) + QLatin1String(" Font Property"));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Cursor, QString::number(i++) + QLatin1String(" Cursor Property"));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Color, QString::number(i++) + QLatin1String(" Color Property"));
    topItem->addSubProperty(item);

    */




    layout->addWidget(variantEditor);



}

QVariant SettingsWidget::getValue(QString var)
{
    std::cout << "request: " << var.toStdString() << std::endl;
    QtProperty* property = _vars[var];
    if (!property) {
        std::cerr << "Unknown variable requested: getValue(" << var.toStdString() << ")" << std::endl;
        return QVariant();
    }
    QtAbstractPropertyManager* manager = property->propertyManager();

    QtIntPropertyManager* intManager = qobject_cast<QtIntPropertyManager*>(manager);
    if (intManager)
        return QVariant(intManager->value(property));

    QtVariantPropertyManager* variantManager = qobject_cast<QtVariantPropertyManager*>(manager);
    if (variantManager)
        return variantManager->value(property);

    std::cerr << "Unimplemeted property requested: getValue(" << var.toStdString() << ")" << std::endl;
    return QVariant();
}

*/
/*
RenderSettings::RenderSettings()
{
    QString xres("{ 'var' : 'xres', 'name' : 'Image Width', 'type' : 'int', 'min' : 1, 'max' : 4096}");
    QString yres("{ 'var' : 'yres', 'name' : 'Image Height', 'type' : 'int', 'min' : 1, 'max' : 4096}");
    QString atts = QStringList(xres, yres).join(",");

    //QString camera;
    QString samples("{ 'var' : 'samples', 'name' : 'Samples', 'type' : 'int', 'min' : 1, 'max' : 99999}");
        "diffuseDepth:Diffuse Depth:int:min=0;max=100",
        "reflectionDepth:Reflection Depth:int:min=0;max=100",
        "refractionDepth:Refraction Depth:int:min=0;max=100"
}
*/


#endif
