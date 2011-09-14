#ifndef VERTEX_UTIL_H
#define VERTEX_UTIL_H

#include "geometry.h"

namespace VertexUtil {
    VertexP closestVertex(PanelGL* panel, QMouseEvent* event, bool onlySelectedMeshes);
    VertexP closestVertexOnFace(Point3 rayOrig, Vector3 rayDir, MeshP mesh, FaceP face);
};

#endif // VERTEX_UTIL_H
