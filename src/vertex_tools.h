#ifndef VERTEX_TOOLS_H
#define VERTEX_TOOLS_H

#include "worktool.h"

class VertexNormalizer : public WorkTool
{
public:
    bool isViewable(PanelGL* panel);
    QList<ContextAction*> actions();
    bool init(PanelGL* panel, QString command, int button);
    bool wantsMouse() { return FALSE; }
    void mouseMoved(PanelGL* panel, QMouseEvent* event, int dx, int dy) {}
    void finish(QMouseEvent* event) {}
    void cancel(QMouseEvent* event) {}
private:
    int axis;
    QPoint lastP;
    QPoint currentP;
    int xDiff;
    PanelGL* panel;
};

#endif // VERTEX_TOOLS_H
