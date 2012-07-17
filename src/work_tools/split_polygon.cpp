#include "object_tools.h"
#include "face_util.h"

bool SplitPolygon::isViewable(PanelGL *panel)
{
    return SunshineUi::workMode() == WorkMode::OBJECT && panel->scene()->hasMeshSelected();
}

bool SplitPolygon::init(PanelGL *panel, QString command, int button)
{
    return true;
}

void SplitPolygon::mousePressed(PanelGL *panel, QMouseEvent *event)
{
    Point3 rayOrig = panel->camera()->eye();
    Vector3 rayDir = panel->computeRayDirection(event->pos());
    QList<Triangle> triangles = panel->_meshGrid.trianglesByPoint(QPoint(event->pos().x(), panel->height()-event->pos().y()));
    FaceUtil::FaceHit faceHit = FaceUtil::closestFace(triangles, rayOrig, rayDir, false);

    if (faceHit.nearFace.is_valid()) {

    }
}

void SplitPolygon::mouseMoved(PanelGL* panel, QMouseEvent *event, int dx, int dy)
{
}

QList<ContextAction*> SplitPolygon::actions()
{
    QList<ContextAction*> actions;
    actions << new ContextAction("Split Polygon", 0, this);

    return actions;
}

void SplitPolygon::cancel(QMouseEvent* event)
{

}

void SplitPolygon::postDrawOverlay(PanelGL *panel)
{

}
