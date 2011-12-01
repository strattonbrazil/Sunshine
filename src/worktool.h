#ifndef WORKTOOL_H
#define WORKTOOL_H

#include <QMouseEvent>
#include "contextmenu.h"
#include "util.h"
#include "panelgl.h"

class PanelGL;

class WorkTool
{
public:
    virtual bool isViewable(PanelGL* panel) = 0;
    virtual bool init(PanelGL* panel, QString command, int button) = 0;
    virtual bool wantsMouse() = 0;
    virtual void mouseMoved(QMouseEvent* event, int xDiff, int yDiff) = 0;
    virtual void finish(QMouseEvent* event) = 0;
    virtual void cancel(QMouseEvent* event) = 0;
    virtual QList<ContextAction*> actions() = 0;

};

#endif // WORKTOOL_H
