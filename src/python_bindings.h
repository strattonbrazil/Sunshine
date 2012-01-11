#ifndef PYTHON_BINDINGS_H
#define PYTHON_BINDINGS_H

/*
#include <PythonQt.h>

void createPythonBindings();

#include "scene.h"
#include "primitive.h"

class PythonQtWrapper_PrimitiveParts : public QObject
{
    Q_OBJECT
public slots:
    PrimitiveParts* new_PrimitiveParts() { return new PrimitiveParts(); }
    void delete_PrimitiveParts(PrimitiveParts* obj) { delete obj; }
    void setNumVertices(PrimitiveParts* obj, int numVertices)
    {
        obj->points.resize(numVertices);
    }
    void setVertex(PrimitiveParts* obj, int index, QVector3D vertex)
    {
        obj->points[index] = vertex;
    }
    void setNumFaces(PrimitiveParts* obj, int numFaces)
    {
        obj->faces.resize(numFaces);
    }
    void setFace(PrimitiveParts* obj, int index, QList<int> indices)
    {
        obj->faces[index] = indices;
    }


    void setPoints(PrimitiveParts* obj, QList<QVector3D> points)
    {
        std::cout << "setting points" << std::endl;
        //obj->points = points;
    }
};

class PythonQtWrapper_Mesh : public QObject
{
    Q_OBJECT
public slots:
    Mesh* new_Mesh() { return new Mesh(); }
    void delete_Mesh(Mesh* obj) { delete obj; }
    Mesh* static_Mesh_buildByIndex(PrimitiveParts* parts) {
        return Mesh::buildByIndex(*parts);
    }
};

*/
#endif // PYTHON_BINDINGS_H
