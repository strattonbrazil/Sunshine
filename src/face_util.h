    #ifndef FACE_UTIL_H
#define FACE_UTIL_H

#include "geometry.h"

namespace FaceUtil {
    class FaceHit {
    public:
        FaceHit() {
            nearFace = 0;
            farFace = 0;
            nearMesh = 0;
            farMesh = 0;
        }

        Face* nearFace;
        Face* farFace;
        Mesh* nearMesh;
        Mesh* farMesh;
        Vector3 range;
    };

    FaceHit closestFace(Scene* scene, Point3 rayOrig, Vector3 rayDir, bool onlySelectedMeshes);
    FaceHit closestFace(QList<Triangle> triangles, Point3 rayOrig, Vector3 rayDir, bool onlySelectedMeshes);
}


#endif // FACE_UTIL_H
