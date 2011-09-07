#ifndef OBJECT_TOOLS_H
#define OBJECT_TOOLS_H

#include "worktool.h"

class TranslateTransformable : public WorkTool
{
public:
    bool isViewable();
    QList<ContextAction*> actions();
    bool init(PanelGL* panel, QString command, int button);
    bool wantsMouse() { return TRUE; }
    void mouseMoved(QMouseEvent* event, int dx, int dy);
    void finish(QMouseEvent* event) {}
    void cancel(QMouseEvent* event);
private:
    int axis;
    QPoint lastP;
    QPoint currentP;
    int xDiff;
    PanelGL* panel;
};

#endif // OBJECT_TOOLS_H
