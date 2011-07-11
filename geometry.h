#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QHash>
#include <QString>
#include <QSharedPointer>
#include "primitive.h"

class Vertex;
class Edge;
class Face;
class Mesh;
typedef QSharedPointer<Vertex> VertexP;
typedef QSharedPointer<Edge> EdgeP;
typedef QSharedPointer<Face> FaceP;
typedef QSharedPointer<Mesh> MeshP;

#include "register.h"

class Vertex
{
public:
                          Vertex(int meshKey, int key, Point3 point);
    void                  setEdge(EdgeP e);
    int                   key() { return _key; }
    Point3                pos() { return _point; }
    bool                  isSelected() { return _selected; }
    void                  setSelected(bool s) { _selected = s; }
private:
    int                   _meshKey;
    int                   _key;
    Point3                _point;
    int                   _edgeKey;
    bool                  _selected;
};

class Mesh : public Transformable
{
public:
                                 Mesh(int key, QString name);
                                 Mesh(int key, QString name, QHash<int,VertexP> vertices, QHash<int,EdgeP> edges, QHash<int,FaceP> faces);
    static void                  buildByIndex(PrimitiveParts parts);
    const int                    numTriangles();
    const int                    numVertices() { return _vertices.size(); }
    EdgeP                        edge(int key) { return _edges[key]; }
    VertexP                      vert(int key) { return _vertices[key]; }
    int                          key() { return _key; }
    QString                      name() { return _name; }
    void                         computeEdgePairs();
    QHashIterator<int,FaceP>     faces() { return QHashIterator<int,FaceP>(_faces); }
    QHashIterator<int,VertexP>   vertices() { return QHashIterator<int,VertexP>(_vertices); }
    void                         validateNormals();
    bool                         isSelected() { return _selected; }
    void                         setSelected(bool s) { _selected = s; }
private:
    int                          _key;
    QString                      _name;
    QHash<int,VertexP>           _vertices;
    QHash<int,EdgeP>             _edges;
    QHash<int,FaceP>             _faces;
    bool                         _validNormals;
    bool                         _selected;
};

class Edge
{
public:
                          Edge(int meshKey, int vertexKey, int faceKey, int edgeKey);
    int                   key() { return _edgeKey; }
    void                  setNext(EdgeP e) { _nextKey = e->key(); }
    void                  setPrev(EdgeP e) { _prevKey = e->key(); }
    void                  setPair(EdgeP e) { _pairKey = e->key(); }
    EdgeP                 next() {
        //std::cout << mesh()->name().toStdString() << std::endl;
        //std::cout << mesh()->edge(0) << std::endl;
        return mesh()->edge(_nextKey);
    }
    EdgeP                 prev() { return mesh()->edge(_prevKey); }
    EdgeP                 pair() { return mesh()->edge(_pairKey); }
    MeshP                 mesh();// { return Register::mesh(_meshKey); }
    VertexP               vert() { return mesh()->vert(_vertexKey); }
    Vector3               normal() { return _normal; }
    void                  setNormal(Vector3 n) { _normal = n; }
private:
    int                   _meshKey;
    int                   _vertexKey;
    int                   _faceKey;
    int                   _edgeKey;
    int                   _nextKey;
    int                   _prevKey;
    int                   _pairKey;
    Vector3               _normal;

};

class Triangle
{
public:
                          Triangle(EdgeP e1, EdgeP e2, EdgeP e3) : a(e1),b(e2),c(e3) {}
    EdgeP               a,b,c;
};

class Face
{
public:
                              Face(int meshKey, int faceKey);
    int                       key() { return _meshKey; }
    EdgeP                     edge();
    void                      setEdge(EdgeP e) { _edgeKey = e->key(); }
    QListIterator<Triangle>   buildTriangles();
    void                      calculateNormal();
    bool                      isSelected() { return _selected; }
private:
    int                       _meshKey;
    int                       _faceKey;
    int                       _edgeKey;
    bool                      _selected;
};


#endif // GEOMETRY_H
