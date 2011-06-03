#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QHash>
#include <QString>
#include <QSharedPointer>

class Vertex;
class Edge;
class Face;
class Mesh;
typedef QSharedPointer<Vertex> VertexP;
typedef QSharedPointer<Edge> EdgeP;
typedef QSharedPointer<Face> FaceP;
typedef QSharedPointer<Mesh> MeshP;

#include "register.h"

class Vertex;

class Mesh
{
public:
                          Mesh(int key, QString name, QHash<int,VertexP> vertices, QHash<int,EdgeP> edges, QHash<int,FaceP> faces);
    const int             numTriangles();
    EdgeP                 edge(int key) { return _edges[key]; }
private:
    int                   _key;
    QString               _name;
    QHash<int,VertexP>     _vertices;
    QHash<int,EdgeP>       _edges;
    QHash<int,FaceP>       _faces;
};

class Edge
{
public:
                          Edge(int meshKey, int vertexKey, int faceKey, int edgeKey);
    EdgeP                 next() { return mesh()->edge(_next); }
    MeshP                 mesh();// { return Register::mesh(_meshKey); }
private:
    int                   _meshKey;
    int                   _vertexKey;
    int                   _edgeKey;
    int                   _next;

};

class Face
{
public:
                          Face(int meshKey, int faceKey);
    EdgeP                 edge() { return EdgeP(NULL); }
private:
    int                   _meshKey;
    int                   _faceKey;
};



#endif // GEOMETRY_H
