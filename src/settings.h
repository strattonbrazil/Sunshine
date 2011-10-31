#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QWidget>
#include <QVariant>
//#include <qtpropertybrowser/qtpropertymanager.h>
#include "attribute_editor.h"

class SettingsWidget : public QWidget
{
public:

                                  SettingsWidget();
                                  QVariant                      operator[](QString name) { return _instance->attributeByName(name)->property("value");}
    //QVariant                      operator[](const char* var) { return getValue(QString(var)); }
private:
    AttributeEditor*              _editor;
    BindableP                       _instance;
    //    QHash<QString,QtProperty*>    _vars;
};


#endif // SETTINGS_H
