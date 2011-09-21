#ifndef OBJECT_TOOLS_H
#define OBJECT_TOOLS_H

#include "worktool.h"

class TranslateTransformable : public WorkTool
{
public:
    bool isViewable(PanelGL* panel);
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

/*
class RotateTransformable : public WorkTool
{
    class RotateTransformable : public WorkTool
    {
    public:
        bool isViewable(PanelGL* panel);
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
};
*/

class PointTool : public CursorTool
{
    bool isViewable() { return TRUE; }
    QIcon icon() {}
    QString label() { return "Cursor"; }
    void activate() {}
    void deactivate() {};
};

#endif // OBJECT_TOOLS_H
