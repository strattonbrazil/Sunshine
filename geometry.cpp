#include "geometry.h"

Edge::Edge(int meshKey, int vertexKey, int faceKey, int edgeKey)
{
    _meshKey = meshKey;
    _vertexKey = vertexKey;
    _edgeKey = edgeKey;
}

MeshP Edge::mesh() { return Register::mesh(_meshKey); }

Face::Face(int meshKey, int faceKey)
{
    _meshKey = meshKey;
    _faceKey = faceKey;
}

Mesh::Mesh(int key, QString name, QHash<int,VertexP> vertices, QHash<int,EdgeP> edges, QHash<int,FaceP> faces)
{
    _key = key;
    _name = name;
    _vertices = vertices;
    _edges = edges;
    _faces = faces;
}

const int Mesh::numTriangles()
{
    int count = 0;
    QHashIterator<int, FaceP> i(_faces);
    while (i.hasNext()) {
         i.next();
         FaceP face = i.value();
         EdgeP edge = face->edge();
         do {
             count++;
             edge = edge->next();
         } while (edge != face->edge());
         count -= 2;
         //cout << i.key() << ": " << i.value() << endl;
    }
    return count;
}
