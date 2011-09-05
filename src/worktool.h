#ifndef WORKTOOL_H
#define WORKTOOL_H

#include <QMouseEvent>
#include <QSharedPointer>
#include "contextmenu.h"
#include "util.h"
#include "panelgl.h"

class PanelGL;

class WorkTool
{
public:
    virtual bool isViewable() = 0;
    virtual void init(PanelGL* panel, int inAxis) = 0;
    virtual void mouseMoved(QMouseEvent* event) = 0;
    virtual void finish(QMouseEvent* event) = 0;
    virtual void cancel(QMouseEvent* event) = 0;
    virtual QList<ContextAction*> actions() = 0;
};
typedef QSharedPointer<WorkTool> WorkToolP;

#endif // WORKTOOL_H
