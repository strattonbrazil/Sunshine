#ifndef CURSOR_TOOLS_H
#define CURSOR_TOOLS_H

#include <QIcon>
#include <QSharedPointer>
#include <QString>
#include <QMouseEvent>
#include <QWidgetAction>
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
    virtual int drawSettings(PanelGL* panel, MeshP mesh) {
        int workMode = SunshineUi::workMode();
        if (workMode == WorkMode::OBJECT) {
            if (mesh->isSelected() && panel->_hoverMesh == mesh) { // any mode
                if (panel->_hoverVert != 0) return DrawSettings::DRAW_VERTICES | DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::CULL_BORING_VERTICES  | DrawSettings::HIGHLIGHT_VERTICES;
                else if (panel->_hoverFace != 0) return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::HIGHLIGHT_FACES;
            }

            // if there's a mesh selected, see if hovering over anything
            bool someMeshSelected = FALSE;
            foreach(QString meshName, panel->scene()->meshes()) {
                MeshP m = panel->scene()->mesh(meshName);
                /*
                if (mesh->isSelected() && panel->_hoverMesh == mesh) {
                    if (panel->_hoverVert != 0) return DrawSettings::DRAW_VERTICES | DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::CULL_BORING_VERTICES  | DrawSettings::HIGHLIGHT_VERTICES;
                    else if (panel->_hoverFace != 0) return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::HIGHLIGHT_FACES;
                }
                */
                someMeshSelected |= m->isSelected();
            }

            // no components highlighted
            if (mesh->isSelected() || !someMeshSelected)
                return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::USE_OBJECT_COLOR | DrawSettings::HIGHLIGHT_OBJECTS;
            else
                return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::USE_OBJECT_COLOR | DrawSettings::STIPPLE_FACES;
        }

        // unselected meshes
        if (!(mesh->isSelected()))
            return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::USE_OBJECT_COLOR | DrawSettings::STIPPLE_FACES;;

        if (workMode == WorkMode::VERTEX && mesh->isSelected())
            return DrawSettings::DRAW_VERTICES | DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::HIGHLIGHT_VERTICES;
        else if (workMode == WorkMode::EDGE && mesh->isSelected())
            return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES;
        else if (workMode == WorkMode::FACE && mesh->isSelected())
            return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::HIGHLIGHT_FACES;
        else { // default to just drawing the object
            return DrawSettings::DRAW_EDGES | DrawSettings::DRAW_FACES | DrawSettings::USE_OBJECT_COLOR;
        }
    }
};
typedef QSharedPointer<CursorTool> CursorToolP;

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
public:
    PointTool() {
        _basicSelect = BasicSelectP(new BasicSelect());
    }
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
