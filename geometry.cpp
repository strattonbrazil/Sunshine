#include "geometry.h"

Vertex::Vertex(int meshKey, int key, Point3 point)
{
    _meshKey = meshKey;
    _key = key;
    _point = point;
}

void Vertex::setEdge(EdgeP e) { _edgeKey = e->key(); }

Edge::Edge(int meshKey, int vertexKey, int faceKey, int edgeKey)
{
    _meshKey = meshKey;
    _vertexKey = vertexKey;
    _faceKey = faceKey;
    _edgeKey = edgeKey;
}

MeshP Edge::mesh() {
    return Register::mesh(_meshKey);
}

Face::Face(int meshKey, int faceKey)
{
    _meshKey = meshKey;
    _faceKey = faceKey;
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

Mesh::Mesh(int key, QString name)
{
    _key = key;
    _name = name;
    _validNormals = FALSE;
}

Mesh::Mesh(int key, QString name, QHash<int,VertexP> vertices, QHash<int,EdgeP> edges, QHash<int,FaceP> faces)
{
    _key = key;
    _name = name;
    _vertices = vertices;
    _edges = edges;
    _faces = faces;
    _validNormals = FALSE;
}

void Mesh::buildByIndex(PrimitiveParts parts)
{
    MeshP emptyMesh = Register::createMesh("mesh");
    int meshKey = emptyMesh->key();

    //QHash<int,VertexP> vertices;
    //QHash<int,EdgeP> edges;
    //QHash<int,FaceP> faces;

    // create vertices
    for (int vertKey = 0; vertKey < parts.points.size(); vertKey++)
        emptyMesh->_vertices[vertKey] = VertexP(new Vertex(meshKey,vertKey,parts.points[vertKey]));

    // create the faces and edges
    int edgeCount = 0;
    for (int faceKey = 0; faceKey < parts.faces.size(); faceKey++) {
        FaceP face(new Face(meshKey, faceKey));
        emptyMesh->_faces[faceKey] = face;

        EdgeP firstEdge;
        EdgeP lastEdge;
        QList<int> vertIndices = parts.faces[faceKey];
        for (int i = 0; i < vertIndices.size(); i++) {
            EdgeP edge(new Edge(meshKey,vertIndices[i],faceKey,edgeCount++));
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

    /*
    for (int i = 0; i < emptyMesh->_vertices.size(); i++) {
        VertexP v = emptyMesh->_vertices[i];
        std::cout << "v: " << v->key() << std::endl;
    }

    std::cout << "----------" << std::endl;

    for (int i = 0; i < emptyMesh->_edges.size(); i++) {
        EdgeP e = emptyMesh->_edges[i];
        std::cout << "e: " << e->key() << std::endl;
        //std::cout << e->next() << std::endl;
        //std::cout << "  n:" << e->next()->key() << std::endl;
        //std::cout << "  p:" << e->prev()->key() << std::endl;
    }

    std::cout << "----------" << std::endl;

    for (int i = 0; i < emptyMesh->_faces.size(); i++) {
        FaceP f = emptyMesh->_faces[i];
        std::cout << "f: " << f->key() << std::endl;
        std::cout << "  e: " << f->edge()->key() << std::endl;
    }
    */

    // construct vertices, edges, and faces
    //MeshP mesh = MeshP(new Mesh(meshKey, emptyMesh->name(), vertices, edges, faces));
    //Register::setMesh(meshKey, mesh);

    emptyMesh->computeEdgePairs();
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
    //QString key("%1_%2");
    //key = key.arg("%f", a).arg("%f", b);
    //std::cout << key.toStdString() << std::endl;
    return key;
}

void Mesh::computeEdgePairs()
{
    std::cout << "Computing edge pairs" << std::endl;
    //std::cout << _faces.size() << std::endl;
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
                //pairs += (edge->vert.key(), edge.next.vert.key()) -> edge
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

EdgeP Face::edge() {
    //std::cout << Register::mesh(_meshKey)-> << std::endl;
    return Register::mesh(_meshKey)->edge(_edgeKey);
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
