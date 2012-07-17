    #ifndef FACE_UTIL_H
#define FACE_UTIL_H

#include "geometry.h"

namespace FaceUtil {
    class FaceHit {
    public:
        FaceHit() {
            nearMesh = 0;
            farMesh = 0;
        }

        OpenMesh::FaceHandle nearFace;
        OpenMesh::FaceHandle farFace;
        Mesh* nearMesh;
        Mesh* farMesh;
        Vector3 range;
    };

    FaceHit closestFace(Scene* scene, Point3 rayOrig, Vector3 rayDir, bool onlySelectedMeshes);
    FaceHit closestFace(QList<Triangle> triangles, Point3 rayOrig, Vector3 rayDir, bool onlySelectedMeshes);
}


#endif // FACE_UTIL_H
