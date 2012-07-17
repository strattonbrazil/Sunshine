#include "geometry.h"
#include "sunshine.h"

Mesh::Mesh() : Transformable()
{
    _validNormals = FALSE;
    _selected = FALSE;
    _material = 0;
    _mesh = new SunshineMesh();
}

Mesh* Mesh::buildByIndex(PrimitiveParts parts)
{
    Mesh* emptyMesh = new Mesh();
    //Mesh* emptyMesh = scene->createMesh("mesh");

    // create vertices
    QVector<OpenMesh::VertexHandle> vertices;
    for (int vertKey = 0; vertKey < parts.points.size(); vertKey++) {
        Point3 pos = parts.points[vertKey];
        vertices.append(emptyMesh->_mesh->add_vertex(SunshineMesh::Point(pos.x(), pos.y(), pos.z())));
    }

    // create the faces and edges
    for (int faceKey = 0; faceKey < parts.faces.size(); faceKey++) {
        std::vector<SunshineMesh::VertexHandle> faceVertexHandles;

        QList<int> vertIndices = parts.faces[faceKey];
        for (int i = 0; i < vertIndices.size(); i++) {
            faceVertexHandles.push_back(vertices[vertIndices[i]]);
        }

        emptyMesh->_mesh->add_face(faceVertexHandles);
    }

    return emptyMesh;
}

bool Mesh::isSelected(OpenMesh::VertexHandle vertex)
{
    return this->_mesh->data(vertex).isSelected();
}


bool Mesh::isSelected(OpenMesh::FaceHandle face)
{
    return this->_mesh->data(face).isSelected();
}

void Mesh::setSelected(OpenMesh::VertexHandle vertex, bool s)
{
    this->_mesh->data(vertex).setSelected(s);
}

void Mesh::setSelected(OpenMesh::FaceHandle face, bool s)
{
    this->_mesh->data(face).setSelected(s);
}

Vector3 Mesh::normal(OpenMesh::HalfedgeHandle edge)
{
    SunshineMesh::Normal n(_mesh->normal(edge));
    return Vector3(n[0], n[1], n[2]);
}

const int Mesh::numTriangles()
{
    int count = 0;

    for (SunshineMesh::FaceIter f_it = _mesh->faces_begin(); f_it != _mesh->faces_end(); ++f_it) {
        OpenMesh::FaceHandle face = f_it.handle();
        for (SunshineMesh::FaceHalfedgeIter fh_it = _mesh->fh_iter(face); fh_it; ++fh_it) {
            OpenMesh::HalfedgeHandle edge = fh_it.handle();
            count++;
        }
        count -= 2;
    }
    return count;
}

Material* Mesh::material()
{
    if (_material == 0)
        _material = SunshineUi::activeScene()->defaultMaterial();
    return _material;
}

void Mesh::setMaterial(Material* material)
{
    _material = material;
}

QMatrix4x4 Mesh::normalToWorld()
{
    return _rotate.matrix();
}

void Mesh::validateNormals()
{

}

Box3D Mesh::worldBounds()
{
    Box3D bounds;
    QMatrix4x4 objToWorld = objectToWorld();
    for (SunshineMesh::VertexIter v_it = _mesh->vertices_begin(); v_it != _mesh->vertices_end(); ++v_it) {
        OpenMesh::VertexHandle vertex = v_it.handle();
        OpenMesh::Vec3f pos = _mesh->point(vertex);
        bounds.includePoint(objToWorld.map(Point3(pos[0], pos[1], pos[2])));
    }
    return bounds;
}

QListIterator<Triangle> buildTriangles(Mesh* mesh, OpenMesh::FaceHandle face)
{
    // this function should eventually take concavity and warping into account
    QList<Triangle> triangles;

    OpenMesh::HalfedgeHandle firstEdge;
    OpenMesh::HalfedgeHandle secondEdge;
    for (SunshineMesh::FaceHalfedgeIter fh_it = mesh->_mesh->fh_iter(face); fh_it; ++fh_it)
    {
        OpenMesh::HalfedgeHandle edge = fh_it.handle();
        if (!firstEdge.is_valid())
            firstEdge = edge;
        else if (!secondEdge.is_valid())
            secondEdge = edge;
        else {
            OpenMesh::HalfedgeHandle thirdEdge = edge;
            OpenMesh::VertexHandle v1 = mesh->_mesh->from_vertex_handle(firstEdge);
            OpenMesh::VertexHandle v2 = mesh->_mesh->from_vertex_handle(secondEdge);
            OpenMesh::VertexHandle v3 = mesh->_mesh->from_vertex_handle(thirdEdge);

            triangles << Triangle(mesh, firstEdge, secondEdge, thirdEdge);
        }
    }

    return QListIterator<Triangle>(triangles);
}

