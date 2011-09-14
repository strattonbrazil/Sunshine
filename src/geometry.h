#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QHash>
#include <QString>
#include <QSharedPointer>
#include <boost/shared_ptr.hpp>
#include "primitive.h"

class Vertex;
class Edge;
class Face;
class Mesh;
class Scene;
typedef boost::shared_ptr<Scene> SceneP;
typedef QSharedPointer<Vertex> VertexP;
typedef QSharedPointer<Edge> EdgeP;
typedef QSharedPointer<Face> FaceP;
typedef QSharedPointer<Mesh> MeshP;

#include "scene.h"

class Vertex
{
public:
                          Vertex(MeshP mesh, int key, Point3 point);
    void                  setEdge(EdgeP e);
    int                   key() { return _key; }
    Point3                pos() { return _point; }
    bool                  isSelected() { return _selected; }
    void                  setSelected(bool s) { _selected = s; }
private:
    MeshP                 _mesh;
    int                   _key;
    Point3                _point;
    int                   _edgeKey;
    bool                  _selected;

};

class Mesh : public Transformable
{
public:
                                 Mesh() {}
                                 Mesh(SceneP scene, int key, QString name);
                                 Mesh(SceneP scene, int key, QString name, QHash<int,VertexP> vertices, QHash<int,EdgeP> edges, QHash<int,FaceP> faces);
    static void                  buildByIndex(SceneP scene, PrimitiveParts parts);
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
    QMatrix4x4                   normalToWorld();
private:
    int                          _key;
    QString                      _name;
    QHash<int,VertexP>           _vertices;
    QHash<int,EdgeP>             _edges;
    QHash<int,FaceP>             _faces;
    bool                         _validNormals;
    bool                         _selected;
    SceneP                       _scene;
};

class Edge
{
public:
                          Edge(MeshP mesh, int vertexKey, int faceKey, int edgeKey);
    int                   key() { return _edgeKey; }
    void                  setNext(EdgeP e) { _nextKey = e->key(); }
    void                  setPrev(EdgeP e) { _prevKey = e->key(); }
    void                  setPair(EdgeP e) { _pairKey = e->key(); }
    EdgeP                 next() { return mesh()->edge(_nextKey); }
    EdgeP                 prev() { return mesh()->edge(_prevKey); }
    EdgeP                 pair() { return mesh()->edge(_pairKey); }
    MeshP                 mesh();// { return Register::mesh(_meshKey); }
    VertexP               vert() { return mesh()->vert(_vertexKey); }
    Vector3               normal() { return _normal; }
    void                  setNormal(Vector3 n) { _normal = n; }
private:
    MeshP                 _mesh;
    int                   _vertexKey;
    int                   _faceKey;
    int                   _edgeKey;
    int                   _nextKey;
    int                   _prevKey;
    int                   _pairKey;
    Vector3               _normal;
    bool                  _selected;
};

class Triangle
{
public:
                          Triangle(EdgeP e1, EdgeP e2, EdgeP e3) : a(e1),b(e2),c(e3) {}
    EdgeP                 a,b,c;
};

class Face
{
public:
                              Face(MeshP mesh, int faceKey);
    int                       key() { return _faceKey; }
    EdgeP                     edge();
    void                      setEdge(EdgeP e) { _edgeKey = e->key(); }
    QListIterator<Triangle>   buildTriangles();
    void                      calculateNormal();
    bool                      isSelected() { return _selected; }
private:
    MeshP                     _mesh;
    int                       _faceKey;
    int                       _edgeKey;
    bool                      _selected;
};

class MeshImporter
{
public:
    QString extension() { return QString("(not implemented)"); }
    void importMesh(QString fileName) {  }
};

#endif // GEOMETRY_H
