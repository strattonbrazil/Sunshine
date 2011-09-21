#ifndef VERTEX_UTIL_H
#define VERTEX_UTIL_H

#include "geometry.h"

namespace VertexUtil {
    class VertexHit {
    public:
        VertexP vertex;
        MeshP mesh;
    };

    //VertexHit closestVertex(PanelGL* panel, QMouseEvent* event, bool onlySelectedMeshes);
    VertexP closestVertexOnFace(Point3 rayOrig, Vector3 rayDir, MeshP mesh, FaceP face);
    VertexHit closestVertex(PanelGL* panel, QMouseEvent* event, bool onlySelectedMeshes);
};

#endif // VERTEX_UTIL_H
