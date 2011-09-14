#include "vertex_util.h"
#include <algorithm>
#include "face_util.h"

namespace VertexUtil {
    VertexP closestVertex(PanelGL* panel, QMouseEvent* event, bool onlySelectedMeshes) {
        // maybe look into: http://stackoverflow.com/questions/398299/looping-in-a-spiral
        // put out kernel to see if any has been hit in the area
        QList<QPoint> offsets;
        int X = 6;
        int Y = 6;
        int x=0, y=0;
        int dx = 0;
        int dy = -1;
        for (int i = 0; i < std::max(X,Y)*std::max(X,Y); i++) {
            if ((-X/2 < x && x <= X/2) && (-Y/2 < y && y <= Y/2))
                offsets << QPoint(x,y);
            if (x == y || (x < 0 && x == -y) || (x > 0 && x == 1-y)) {
                int tmp = dx;
                dx = -dy;
                dy = tmp;
            }
            x += dx;
            y += dy;
        }

        foreach(QPoint offset, offsets) {
            QPoint pointQuery = event->pos() + offset;
            Point3 rayOrig = panel->camera()->eye();
            Vector3 rayDir = panel->computeRayDirection(pointQuery);
            FaceUtil::FaceHit faceHit = FaceUtil::closestFace(panel->scene(), rayOrig, rayDir, onlySelectedMeshes);
            if (faceHit.nearMesh) { // see if close to vertex
                QMatrix4x4 objToWorld = faceHit.nearMesh->objectToWorld();
                EdgeP edge = faceHit.nearFace->edge();
                do {
                    Vector3 vertScreen = panel->project(objToWorld.map(edge->vert()->pos()));
                    QPoint vertPoint((int)(vertScreen.x() + .5),(int)(vertScreen.y() + .5));
                    QPoint pointQueryFlipped(pointQuery.x(),panel->height()-pointQuery.y());
                    //std::cout << pointQueryFlipped << " vs " << vertPoint << std::endl;
                    if ((vertPoint - pointQueryFlipped).manhattanLength() < 3)
                        return edge->vert();
                    edge = edge->next();
                } while (edge != faceHit.nearFace->edge());
            }
        }
        return VertexP();
    }
}

VertexP closestVertexOnFace(Point3 rayOrig, Vector3 rayDir, MeshP mesh, FaceP face)
{
    return VertexP();
}
