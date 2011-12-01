#include "vertex_util.h"
#include <algorithm>
#include "face_util.h"

namespace VertexUtil {
/*
    VertexHit closestVertex(PanelGL* panel, QMouseEvent* event, bool onlySelectedMeshes) {
        // maybe look into: http://stackoverflow.com/questions/398299/looping-in-a-spiral
        // put out kernel to see if any has been hit in the area
        QList<QPoint> offsets;
        int X = 20;
        int Y = 20;
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
                Edge* edge = faceHit.nearFace->edge();
                do {
                    Vector3 vertScreen = panel->project(objToWorld.map(edge->vert()->pos()));
                    QPoint vertPoint((int)(vertScreen.x() + .5),(int)(vertScreen.y() + .5));
                    QPoint pointQueryFlipped(event->pos().x(),panel->height()-event->pos().y());
                    QPoint cursorDiff = vertPoint - pointQueryFlipped;
                    //std::cout << pointQueryFlipped << " vs " << vertPoint << std::endl;
                    if (abs(cursorDiff.x()) < X*.5 && abs(cursorDiff.y()) < X*.5) {
                        VertexHit hit;
                        hit.mesh = faceHit.nearMesh;
                        hit.vertex = edge->vert();
                        return hit;
                    }
                    edge = edge->next();
                } while (edge != faceHit.nearFace->edge());
            }
        }
        return VertexHit();
    }
    */

    VertexHit closestVertex(PanelGL* panel, QMouseEvent* event, bool onlySelectedMeshes) {
        // maybe look into: http://stackoverflow.com/questions/398299/looping-in-a-spiral
        // put out kernel to see if any has been hit in the area
        QList<QPoint> offsets;
        int X = 20;
        int Y = 20;
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
            QPoint pointQueryFlipped(pointQuery.x(), panel->height() - pointQuery.y());
            QList<Triangle> triangles = panel->_meshGrid.trianglesByPoint(pointQueryFlipped);
            //std::cout << "# of triangles: " << triangles.size() << std::endl;

            foreach(Triangle triangle, triangles) {
                Vertex* vertices[3] = { triangle.a->vert(), triangle.b->vert(), triangle.c->vert() };
                for (int i = 0; i < 3; i++) {
                    Point3 vertScreen = triangle.screenP[i];
                    QPoint vertPoint((int)(vertScreen.x() + .5),(int)(vertScreen.y() + .5));
                    QPoint cursorFlipped(event->pos().x(),panel->height()-event->pos().y());
                    QPoint cursorDiff = vertPoint - cursorFlipped;
                    if (abs(cursorDiff.x()) < X*.5 && abs(cursorDiff.y()) < X*.5) {
                        VertexHit hit;
                        hit.mesh = triangle.a->mesh();
                        hit.vertex = vertices[i];
                        return hit;
                    }
                }
            }

            /*
            FaceUtil::FaceHit faceHit = FaceUtil::closestFace(panel->scene(), rayOrig, rayDir, onlySelectedMeshes);
            if (faceHit.nearMesh) { // see if close to vertex
                QMatrix4x4 objToWorld = faceHit.nearMesh->objectToWorld();
                Edge* edge = faceHit.nearFace->edge();
                do {
                    Vector3 vertScreen = panel->project(objToWorld.map(edge->vert()->pos()));
                    QPoint vertPoint((int)(vertScreen.x() + .5),(int)(vertScreen.y() + .5));
                    QPoint pointQueryFlipped(event->pos().x(),panel->height()-event->pos().y());
                    QPoint cursorDiff = vertPoint - pointQueryFlipped;
                    //std::cout << pointQueryFlipped << " vs " << vertPoint << std::endl;
                    if (abs(cursorDiff.x()) < X*.5 && abs(cursorDiff.y()) < X*.5) {
                        VertexHit hit;
                        hit.mesh = faceHit.nearMesh;
                        hit.vertex = edge->vert();
                        return hit;
                    }
                    edge = edge->next();
                } while (edge != faceHit.nearFace->edge());
            }
            */
        }

        return VertexHit();
    }
}

Vertex* closestVertexOnFace(Point3 rayOrig, Vector3 rayDir, Mesh* mesh, Face* face)
{
    return 0;
}
