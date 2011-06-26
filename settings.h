#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QWidget>
#include <QVariant>
#include <qtpropertybrowser/qtpropertymanager.h>

class SettingsWidget : public QWidget
{
public:

                                  SettingsWidget(QString atts);
    QVariant                      getValue(QString var);
    QVariant                      operator[](QString var) { return getValue(var); }
    //QVariant                      operator[](const char* var) { return getValue(QString(var)); }
private:
    QHash<QString,QtProperty*>    _vars;
};


#endif // SETTINGS_H
