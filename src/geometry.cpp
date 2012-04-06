#include "geometry.h"
#include "sunshine.h"

#if 0
QScriptValue Mesh_buildByIndex(QScriptContext *context, QScriptEngine *engine)
{
    std::cout << "building mesh by index" << std::endl;

    //MeshPointer meshPointer = qscriptvalue_cast<MeshPointer>(context->thisObject());
    //if (!meshPointer)
      //  return context->throwError(QScriptContext::TypeError, "this object is not a Mesh");

    PrimitivePartsPointer ppPointer = qscriptvalue_cast<PrimitivePartsPointer>(context->argument(0));
    Mesh* mesh = Mesh::buildByIndex(*ppPointer);

    return engine->newQObject(mesh);

    // set faces here
    /*
    QScriptValue faces = context->argument(0);
    const int numFaces = faces.property("length").toInteger();
    ppPointer->faces.resize(numFaces);
    for (int i = 0; i < numFaces; i++) {
        QVariantList faceList = faces.property(i).toVariant().toList();
        for (int j = 0; j < faceList.size(); j++) {
            ppPointer->faces[i].append(faceList[j].toInt());
        }
    }
    */

    return QScriptValue();
}
#endif


Vertex::Vertex(Mesh* mesh, int key, Point3 point)
{
    _mesh = mesh;
    _key = key;
    _point = point;
    _selected = FALSE;
}

void Vertex::setEdge(Edge* e) { _edgeKey = e->key(); }

Edge::Edge(Mesh* mesh, int vertexKey, int faceKey, int edgeKey)
{
    _mesh = mesh;
    _vertexKey = vertexKey;
    _faceKey = faceKey;
    _edgeKey = edgeKey;
    _selected = FALSE;
}

Vector3 Edge::flatNormal()
{
    Vector3 a = next()->vert()->pos() - vert()->pos();
    a.normalize();
    Vector3 b = vert()->pos() - prev()->vert()->pos();
    b.normalize();
    return Vector3::crossProduct(b, a);
}

Mesh* Edge::mesh() {
    return _mesh;
}

Face::Face(Mesh* mesh, int faceKey)
{
    _mesh = mesh;
    _faceKey = faceKey;
    _edgeKey = -1;
    _selected = FALSE;
}

QListIterator<Triangle> Face::buildTriangles()
{
    // this function should eventually take concavity and warping into account
    QList<Triangle> triangles;

    Edge* tmpEdge = edge()->next();
    do {
        triangles << Triangle(edge(), tmpEdge, tmpEdge->next());
        tmpEdge = tmpEdge->next();
    } while (tmpEdge != edge()->prev());

    return QListIterator<Triangle>(triangles);
}

Mesh::Mesh() : Transformable()
{
    _validNormals = FALSE;
    _selected = FALSE;
    _material = 0;
}

Mesh::Mesh(QHash<int,Vertex*> vertices, QHash<int,Edge*> edges, QHash<int,Face*> faces) : Transformable()
{
    _vertices = vertices;
    _edges = edges;
    _faces = faces;
    _validNormals = FALSE;
    _material = 0;
}

Mesh* Mesh::buildByIndex(PrimitiveParts parts)
{
    Mesh* emptyMesh = new Mesh();
    //Mesh* emptyMesh = scene->createMesh("mesh");

    // create vertices
    for (int vertKey = 0; vertKey < parts.points.size(); vertKey++)
        emptyMesh->_vertices[vertKey] = new Vertex(emptyMesh,vertKey,parts.points[vertKey]);

    // create the faces and edges
    int edgeCount = 0;
    for (int faceKey = 0; faceKey < parts.faces.size(); faceKey++) {
        Face* face = new Face(emptyMesh, faceKey);
        emptyMesh->_faces[faceKey] = face;

        //Edge* firstEdge = 0;
        //Edge* lastEdge = 0;
        QList<int> vertIndices = parts.faces[faceKey];
        QList<Edge*> edges;
        for (int i = 0; i < vertIndices.size(); i++) {
            Edge* edge = new Edge(emptyMesh,vertIndices[i],faceKey,edgeCount++);
            emptyMesh->_vertices[vertIndices[i]]->setEdge(edge);
            emptyMesh->_edges[edge->key()] = edge;
            edges.append(edge);

            /*
            if (firstEdge == 0)
                firstEdge = edge;
            if (lastEdge != 0) {
                //std::cout << "setting next: " << lastEdge->key() << std::endl;
                lastEdge->setNext(edge);
                lastEdge = lastEdge->next();
                edge->setPrev(lastEdge);
            }
            lastEdge = edge;
            */
            //std::cout << edgeCount << std::endl;
        }
        //lastEdge->setNext(firstEdge);
        //firstEdge->setPrev(lastEdge);

        for (int i = 0; i < edges.length(); i++) {
            if (i == 0)
                edges[i]->setPrev(edges[edges.length()-1]);
            else
                edges[i]->setPrev(edges[i-1]);
            if (i == edges.length()-1)
                edges[i]->setNext(edges[0]);
            else
                edges[i]->setNext(edges[i+1]);
        }

        // add any edge to the face
        face->setEdge(edges[0]);
    }

    emptyMesh->computeEdgePairs();

    QHashIterator<int,Face*> faces = emptyMesh->faces();
    while(faces.hasNext()) {
        faces.next();
        Face* face = faces.value();
        Edge* edge = face->edge();
        do {
            if (edge == edge->next())
                std::cerr << "invalid edge: edge = edge->next" << std::endl;
            if (edge == edge->prev())
                std::cerr << "invalid edge: edge = edge->prev" << std::endl;

            if (edge == edge->next()->next())
                std::cerr << "invalid edge: edge = edge->next->next" << std::endl;

            if (edge == edge->prev()->prev())
                std::cerr << "invalid edge: edge = edge->prev->prev" << std::endl;

            edge = edge->next();
        } while (edge != face->edge());

        edge = face->edge();
        do {
            if (edge->pair() != 0 && edge->pair()->pair() != edge)
                std::cerr << "invalid edge: pair != pair" << std::endl;

            edge = edge->next();
        } while (edge != face->edge());
    }

    return emptyMesh;
}

const int Mesh::numTriangles()
{
    int count = 0;
    QHashIterator<int, Face*> i(_faces);
    while (i.hasNext()) {
         i.next();
         Face* face = i.value();
         Edge* edge = face->edge();
         do {
             count++;
             edge = edge->next();
         } while (edge != face->edge());
         count -= 2;
    }
    return count;
}

class EdgePair
{
public:
    EdgePair(int a, int b) { _a = a; _b = b; }
    int _a;
    int _b;
};

QString pairKey(int a, int b)
{
    QString key;
    QTextStream(&key) << a << "_" << b;

    return key;
}

void Mesh::computeEdgePairs()
{
    // gather edges for each face
    QHash<QString,Edge*> pairs;
    {
        QHashIterator<int, Face*> i(_faces);
        while (i.hasNext()) {
            i.next();
            //std::cout << "face" << std::endl;

            Face* face = i.value();
            Edge* edge = face->edge();

            do {
                pairs[pairKey(edge->vert()->key(), edge->next()->vert()->key())] = edge;
                edge = edge->next();
            } while (edge != face->edge());
        }
    }


    // attach each edge-pair to its corresponding equal
    {
        QHashIterator<int,Face*> i(_faces);
        while (i.hasNext()) {
            i.next();
            Face* face = i.value();
            Edge* edge = face->edge();

            do {
                QString pKey = pairKey(edge->next()->vert()->key(), edge->vert()->key());
                if (pairs.contains(pKey))
                    edge->setPair(pairs[pKey]);

                edge = edge->next();
            } while (edge != face->edge());
        }
    }
}

void Mesh::validateNormals()
{
    if (!_validNormals) {
        QHashIterator<int,Face*> i = faces();
        while (i.hasNext()) {
            i.next();
            Face* face = i.value();
            face->calculateNormal();

        }
    }

    _validNormals = TRUE;
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

Box3D Mesh::worldBounds()
{
    Box3D bounds;
    QMatrix4x4 objToWorld = objectToWorld();
    foreach(Vertex* vertex, _vertices) {
        bounds.includePoint(objToWorld.map(vertex->pos()));
    }
    return bounds;
}

Edge* Face::edge() {
    return _mesh->edge(_edgeKey);
}

void Face::calculateNormal()
{
    Edge* tmpEdge = edge();
    do {
      Edge* edge1 = tmpEdge;
      Edge* edge2 = edge1->next();
      Edge* edge3 = edge2->next();

      Point3 v1 = edge1->vert()->pos();
      Point3 v2 = edge2->vert()->pos();
      Point3 v3 = edge3->vert()->pos();

      Vector3 a = v2 - v1;
      Vector3 b = v3 - v1;
      Vector3 normal = Vector3::crossProduct(a, b).normalized();
      tmpEdge->setNormal(normal);

      tmpEdge = tmpEdge->next();
    } while (tmpEdge != edge());
}
