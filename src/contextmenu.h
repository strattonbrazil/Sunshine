#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>
#include "worktool.h"

class ContextMenu : public QMenu
{
public:
    ContextMenu();
    void mouseReleaseEvent(QMouseEvent* event);
    int lastButton;
};

class WorkTool;

class ContextAction : public QAction
{
    Q_OBJECT
public:
    ContextAction(QString label, QObject* parent, WorkTool* tool);
signals:
    void workToolChange(WorkTool* tool, QString label, int button);
public slots:
    void itemTriggered();
private:
    QString _label;
    WorkTool* _tool;
};

#endif // CONTEXTMENU_H
