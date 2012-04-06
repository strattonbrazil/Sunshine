#include "vertex_tools.h"

bool VertexNormalizer::isViewable(PanelGL* panel)
{
    if (SunshineUi::workMode() == WorkMode::VERTEX)
        return SunshineUi::activeScene()->hasVertexSelected();
    return false;
}

QList<ContextAction*> VertexNormalizer::actions()
{
    QList <ContextAction*> actions;

    if (SunshineUi::workMode() == WorkMode::VERTEX) {
        actions << new ContextAction("Adjust Normals", 0, this);
    }



    return actions;
}

bool VertexNormalizer::init(PanelGL *panel, QString command, int button)
{
    ContextMenu popup;
    popup.addAction("Smooth Normals");
    popup.addAction("Flatten Normals");

    QAction* action = popup.exec(QCursor::pos());
    if (!action)
        return FALSE;

    if (action->text() == "Smooth Normals") {
        foreach(QString meshName, panel->scene()->meshes()) {
            Mesh* mesh = panel->scene()->mesh(meshName);
            if (mesh->isSelected()) {
                QHashIterator<int,Vertex*> vertices = mesh->vertices();
                while (vertices.hasNext()) {
                    vertices.next();
                    Vertex* vertex = vertices.value();
                    Vector3 normal;
                    if (vertex->isSelected()) {
                        Edge* edge = vertex->edge();
                        QSet<int> processedEdges;
                        std::cout << "starting: " << edge << std::endl;
                        do {
                            normal += edge->flatNormal();

                            std::cout << "got normal" << std::endl;
                            //std::cout << edge << std::endl;
                            //std::cout << edge->flatNormal() << std::endl;
                            //edge->vert();
                            //std::cout << edge->pair() << std::endl;
                            //edge->pair()->next();
                            //edge->pair()->next()->vert();
                            //std::cout << edge->vert() << " " << edge->pair()->next()->vert() << std::endl;

                            if (processedEdges.contains(edge->key()))
                                break;
                            processedEdges.insert(edge->key());

                            std::cout << edge << std::endl;

                            edge = edge->prev()->pair();
                        } while (edge != vertex->edge() && edge != 0);
                        normal.normalize();

                        processedEdges.clear();

                        edge = vertex->edge();
                        do {
                            edge->setNormal(normal);

                            if (processedEdges.contains(edge->key()))
                                break;
                            processedEdges.insert(edge->key());

                            edge = edge->prev()->pair();
                        } while (edge != vertex->edge() && edge != 0);

                        std::cout << "done" << std::endl;
                    }
                }
            }
        }
    }
    else if (action->text() == "Flatten Normals") {

    }

    return TRUE;
}
