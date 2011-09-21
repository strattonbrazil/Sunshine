    #ifndef FACE_UTIL_H
#define FACE_UTIL_H

#include "geometry.h"

namespace FaceUtil {
    class FaceHit {
    public:
        FaceP nearFace;
        FaceP farFace;
        MeshP nearMesh;
        MeshP farMesh;
        Vector3 range;
    };

    FaceHit closestFace(SceneP scene, Point3 rayOrig, Vector3 rayDir, bool onlySelectedMeshes);
    FaceHit closestFace(QList<Triangle> triangles, Point3 rayOrig, Vector3 rayDir, bool onlySelectedMeshes);
}


#endif // FACE_UTIL_H
