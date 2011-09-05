#include "contextmenu.h"

#include <iostream>
#include <QMouseEvent>

ContextMenu::ContextMenu() : QMenu()
{
}

void ContextMenu::mouseReleaseEvent(QMouseEvent* event)
{
    lastButton = event->button();
    QMenu::mouseReleaseEvent(event);
}

ContextAction::ContextAction(QString label, QObject* parent, WorkTool* tool) : QAction(label, parent), _label(label), _tool(tool)
{

}

void ContextAction::itemTriggered()
{
    ContextMenu* menu = dynamic_cast<ContextMenu*>(parent());
    if (menu) {
        //std::cout << menu->lastButton << std::endl;
        //std::cout << _label.toStdString() << std::endl;

        emit workToolChange(_tool, _label, menu->lastButton);
    } else {
        std::cerr << "Unable to retrieve ContextMenu from ContextAction" << std::endl;
    }

}
