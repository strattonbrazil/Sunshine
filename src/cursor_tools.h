#ifndef CURSOR_TOOLS_H
#define CURSOR_TOOLS_H

#include <QIcon>
#include <QString>
#include <QMouseEvent>
#include <QWidgetAction>
#include "panelgl.h"
#include "util.h"
#include "select.h"

class CursorTool : public QObject
{
    Q_OBJECT
public:
    virtual bool isViewable() { return TRUE; } // on sidebar
    virtual QIcon icon() = 0;
    virtual QString label() = 0;
    virtual QCursor cursor() { return Qt::ArrowCursor; }
    virtual void activate() {}
    virtual void deactivate() {}
    virtual bool canMouseCancel(PanelGL* panel) { return FALSE; }
    virtual void cancel(PanelGL* panel) {}
    virtual void mousePressed(PanelGL* panel, QMouseEvent* event) {}
    virtual void mouseDoubleClicked(PanelGL* panel, QMouseEvent* event) {}
    virtual void mouseMoved(PanelGL*panel, QMouseEvent* event) {}
    virtual void mouseDragged(PanelGL* panel, QMouseEvent* event) {}
    virtual void mouseReleased(PanelGL* panel, QMouseEvent* event) {}
    virtual void preDrawOverlay(PanelGL* panel) {}
    virtual void postDrawOverlay(PanelGL* panel) {}
    virtual void updateMenu(QMenu* menu) {}
    virtual int workMode() { return WorkMode::OBJECT; }
    virtual QToolBar* toolbar() { return 0; }
    virtual int drawSettings(PanelGL* panel, Mesh* mesh) {
        int mode = workMode();
        if (mode == WorkMode::OBJECT) {
            if (mesh->isSelected())
                return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::USE_OBJECT_COLOR | DrawSettings::STIPPLE_FACES;
            else
                return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::USE_OBJECT_COLOR;
        }

        if (mode == WorkMode::VERTEX && mesh->isSelected()) {
            return DrawSettings::DRAW_VERTICES | DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::HIGHLIGHT_VERTICES | DrawSettings::USE_OBJECT_COLOR;
        }
        else if (mode == WorkMode::EDGE && mesh->isSelected())
            return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES;
        else if (mode == WorkMode::FACE && mesh->isSelected())
            return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::HIGHLIGHT_FACES;
        else { // default to just drawing the object
            return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::USE_OBJECT_COLOR;
        }
    }
protected:
    BasicSelect _basicSelect;
};

class WorkModeAction : public QWidgetAction
{
    Q_OBJECT;
public:
    WorkModeAction(QObject* parent);
    QToolButton objectMenu;
    QToolButton anyMenu;
    QToolButton vertexMenu;
    QToolButton edgeMenu;
    QToolButton faceMenu;
};

class PointTool : public CursorTool
{
    Q_OBJECT
public:
    PointTool();
    ~PointTool();
    QIcon icon() { return QIcon(":/icons/point_tool.png"); }
    QString label() { return "Cursor"; }

    void mousePressed(PanelGL* panel, QMouseEvent* event);
    void mouseDoubleClicked(PanelGL* panel, QMouseEvent* event);
    void mouseDragged(PanelGL* panel, QMouseEvent* event);
    void mouseReleased(PanelGL* panel, QMouseEvent* event);
    void postDrawOverlay(PanelGL* panel);
    void updateMenu(QMenu* menu) {
        //menu->addAction(new WorkModeAction(menu));
        //menu->addAction(QIcon(":/icons/object_work_mode.png"), "other");
    }
    QToolBar* toolbar() { return _toolbar; }
    int workMode();
public slots:
    void on_workModeChanged(QAbstractButton*);
private:
    QToolBar* _toolbar;
    QToolButton* objectModeButton;
    QToolButton* vertexModeButton;
    QToolButton* edgeModeButton;
    QToolButton* faceModeButton;
};

class EditTool : public CursorTool
{
public:
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
    Mesh* _plane;
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
public:
    TranslateTool() {
        _translating = FALSE;
    }
    QIcon icon() { return QIcon(":/icons/translate_tool.png"); }
    QString label() { return "Translate"; }
    bool canMouseCancel(PanelGL* panel) { return _translating; }
    void cancel(PanelGL* panel);
    void mousePressed(PanelGL *panel, QMouseEvent *event);
    void mouseMoved(PanelGL*panel, QMouseEvent* event);
    void mouseDragged(PanelGL *panel, QMouseEvent *event);
    void mouseReleased(PanelGL *panel, QMouseEvent *event);
private:
    bool _translating;
    Point3 _origin;
    QPoint _pick;
};

class RotateTool : public CursorTool
{
    QIcon icon() { return QIcon(":/icons/rotate_tool.png"); }
    QString label() { return "Rotate"; }
};

#endif // CURSOR_TOOLS_H
