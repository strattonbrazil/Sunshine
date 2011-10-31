#include "cursor_tools.h"

#include <QHBoxLayout>
#include <face_util.h>
#include <vertex_util.h>

WorkModeAction::WorkModeAction(QObject* parent) : QWidgetAction(parent)
{
    QWidget* widget = new QWidget();
    widget->setLayout(new QHBoxLayout());
    widget->layout()->addWidget(&objectMenu);
    widget->layout()->addWidget(&anyMenu);
    widget->layout()->addWidget(&vertexMenu);
    widget->layout()->addWidget(&edgeMenu);
    widget->layout()->addWidget(&faceMenu);

    objectMenu.setIcon(QIcon(":/icons/object_work_mode.png"));

    //anyMenu.addAction("A");
    //vertexMenu.addAction("V");
    //edgeMenu.addAction("E");
    //faceMenu.addAction("F");

    setDefaultWidget(widget);
}

void PointTool::mousePressed(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect->mousePressed(panel, event);
}

void PointTool::mouseDoubleClicked(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect->mouseDoubleClicked(panel, event);
}

void PointTool::mouseDragged(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect->mouseDragged(panel, event);
}

void PointTool::mouseReleased(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect->mouseReleased(panel, event);
}

void PointTool::postDrawOverlay(PanelGL *panel)
{
    _basicSelect->postDrawOverlay(panel);
}

void TranslateTool::cancel(PanelGL* panel)
{
    // restore original mesh centers
    foreach(QString meshName, panel->scene()->meshes()) {
        MeshP mesh = panel->scene()->mesh(meshName);
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
                MeshP mesh = panel->scene()->mesh(meshName);
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
            MeshP mesh = panel->scene()->mesh(meshName);
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
