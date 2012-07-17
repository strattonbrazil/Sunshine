#include "face_util.h"
#include "sunshine.h"

namespace FaceUtil {
    FaceHit closestFace(Scene* scene, Point3 rayOrig, Vector3 rayDir, bool onlySelectedMeshes)
    {
        // taken from Fast, Minimum Storage Ray/Triangle Intersection by Moeller et al.
        FaceHit faceHit;
        float validMin = 99999999999.9f;
        float validMax = -1.0f;

        // render all the meshes
        foreach (QString meshName, scene->meshes()) {
            Mesh* mesh = scene->mesh(meshName);
            if (!mesh->isSelected() && onlySelectedMeshes)
                continue;

            QMatrix4x4 objectToWorld = mesh->objectToWorld();
            for (SunshineMesh::FaceIter f_it = mesh->_mesh->faces_begin(); f_it != mesh->_mesh->faces_end(); ++f_it) {
                OpenMesh::FaceHandle face = f_it.handle();
                QListIterator<Triangle> j = buildTriangles(mesh, face);
                while (j.hasNext()) {
                    Triangle triangle = j.next();

                    OpenMesh::Vec3f p0 = mesh->_mesh->point(mesh->_mesh->from_vertex_handle(triangle.a));
                    OpenMesh::Vec3f p1 = mesh->_mesh->point(mesh->_mesh->from_vertex_handle(triangle.b));
                    OpenMesh::Vec3f p2 = mesh->_mesh->point(mesh->_mesh->from_vertex_handle(triangle.c));

                    Point3 v0 = objectToWorld.map(Vector3(p0[0], p0[1], p0[2]));
                    Point3 v1 = objectToWorld.map(Vector3(p1[0], p1[1], p1[2]));
                    Point3 v2 = objectToWorld.map(Vector3(p2[0], p2[1], p2[2]));

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
            QMatrix4x4 objectToWorld = triangle.mesh->objectToWorld();
            OpenMesh::Vec3f p0 = triangle.mesh->_mesh->point(triangle.mesh->_mesh->from_vertex_handle(triangle.a));
            OpenMesh::Vec3f p1 = triangle.mesh->_mesh->point(triangle.mesh->_mesh->from_vertex_handle(triangle.b));
            OpenMesh::Vec3f p2 = triangle.mesh->_mesh->point(triangle.mesh->_mesh->from_vertex_handle(triangle.c));
            Point3 v0 = objectToWorld.map(Vector3(p0[0], p0[1], p0[2]));
            Point3 v1 = objectToWorld.map(Vector3(p1[0], p1[1], p1[2]));
            Point3 v2 = objectToWorld.map(Vector3(p2[0], p2[1], p2[2]));

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
                            faceHit.nearFace = triangle.mesh->_mesh->face_handle(triangle.a);
                            faceHit.nearMesh = triangle.mesh;
                            faceHit.range.setX(t);
                            validMin = t;
                        }
                        if (t > validMax) {
                            faceHit.farFace = triangle.mesh->_mesh->face_handle(triangle.a);
                            faceHit.farMesh = triangle.mesh;
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
