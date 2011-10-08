#ifndef CURSOR_TOOLS_H
#define CURSOR_TOOLS_H

#include <QIcon>
#include <QSharedPointer>
#include <QString>
#include <QMouseEvent>
#include "panelgl.h"
#include "util.h"
#include "select.h"

class CursorTool
{
public:
    virtual bool isViewable() { return TRUE; } // on sidebar
    virtual QIcon icon() = 0;
    virtual QString label() = 0;
    virtual QCursor cursor() { return Qt::ArrowCursor; }
    virtual void activate() {}
    virtual void deactivate() {}
    virtual void mousePressed(PanelGL* panel, QMouseEvent* event) {}
    virtual void mouseDragged(PanelGL* panel, QMouseEvent* event) {}
    virtual void mouseReleased(PanelGL* panel, QMouseEvent* event) {}
    virtual void preDrawOverlay(PanelGL* panel) {}
    virtual void postDrawOverlay(PanelGL* panel) {}
    virtual int drawSettings() { return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::USE_OBJECT_COLOR; }
};
typedef QSharedPointer<CursorTool> CursorToolP;

class PointTool : public CursorTool
{
public:
    PointTool() {
        _basicSelect = BasicSelectP(new BasicSelect());
    }
    QIcon icon() { return QIcon(":/icons/point_tool.png"); }
    QString label() { return "Cursor"; }
    void mousePressed(PanelGL* panel, QMouseEvent* event);
    void mouseDragged(PanelGL* panel, QMouseEvent* event);
    void mouseReleased(PanelGL* panel, QMouseEvent* event);
    void postDrawOverlay(PanelGL* panel);
private:
    BasicSelectP _basicSelect;
};

class EditTool : public CursorTool
{
    QIcon icon() { return QIcon(":/icons/edit_tool.png"); }
    QString label() { return "Edit"; }
    int drawSettings() { return DrawSettings::DRAW_VERTICES | DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES; }
};

class DrawBoxTool : public CursorTool
{
public:
    QIcon icon() { return QIcon(":/icons/draw_box_tool.png"); }
    QString label() { return "Draw Box"; }
    QCursor cursor() { return Qt::CrossCursor; }
    void mousePressed(PanelGL *panel, QMouseEvent *event);
    void mouseDragged(PanelGL *panel, QMouseEvent *event);
    void mouseReleased(PanelGL *panel, QMouseEvent *event);
private:
    void updateWorkspacePlane();
    MeshP _plane;
    Point3 _pick;
    Point3 _current;
};

class PushPullTool : public CursorTool
{
    QIcon icon() { return QIcon(":/icons/extrude_tool.png"); }
    QString label() { return "Push/Pull"; }
};

class TranslateTool : public CursorTool
{
    QIcon icon() { return QIcon(":/icons/translate_tool.png"); }
    QString label() { return "Translate"; }
};

class RotateTool : public CursorTool
{
    QIcon icon() { return QIcon(":/icons/rotate_tool.png"); }
    QString label() { return "Rotate"; }
};

#endif // CURSOR_TOOLS_H
