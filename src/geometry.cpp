#include "geometry.h"
#include "sunshine.h"

Vertex::Vertex(MeshP mesh, int key, Point3 point)
{
    _mesh = mesh;
    _key = key;
    _point = point;
    _selected = FALSE;
}

void Vertex::setEdge(EdgeP e) { _edgeKey = e->key(); }

Edge::Edge(MeshP mesh, int vertexKey, int faceKey, int edgeKey)
{
    _mesh = mesh;
    _vertexKey = vertexKey;
    _faceKey = faceKey;
    _edgeKey = edgeKey;
    _selected = FALSE;
}

MeshP Edge::mesh() {
    return _mesh;
}

Face::Face(MeshP mesh, int faceKey)
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

    EdgeP tmpEdge = edge()->next();
    do {
        triangles << Triangle(edge(), tmpEdge, tmpEdge->next());
        tmpEdge = tmpEdge->next();
    } while (tmpEdge != edge()->prev());

    return QListIterator<Triangle>(triangles);
}

Mesh::Mesh(SceneP scene, QString name) : Transformable()
{
    _scene = scene;
    _name = name;
    _validNormals = FALSE;
}

Mesh::Mesh(SceneP scene, QString name, QHash<int,VertexP> vertices, QHash<int,EdgeP> edges, QHash<int,FaceP> faces)
{
    _scene = scene;
    _name = name;
    _vertices = vertices;
    _edges = edges;
    _faces = faces;
    _validNormals = FALSE;
}

MeshP Mesh::buildByIndex(SceneP scene, PrimitiveParts parts)
{
    MeshP emptyMesh = scene->createMesh("mesh");
    QString meshName = emptyMesh->name();

    // create vertices
    for (int vertKey = 0; vertKey < parts.points.size(); vertKey++)
        emptyMesh->_vertices[vertKey] = VertexP(new Vertex(emptyMesh,vertKey,parts.points[vertKey]));

    // create the faces and edges
    int edgeCount = 0;
    for (int faceKey = 0; faceKey < parts.faces.size(); faceKey++) {
        FaceP face(new Face(emptyMesh, faceKey));
        emptyMesh->_faces[faceKey] = face;

        EdgeP firstEdge;
        EdgeP lastEdge;
        QList<int> vertIndices = parts.faces[faceKey];
        for (int i = 0; i < vertIndices.size(); i++) {
            EdgeP edge(new Edge(emptyMesh,vertIndices[i],faceKey,edgeCount++));
            emptyMesh->_vertices[vertIndices[i]]->setEdge(edge);
            emptyMesh->_edges[edge->key()] = edge;

            if (firstEdge == NULL)
                firstEdge = edge;
            if (lastEdge != NULL) {
                //std::cout << "setting next: " << lastEdge->key() << std::endl;
                lastEdge->setNext(edge);
                lastEdge->next();
                edge->setPrev(lastEdge);
            }
            lastEdge = edge;
            //std::cout << edgeCount << std::endl;
        }
        lastEdge->setNext(firstEdge);
        firstEdge->setPrev(lastEdge);

        // add any edge to the face
        face->setEdge(firstEdge);
    }

    emptyMesh->computeEdgePairs();

    return emptyMesh;
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
    QHash<QString,EdgeP> pairs;
    {
        QHashIterator<int, FaceP> i(_faces);
        while (i.hasNext()) {
            i.next();
            //std::cout << "face" << std::endl;

            FaceP face = i.value();
            EdgeP edge = face->edge();

            do {
                pairs[pairKey(edge->vert()->key(), edge->next()->vert()->key())] = edge;
                edge = edge->next();
            } while (edge != face->edge());
        }
    }


    // attach each edge-pair to its corresponding equal
    {
        QHashIterator<int,FaceP> i(_faces);
        while (i.hasNext()) {
            i.next();
            FaceP face = i.value();
            EdgeP edge = face->edge();

            do {
                QString key = pairKey(edge->vert()->key(), edge->next()->vert()->key());
                if (pairs.contains(key))
                    edge->setPair(pairs[key]);

                edge = edge->next();
            } while (edge != face->edge());
        }
    }
}

void Mesh::validateNormals()
{
    if (!_validNormals) {
        QHashIterator<int,FaceP> i = faces();
        while (i.hasNext()) {
            i.next();
            FaceP face = i.value();
            face->calculateNormal();

        }
    }

    _validNormals = TRUE;
}

MaterialP Mesh::material()
{
    if (!_material)
        _material = _scene->defaultMaterial();
    return _material;
}

QMatrix4x4 Mesh::normalToWorld()
{
    return _rotate.matrix();
}

EdgeP Face::edge() {
    return _mesh->edge(_edgeKey);
}

void Face::calculateNormal()
{
    EdgeP tmpEdge = edge();
    do {
      EdgeP edge1 = tmpEdge;
      EdgeP edge2 = edge1->next();
      EdgeP edge3 = edge2->next();

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
