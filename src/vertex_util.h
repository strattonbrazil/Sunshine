#ifndef VERTEX_UTIL_H
#define VERTEX_UTIL_H

#include "geometry.h"

namespace VertexUtil {
    class VertexHit {
    public:
        OpenMesh::VertexHandle vertex;
        Mesh* mesh;
    };

    //VertexHit closestVertex(PanelGL* panel, QMouseEvent* event, bool onlySelectedMeshes);
    //Vertex* closestVertexOnFace(Point3 rayOrig, Vector3 rayDir, Mesh* mesh, Face* face);
    VertexHit closestVertex(PanelGL* panel, QMouseEvent* event, bool onlySelectedMeshes);
};

#endif // VERTEX_UTIL_H
