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
    void mouseMoved(PanelGL* panel, QMouseEvent* event, int dx, int dy);
    void finish(QMouseEvent* event) {}
    void cancel(QMouseEvent* event);
private:
    int axis;
    QPoint lastP;
    QPoint currentP;
    int xDiff;
    PanelGL* panel;
};

class ScaleTransformable : public WorkTool
{
public:
    bool isViewable(PanelGL* panel);
    QList<ContextAction*> actions();
    bool init(PanelGL* panel, QString command, int button);
    bool wantsMouse() { return TRUE; }
    void mouseMoved(PanelGL* panel, QMouseEvent* event, int dx, int dy);
    void finish(QMouseEvent* event) {}
    void cancel(QMouseEvent* event);
private:
    Vector3 scaleVector;
    int axis;
    QPoint lastP;
    QPoint currentP;
    int xDiff;
    PanelGL* panel;
};

class SplitPolygon : public WorkTool
{
    Q_OBJECT
public:
    bool isViewable(PanelGL* panel);
    QList<ContextAction*> actions();
    bool init(PanelGL* panel, QString command, int button);
    bool wantsMouse() { return TRUE; }
    bool ravageMouse() { return FALSE; }
    void mousePressed(PanelGL *panel, QMouseEvent *event);
    void mouseMoved(PanelGL* panel, QMouseEvent* event, int dx, int dy);
    void finish(QMouseEvent* event) {}
    void cancel(QMouseEvent* event);
    QString displayName() { return "Split Polygon"; }
    void postDrawOverlay(PanelGL *panel);
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



#endif // OBJECT_TOOLS_H
