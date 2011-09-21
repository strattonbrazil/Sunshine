#include "face_util.h"
#include "sunshine.h"

namespace FaceUtil {
    FaceHit closestFace(SceneP scene, Point3 rayOrig, Vector3 rayDir, bool onlySelectedMeshes)
    {
        // taken from Fast, Minimum Storage Ray/Triangle Intersection by Moeller et al.
        FaceHit faceHit;
        float validMin = 99999999999.9f;
        float validMax = -1.0f;

        // render all the meshes
        QHashIterator<int,MeshP> meshes = scene->meshes();
        while (meshes.hasNext()) {
            meshes.next();
            int meshKey = meshes.key();
            MeshP mesh = meshes.value();
            if (!mesh->isSelected() && onlySelectedMeshes)
                continue;

            QMatrix4x4 objectToWorld = mesh->objectToWorld();
            QHashIterator<int,FaceP> i = mesh->faces();
            while (i.hasNext()) {
                i.next();
                FaceP face = i.value();
                QListIterator<Triangle> j = face->buildTriangles();
                while (j.hasNext()) {
                    Triangle triangle = j.next();

                    Point3 v0 = objectToWorld.map(triangle.a->vert()->pos());
                    Point3 v1 = objectToWorld.map(triangle.b->vert()->pos());
                    Point3 v2 = objectToWorld.map(triangle.c->vert()->pos());

                    Vector3 edge1 = v1 - v0;
                    Vector3 edge2 = v2 - v0;

                    Vector3 pvec = Vector3::crossProduct(rayDir, edge2);
                    float det = Vector3::dotProduct(edge1, pvec);

                    // if ray not aligned with triangle
                    if (!(det > -0.00001f && det < 0.00001f)) {
                        float invDet = 1.0f / det;

                        Vector3 tvec = rayOrig - v0;

                        float u = Vector3::dotProduct(tvec, pvec) * invDet;
                        if (!(u < 0.0f || u > 1.0f)) {
                            Vector3 qvec = Vector3::crossProduct(tvec, edge1);

                            float v = Vector3::dotProduct(rayDir, qvec) * invDet;
                            if (!(v < 0.0f || u+v > 1)) { // intersection!
                                float t = Vector3::dotProduct(edge2, qvec) * invDet;

                                if (t < validMin && t > 0) {
                                    faceHit.nearFace = face;
                                    faceHit.nearMesh = mesh;
                                    faceHit.range.setX(t);
                                    validMin = t;
                                }
                                if (t > validMax) {
                                    faceHit.farFace = face;
                                    faceHit.farMesh = mesh;
                                    faceHit.range.setY(t);
                                    validMax = t;
                                }
                            }
                        }
                    } else {
                        //println("ray aligned with triangle")
                    }
                }
            }
        }

        return faceHit;
    }

    FaceHit closestFace(QList<Triangle> triangles, Point3 rayOrig, Vector3 rayDir, bool onlySelectedMeshes)
    {
        FaceHit faceHit;
        float validMin = 99999999999.9f;
        float validMax = -1.0f;

        foreach (Triangle triangle, triangles) {
            QMatrix4x4 objectToWorld = triangle.a->mesh()->objectToWorld();
            Point3 v0 = objectToWorld.map(triangle.a->vert()->pos());
            Point3 v1 = objectToWorld.map(triangle.b->vert()->pos());
            Point3 v2 = objectToWorld.map(triangle.c->vert()->pos());

            Vector3 edge1 = v1 - v0;
            Vector3 edge2 = v2 - v0;

            Vector3 pvec = Vector3::crossProduct(rayDir, edge2);
            float det = Vector3::dotProduct(edge1, pvec);

            // if ray not aligned with triangle
            if (!(det > -0.00001f && det < 0.00001f)) {
                float invDet = 1.0f / det;

                Vector3 tvec = rayOrig - v0;

                float u = Vector3::dotProduct(tvec, pvec) * invDet;
                if (!(u < 0.0f || u > 1.0f)) {
                    Vector3 qvec = Vector3::crossProduct(tvec, edge1);

                    float v = Vector3::dotProduct(rayDir, qvec) * invDet;
                    if (!(v < 0.0f || u+v > 1)) { // intersection!
                        float t = Vector3::dotProduct(edge2, qvec) * invDet;

                        if (t < validMin && t > 0) {
                            faceHit.nearFace = triangle.a->face();
                            faceHit.nearMesh = triangle.a->mesh();
                            faceHit.range.setX(t);
                            validMin = t;
                        }
                        if (t > validMax) {
                            faceHit.farFace = triangle.a->face();
                            faceHit.farMesh = triangle.a->mesh();
                            faceHit.range.setY(t);
                            validMax = t;
                        }
                    }
                }
            } else {
                //println("ray aligned with triangle")
            }
        }

        return faceHit;
    }
}
