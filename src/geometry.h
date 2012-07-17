#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QHash>
#include <QString>
#include <boost/shared_ptr.hpp>
#include "primitive.h"
#include "material.h"

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>

#include "scene.h"

//QScriptValue Mesh_buildByIndex(QScriptContext *context, QScriptEngine *engine);

struct MyTraits : public OpenMesh::DefaultTraits
{
  // store barycenter of neighbors in this member
  VertexTraits
  {
  private:
    Point  cog_;
    bool _selected;
  public:
    VertexT() : cog_( Point(0.0f, 0.0f, 0.0f ) ) { }
    const Point& cog() const { return cog_; }
    void set_cog(const Point& _p) { cog_ = _p; }
    void setSelected(bool s) { _selected = s; }
    bool isSelected() { return _selected; }
  };

  FaceTraits
  {
      private:
      bool _selected;
      public:
      void setSelected(bool s) { _selected = s; }
      bool isSelected() { return _selected; }
  };
};
typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> SunshineMesh;

class Mesh;

class Triangle
{
public:
                              Triangle(Mesh* mesh,
                                       OpenMesh::HalfedgeHandle e1,
                                       OpenMesh::HalfedgeHandle e2,
                                       OpenMesh::HalfedgeHandle e3) : mesh(mesh), a(e1),b(e2),c(e3) {}
    Mesh* mesh;
    OpenMesh::HalfedgeHandle  a,b,c;
    Point3                     screenP[3];
};

class Mesh : public Transformable
{
    Q_OBJECT
public:

                                 Mesh();
                                 //Mesh(QHash<int,Vertex*> vertices, QHash<int,Edge*> edges, QHash<int,Face*> faces);
    int                          assetType() { return AssetType::MESH_ASSET; }
    static Mesh*                 buildByIndex(PrimitiveParts parts);
    const int                    numTriangles();
    const int                    numVertices() { return _mesh->n_vertices(); }
    OpenMesh::FaceHandle         face(int key) { return _mesh->face_handle(key); }
    OpenMesh::EdgeHandle         edge(int key) { return _mesh->edge_handle(key); }
    OpenMesh::VertexHandle       vertex(int key) { return _mesh->vertex_handle(key); }
    Vector3                      normal(OpenMesh::HalfedgeHandle edge);
    bool                         isSelected(OpenMesh::VertexHandle vertex);
    bool                         isSelected(OpenMesh::FaceHandle face);
    void                         setSelected(OpenMesh::VertexHandle vertex, bool s);
    void                         setSelected(OpenMesh::FaceHandle face, bool s);
    Material*                    material();
    void                         setMaterial(Material* material);
    void                         validateNormals();
    bool                         isSelected() { return _selected; }
    void                         setSelected(bool s) { _selected = s; }
    QMatrix4x4                   normalToWorld();
    Box3D                        worldBounds();
    SunshineMesh*                _mesh;
private:
    bool                         _validNormals;
    bool                         _selected;
    Material*                    _material;
    Q_DISABLE_COPY(Mesh)
    //QHash<QString,QHash<QPoint,QVariantList> > _faceVertAttributes;
};

class MeshImporter
{
public:
    QString extension() { return QString("(not implemented)"); }
    void importMesh(QString fileName) {  }
};

QListIterator<Triangle> buildTriangles(Mesh* mesh, OpenMesh::FaceHandle face);

#endif // GEOMETRY_H
