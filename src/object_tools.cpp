#include "object_tools.h"

bool TranslateTransformable::isViewable()
{
    return TRUE;

}

QList<ContextAction*> TranslateTransformable::actions()
{
    QList <ContextAction*> actions;
    actions << new ContextAction(".Move.", 0, this);
    return actions;
}

void TranslateTransformable::init(QMouseEvent* event, PanelGL* inPanel, int inAxis) {
    axis = inAxis;
    panel = inPanel;
    xDiff = 0;

    // save off object positions
    QHashIterator<int,MeshP> meshes = panel->scene()->meshes();
    while (meshes.hasNext()) {
        int meshKey = meshes.key();
        MeshP mesh = meshes.value();

        if (mesh->isSelected())
            mesh->setCenterReference(mesh->center());
    }

    lastP = panel->centerMouse();
}

void TranslateTransformable::mouseMoved(QMouseEvent* event) {
    float scale = 0.01f;
    currentP = event->pos();
    xDiff += currentP.x() - lastP.x();
    if (axis == Axis::GlobalX || axis == Axis::GlobalY || axis == Axis::GlobalZ) {
        Vector3 direction;
        if (axis == Axis::GlobalX) direction = Vector3(1,0,0);
        else if (axis == Axis::GlobalY) direction = Vector3(0,1,0);
        else if (axis == Axis::GlobalZ) direction = Vector3(0,0,1);
        direction = direction * xDiff * scale;
        QHashIterator<int,MeshP> meshes = panel->scene()->meshes();
        while (meshes.hasNext()) {
            int meshKey = meshes.key();
            MeshP mesh = meshes.value();

            if (mesh->isSelected())
                mesh->setCenter(mesh->centerReference() + direction);
        }
    }
}

void TranslateTransformable::cancel(QMouseEvent* event) {
    // restore objects to reference positions
    QHashIterator<int,MeshP> meshes = panel->scene()->meshes();
    while (meshes.hasNext()) {
        int meshKey = meshes.key();
        MeshP mesh = meshes.value();

        if (mesh->isSelected())
            mesh->setCenter(mesh->centerReference());
    }
}
