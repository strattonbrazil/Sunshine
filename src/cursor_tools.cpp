#include "cursor_tools.h"

#include <QHBoxLayout>


WorkModeAction::WorkModeAction(QObject* parent) : QWidgetAction(parent)
{
    QWidget* widget = new QWidget();
    widget->setLayout(new QHBoxLayout());
    widget->layout()->addWidget(&objectMenu);
    widget->layout()->addWidget(&anyMenu);
    widget->layout()->addWidget(&vertexMenu);
    widget->layout()->addWidget(&edgeMenu);
    widget->layout()->addWidget(&faceMenu);

    objectMenu.setIcon(QIcon(":/icons/object_work_mode.png"));

    //anyMenu.addAction("A");
    //vertexMenu.addAction("V");
    //edgeMenu.addAction("E");
    //faceMenu.addAction("F");

    setDefaultWidget(widget);
}



