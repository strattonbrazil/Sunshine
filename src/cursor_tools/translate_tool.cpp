#include "cursor_tools.h"
#include <face_util.h>
#include <vertex_util.h>

void TranslateTool::cancel(PanelGL* panel)
{
    // restore original mesh centers
    foreach(QString meshName, panel->scene()->meshes()) {
        Mesh* mesh = panel->scene()->mesh(meshName);
        if (mesh->isSelected())
            mesh->setCenter(mesh->centerReference());
    }
    _translating = FALSE;
}

void TranslateTool::mousePressed(PanelGL *panel, QMouseEvent *event)
{
    _pick = event->pos();

    // if translating, release and return
    if (_translating) {
        _translating = FALSE;
    } else {
        // click on something, that is the origin
        if (panel->_hoverMesh != 0 && panel->_hoverMesh->isSelected()) {
            Point3 rayOrig = panel->camera()->eye();
            Vector3 rayDir = panel->computeRayDirection(event->pos());

            QList<Triangle> triangles = panel->_meshGrid.trianglesByPoint(QPoint(event->pos().x(), panel->height() - event->pos().y()));
            FaceUtil::FaceHit faceHit = FaceUtil::closestFace(triangles, rayOrig, rayDir, false);
            VertexUtil::VertexHit vertexHit = VertexUtil::closestVertex(panel, event, false);

            // determine origin of move
            if (vertexHit.vertex != 0)
                _origin = vertexHit.mesh->objectToWorld().map(vertexHit.vertex->pos());
            else
                _origin = rayOrig + faceHit.range.x() * rayDir;

            foreach(QString meshName, panel->scene()->meshes()) {
                Mesh* mesh = panel->scene()->mesh(meshName);
                if (mesh->isSelected())
                    mesh->setCenterReference(mesh->center());
            }

            _translating = TRUE;
        }
        else
            std::cout << "missed something" << std::endl;
    }
}

void TranslateTool::mouseMoved(PanelGL *panel, QMouseEvent *event)
{
    mouseDragged(panel, event);
}

void TranslateTool::mouseDragged(PanelGL *panel, QMouseEvent *event)
{
    if (_translating) {
        Point3 rayOrig = panel->camera()->eye();
        Vector3 rayDir = panel->computeRayDirection(event->pos());

        QList<Triangle> triangles = panel->_meshGrid.trianglesByPoint(QPoint(event->pos().x(), panel->height() - event->pos().y()));
        FaceUtil::FaceHit faceHit = FaceUtil::closestFace(triangles, rayOrig, rayDir, false);

        Vector3 offset;

        if (faceHit.nearFace != 0 && !(faceHit.nearMesh->isSelected())) {
            Point3 dst = rayOrig + rayDir * faceHit.range.x();
            offset = dst - _origin;
        } else {
            offset = PlaneUtil::intersect(Vector3(0,1,0), Point3(0,0,0), rayDir, rayOrig) * rayDir + rayOrig - _origin;
        }

        foreach(QString meshName, panel->scene()->meshes()) {
            Mesh* mesh = panel->scene()->mesh(meshName);
            if (mesh->isSelected())
                mesh->setCenter(mesh->centerReference() + offset);
        }
    }
}

void TranslateTool::mouseReleased(PanelGL *panel, QMouseEvent *event)
{

    bool significantMove = (event->pos() - _pick).manhattanLength() > 1;
    if (event->button() == Qt::LeftButton && significantMove)
        _translating = FALSE;
    else if (event->button() == Qt::RightButton) { // cancel
    }
}
