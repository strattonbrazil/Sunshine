#include "cursor_tools.h"
#include <cmath>

void DrawBoxTool::mousePressed(PanelGL *panel, QMouseEvent *event)
{
    Point3 rayOrig = panel->camera()->eye();
    Vector3 rayDir = panel->computeRayDirection(event->pos());

    //QList<Triangle> triangles = _meshGrid.trianglesByPoint(QPoint(event->pos().x(), height()-event->pos().y()));
    //FaceUtil::FaceHit faceHit = FaceUtil::closestFace(triangles, rayOrig, rayDir, false);

    // drawing on "workspace" or mesh
    if (FALSE) { // drawing on mesh

    } else {
        // figure out where it hit plane
        float t = PlaneUtil::intersect(Vector3(0,1,0), Point3(0,0,0), rayDir, rayOrig);
        if (t > 0) {
            // make mesh at that location
            _pick = rayOrig + rayDir * t;
            _current = _pick;
            _plane = Mesh::buildByIndex(primitive::planePrimitive(10,10,1,1));
            updateWorkspacePlane();
        }
    }
}

void DrawBoxTool::mouseDragged(PanelGL *panel, QMouseEvent *event)
{
    if (_plane) {
        Point3 rayOrig = panel->camera()->eye();
        Vector3 rayDir = panel->computeRayDirection(event->pos());
        float t = PlaneUtil::intersect(Vector3(0,1,0), Point3(0,0,0), rayDir, rayOrig);
        if (t > 0) {
            _current = rayOrig + rayDir * t;
            updateWorkspacePlane();
        }
    }
}

void DrawBoxTool::mouseReleased(PanelGL *panel, QMouseEvent *event)
{
    const float EPS = 0.1;
    float xDist = std::fabs(_current.x() - _pick.x());
    float zDist = std::fabs(_current.z() - _pick.z());
    if (xDist < EPS || zDist < EPS) {
        std::cout << "plane too small" << std::endl;
        //panel->scene()->deleteMesh(_plane->name());
        _plane = 0;
    }
}

void DrawBoxTool::updateWorkspacePlane()
{
    // update the plane position based on the pick and current values
    float minX = std::min(_pick.x(), _current.x());
    float maxX = std::max(_pick.x(), _current.x());
    float minZ = std::min(_pick.z(), _current.z());
    float maxZ = std::max(_pick.z(), _current.z());

    Face* face = _plane->face(0);
    face->edge()->vert()->setPos(Point3(maxX,0,maxZ));
    face->edge()->next()->vert()->setPos(Point3(maxX,0,minZ));
    face->edge()->next()->next()->vert()->setPos(Point3(minX,0,minZ));
    face->edge()->next()->next()->next()->vert()->setPos(Point3(minX,0,maxZ));
}
