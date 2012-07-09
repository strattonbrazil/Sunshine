#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QHash>
#include <QString>
#include <boost/shared_ptr.hpp>
#include "primitive.h"
#include "material.h"

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>

class Vertex;
class Edge;
class Face;
class Mesh;
class Scene;

#include "scene.h"

//QScriptValue Mesh_buildByIndex(QScriptContext *context, QScriptEngine *engine);

struct MyTraits : public OpenMesh::DefaultTraits
{
  // store barycenter of neighbors in this member
  VertexTraits
  {
  private:
    Point  cog_;
  public:
    VertexT() : cog_( Point(0.0f, 0.0f, 0.0f ) ) { }
    const Point& cog() const { return cog_; }
    void set_cog(const Point& _p) { cog_ = _p; }
  };
};
typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> SunshineMesh;

class Mesh : public Transformable
{
    Q_OBJECT
public:

                                 Mesh();
                                 Mesh(QHash<int,Vertex*> vertices, QHash<int,Edge*> edges, QHash<int,Face*> faces);
    int                          assetType() { return AssetType::MESH_ASSET; }
    static Mesh*                 buildByIndex(PrimitiveParts parts);
    const int                    numTriangles();
    const int                    numVertices() { return _vertices.size(); }
    Face*                        face(int key) { return _faces[key]; }
    Edge*                        edge(int key) { return _edges[key]; }
    Vertex*                      vert(int key) { return _vertices[key]; }
    void                         computeEdgePairs();
    QHashIterator<int,Face*>     faces() { return QHashIterator<int,Face*>(_faces); }
    QHashIterator<int,Vertex*>   vertices() { return QHashIterator<int,Vertex*>(_vertices); }
    Material*                    material();
    void                         setMaterial(Material* material);
    void                         validateNormals();
    bool                         isSelected() { return _selected; }
    void                         setSelected(bool s) { _selected = s; }
    QMatrix4x4                   normalToWorld();
    Box3D                        worldBounds();
private:
    QHash<int,Vertex*>           _vertices;
    QHash<int,Edge*>             _edges;
    QHash<int,Face*>             _faces;
    bool                         _validNormals;
    bool                         _selected;
    Material*                    _material;
    SunshineMesh*                _mesh;
    Q_DISABLE_COPY(Mesh)
    //QHash<QString,QHash<QPoint,QVariantList> > _faceVertAttributes;
};

class Vertex
{
public:
                          Vertex(Mesh* mesh, int key, Point3 point);
    Edge*                 edge() { return _mesh->edge(_edgeKey); }
    void                  setEdge(Edge* e);
    int                   key() { return _key; }
    Point3                pos() { return _point; }
    void                  setPos(Point3 p) { _point = p; }
    bool                  isSelected() { return _selected; }
    void                  setSelected(bool s) { _selected = s; }
private:
    Mesh*                 _mesh;
    int                   _key;
    Point3                _point;
    int                   _edgeKey;
    bool                  _selected;

};

class Edge
{
public:
                          Edge(Mesh* mesh, int vertexKey, int faceKey, int edgeKey);
    int                   key() { return _edgeKey; }
    void                  setNext(Edge* e) { _nextKey = e->key(); }
    void                  setPrev(Edge* e) { _prevKey = e->key(); }
    void                  setPair(Edge* e) { _pairKey = e->key(); }
    Edge*                 next() { return mesh()->edge(_nextKey); }
    Edge*                 prev() { return mesh()->edge(_prevKey); }
    Edge*                 pair() { return mesh()->edge(_pairKey); }
    Face*                 face() { return mesh()->face(_faceKey); }
    Mesh*                 mesh();// { return Register::mesh(_meshKey); }
    Vertex*               vert() { return mesh()->vert(_vertexKey); }
    Vector3               normal() { return _normal; }
    void                  setNormal(Vector3 n) { _normal = n; }
    Vector3               flatNormal();
private:
    Mesh*                 _mesh;
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
                          Triangle(Edge* e1, Edge* e2, Edge* e3) : a(e1),b(e2),c(e3) {}
    Edge                  *a,*b,*c;
    Point3                screenP[3];
};

class Face
{
public:
                              Face(Mesh* mesh, int faceKey);
    int                       key() { return _faceKey; }
    Edge*                     edge();
    void                      setEdge(Edge* e) { _edgeKey = e->key(); }
    QListIterator<Triangle>   buildTriangles();
    void                      calculateNormal();
    bool                      isSelected() { return _selected; }
    void                      setSelected(bool s) { _selected = s; }
private:
    Mesh*                     _mesh;
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
