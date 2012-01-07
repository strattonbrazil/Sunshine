#ifndef WORKTOOL_H
#define WORKTOOL_H

#include <QObject>
#include <QMouseEvent>
#include "contextmenu.h"
#include "util.h"
#include "panelgl.h"

class PanelGL;

class WorkTool : public QObject
{
public:
    virtual bool isViewable(PanelGL* panel) = 0;
    virtual bool init(PanelGL* panel, QString command, int button) = 0;
    virtual bool wantsMouse() = 0;
    virtual bool ravageMouse() { return TRUE; }
    virtual void mousePressed(PanelGL* panel, QMouseEvent* event) {}
    virtual void mouseMoved(PanelGL* panel, QMouseEvent* event, int xDiff, int yDiff) = 0;
    virtual void mouseDragged(PanelGL* panel, QMouseEvent* event) {}
    virtual void mouseReleased(PanelGL* panel, QMouseEvent* event) {}
    virtual void finish(QMouseEvent* event) = 0;
    virtual void cancel(QMouseEvent* event) = 0;
    virtual void preDrawOverlay(PanelGL* panel) {}
    virtual void postDrawOverlay(PanelGL* panel) {}
    virtual QString displayName() { return ""; }
    virtual QList<ContextAction*> actions() = 0;
};

#endif // WORKTOOL_H
