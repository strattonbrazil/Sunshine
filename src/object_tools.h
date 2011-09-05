#ifndef OBJECT_TOOLS_H
#define OBJECT_TOOLS_H

#include "worktool.h"

class TranslateTransformable : public WorkTool
{
public:
    bool isViewable();
    QList<ContextAction*> actions();
    void init(PanelGL* inPanel, int inAxis);
    void mouseMoved(QMouseEvent* event);
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
