#include "object_tools.h"

bool TranslateTransformable::isViewable(PanelGL* panel)
{
    return SunshineUi::workMode() == WorkMode::OBJECT;
}

QList<ContextAction*> TranslateTransformable::actions()
{
    QList <ContextAction*> actions;

    Scene* scene = SunshineUi::activeScene();
    if (!scene->hasMeshSelected())
        return actions;


    if (SunshineUi::workMode() == WorkMode::OBJECT) {
        actions << new ContextAction(".Move.", 0, this);
    }

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
        Mesh* mesh = panel->scene()->mesh(meshName);

        if (mesh->isSelected())
            mesh->setCenterReference(mesh->center());
    }

    //lastP = panel->centerMouse();
    //delete action;
    return TRUE;
}

void TranslateTransformable::mouseMoved(PanelGL* panel, QMouseEvent* event, int dx, int dy)
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
            Mesh* mesh = panel->scene()->mesh(meshName);

            if (mesh->isSelected())
                mesh->setCenter(mesh->centerReference() + direction);
        }
    }
    else if (axis == Axis::Screen) {
    }
    lastP = currentP;
}

void TranslateTransformable::cancel(QMouseEvent* event) {
    // restore objects to reference positions
    foreach(QString meshName, panel->scene()->meshes()) {
        Mesh* mesh = panel->scene()->mesh(meshName);

        if (mesh->isSelected())
            mesh->setCenter(mesh->centerReference());
    }
}

bool ScaleTransformable::isViewable(PanelGL* panel)
{
    return SunshineUi::workMode() == WorkMode::OBJECT;
}

QList<ContextAction*> ScaleTransformable::actions()
{
    QList <ContextAction*> actions;

    Scene* scene = SunshineUi::activeScene();
    if (!scene->hasMeshSelected())
        return actions;


    if (SunshineUi::workMode() == WorkMode::OBJECT) {
        actions << new ContextAction(".Scale.", 0, this);
    }

    return actions;
}

bool ScaleTransformable::init(PanelGL* inPanel, QString command, int button) {
    ContextMenu popup;
    popup.addAction("Uniform");
    popup.addAction("X");
    popup.addAction("Y");
    popup.addAction("Z");
    QAction* action = popup.exec(QCursor::pos());
    if (!action)
        return FALSE;

    if (action->text() == "Uniform")
        axis = Axis::NoAxis;
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
        Mesh* mesh = panel->scene()->mesh(meshName);

        if (mesh->isSelected())
            mesh->setScaleReference(mesh->scale());
    }

    //lastP = panel->centerMouse();
    //delete action;
    return TRUE;
}

void ScaleTransformable::mouseMoved(PanelGL* panel, QMouseEvent* event, int dx, int dy)
{
    currentP = event->pos();
    xDiff += dx;
    float scale = 1.0f + xDiff*0.01f;
    if (axis == Axis::GlobalX || axis == Axis::GlobalY || axis == Axis::GlobalZ || axis == Axis::NoAxis) {
        Vector3 scaleVector;
        if (axis == Axis::GlobalX)
            scaleVector = Vector3(scale,1,1);
        else if (axis == Axis::GlobalY)
            scaleVector = Vector3(1,scale,1);
        else if (axis == Axis::GlobalZ)
            scaleVector = Vector3(1,1,scale);
        else if (axis == Axis::NoAxis)
            scaleVector = Vector3(scale,scale,scale);

        foreach(QString meshName, panel->scene()->meshes()) {
            Mesh* mesh = panel->scene()->mesh(meshName);

            if (mesh->isSelected())
                mesh->setScale(Vector3(mesh->scaleReference().x() * scaleVector.x(),
                                       mesh->scaleReference().y() * scaleVector.y(),
                                       mesh->scaleReference().z() * scaleVector.z()));

        }
    }
    lastP = currentP;
}

void ScaleTransformable::cancel(QMouseEvent* event) {
    // restore objects to reference positions
    foreach(QString meshName, panel->scene()->meshes()) {
        Mesh* mesh = panel->scene()->mesh(meshName);

        if (mesh->isSelected())
            mesh->setScale(mesh->scaleReference());
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
    QHashIterator<int,Mesh*> meshes = panel->scene()->meshes();
    while (meshes.hasNext()) {
        meshes.next();
        int meshKey = meshes.key();
        Mesh* mesh = meshes.value();

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
        QHashIterator<int,Mesh*> meshes = panel->scene()->meshes();
        while (meshes.hasNext()) {
            meshes.next();
            int meshKey = meshes.key();
            Mesh* mesh = meshes.value();

            if (mesh->isSelected())
                mesh->setCenter(mesh->centerReference() + direction);
        }
    }
    lastP = currentP;
}

void RotateTransformable::cancel(QMouseEvent* event) {
    // restore objects to reference positions
    QHashIterator<int,Mesh*> meshes = panel->scene()->meshes();
    while (meshes.hasNext()) {
        meshes.next();
        int meshKey = meshes.key();
        Mesh* mesh = meshes.value();

        if (mesh->isSelected())
            mesh->setCenter(mesh->centerReference());
    }
}

*/
