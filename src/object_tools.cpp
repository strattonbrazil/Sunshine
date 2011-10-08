#include "object_tools.h"

bool TranslateTransformable::isViewable(PanelGL* panel)
{
    return SunshineUi::workMode() == WorkMode::LAYOUT;
}

QList<ContextAction*> TranslateTransformable::actions()
{
    QList <ContextAction*> actions;
    actions << new ContextAction(".Move.", 0, this);
    return actions;
}

bool TranslateTransformable::init(PanelGL* inPanel, QString command, int button) {
    ContextMenu popup;
    popup.addAction("Screen");
    popup.addAction("X");
    popup.addAction("Y");
    popup.addAction("Z");
    QAction* action = popup.exec(QCursor::pos());
    if (!action)
        return FALSE;

    if (action->text() == "Screen")
        axis = Axis::Screen;
    else if (action->text() == "X")
        axis = Axis::GlobalX;
    else if (action->text() == "Y")
        axis = Axis::GlobalY;
    else if (action->text() == "Z")
        axis = Axis::GlobalZ;

    panel = inPanel;
    xDiff = 0;

    // save off object positions
    foreach(QString meshName, panel->scene()->meshes()) {
        MeshP mesh = panel->scene()->mesh(meshName);

        if (mesh->isSelected())
            mesh->setCenterReference(mesh->center());
    }

    //lastP = panel->centerMouse();
    //delete action;
    return TRUE;
}

void TranslateTransformable::mouseMoved(QMouseEvent* event, int dx, int dy)
{
    float scale = 0.01f;
    currentP = event->pos();
    xDiff += dx;
    if (axis == Axis::GlobalX || axis == Axis::GlobalY || axis == Axis::GlobalZ) {
        Vector3 direction;
        if (axis == Axis::GlobalX) direction = Vector3(1,0,0);
        else if (axis == Axis::GlobalY) direction = Vector3(0,1,0);
        else if (axis == Axis::GlobalZ) direction = Vector3(0,0,1);
        direction = direction * xDiff * scale;

        foreach(QString meshName, panel->scene()->meshes()) {
            MeshP mesh = panel->scene()->mesh(meshName);

            if (mesh->isSelected())
                mesh->setCenter(mesh->centerReference() + direction);
        }
    }
    lastP = currentP;
}

void TranslateTransformable::cancel(QMouseEvent* event) {
    // restore objects to reference positions
    foreach(QString meshName, panel->scene()->meshes()) {
        MeshP mesh = panel->scene()->mesh(meshName);

        if (mesh->isSelected())
            mesh->setCenter(mesh->centerReference());
    }
}

// RotateTransformable
//
/*
bool RotateTransformable::isViewable(PanelGL* panel)
{
    return SunshineUi::workMode() == WorkMode::LAYOUT;
}

QList<ContextAction*> RotateTransformable::actions()
{
    QList <ContextAction*> actions;
    actions << new ContextAction(".Rotate.", 0, this);
    return actions;
}

bool RotateTransformable::init(PanelGL* inPanel, QString command, int button) {
    ContextMenu popup;
    popup.addAction("Object X");
    popup.addAction("Object Y");
    popup.addAction("Object Z");
    QAction* action = popup.exec(QCursor::pos());
    if (!action)
        return FALSE;

    if (action->text() == "Object X")
        axis = Axis::LocalX;
    else if (action->text() == "Object Y")
        axis = Axis::LocalY;
    else if (action->text() == "Object Z")
        axis = Axis::LocalZ;

    panel = inPanel;
    xDiff = 0;

    // save off object positions
    QHashIterator<int,MeshP> meshes = panel->scene()->meshes();
    while (meshes.hasNext()) {
        meshes.next();
        int meshKey = meshes.key();
        MeshP mesh = meshes.value();

        if (mesh->isSelected())
            mesh->setRotateReference(mesh->rotate());
    }

    //lastP = panel->centerMouse();
    //delete action;
    return TRUE;
}

void RotateTransformable::mouseMoved(QMouseEvent* event, int dx, int dy)
{
    float scale = 0.01f;
    currentP = event->pos();
    xDiff += dx;
    if (axis == Axis::LocalX || axis == Axis::LocalY || axis == Axis::LocalZ) {
        Vector3 direction;
        if (axis == Axis::GlobalX) direction = Vector3(1,0,0);
        else if (axis == Axis::GlobalY) direction = Vector3(0,1,0);
        else if (axis == Axis::GlobalZ) direction = Vector3(0,0,1);
        direction = direction * xDiff * scale;
        QHashIterator<int,MeshP> meshes = panel->scene()->meshes();
        while (meshes.hasNext()) {
            meshes.next();
            int meshKey = meshes.key();
            MeshP mesh = meshes.value();

            if (mesh->isSelected())
                mesh->setCenter(mesh->centerReference() + direction);
        }
    }
    lastP = currentP;
}

void RotateTransformable::cancel(QMouseEvent* event) {
    // restore objects to reference positions
    QHashIterator<int,MeshP> meshes = panel->scene()->meshes();
    while (meshes.hasNext()) {
        meshes.next();
        int meshKey = meshes.key();
        MeshP mesh = meshes.value();

        if (mesh->isSelected())
            mesh->setCenter(mesh->centerReference());
    }
}

*/
