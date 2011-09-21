#include "select.h"
#include "face_util.h"
#include "vertex_util.h"
#include "sunshine.h"
#include <algorithm>
#include "scene.h"

void BasicSelect::mousePressed(PanelGL *panel, QMouseEvent *event)
{
    pick = event->pos();
    current = event->pos();

    minX = std::min(pick.x(), current.x());
    minY = std::min(panel->height() - pick.y(), panel->height() - current.y());
    maxX = std::max(pick.x(), current.x());
    maxY = std::max(panel->height() - pick.y(), panel->height() - current.y());

    Point3 rayOrig = panel->camera()->eye();
    Vector3 rayDir = panel->computeRayDirection(event->pos());

    if (SunshineUi::workMode() == WorkMode::LAYOUT)
        _selectMode = SelectMode::BOX;
    else {
        if (panel->_hoverFace.data() == 0 && panel->_hoverVert.data() == 0)
            _selectMode = SelectMode::BOX;
        else
            _selectMode = SelectMode::LINE;
    }

    // figure out if model mode should be NONE
    if (SunshineUi::workMode() == WorkMode::MODEL) {

        QHashIterator<int,MeshP> meshes = panel->scene()->meshes();
        bool hasFaces = FALSE;
        bool hasVertices = FALSE;
        while (meshes.hasNext()) {
            meshes.next();
            int meshKey = meshes.key();
            MeshP mesh = meshes.value();
            if (mesh->isSelected()) {
                // any selected faces?
                QHashIterator<int,FaceP> i = mesh->faces();
                while (i.hasNext()) {
                    i.next();
                    FaceP face = i.value();
                    if (face->isSelected())
                        hasFaces = TRUE;
                }
                // any selected edges?

                // any selected vertices?
                QHashIterator<int,VertexP> j = mesh->vertices();
                while (j.hasNext()) {
                    j.next();
                    VertexP vertex = j.value();
                    if (vertex->isSelected())
                        hasVertices = TRUE;
                }
            }
        }

        // nothing selected anymore
        if (!hasFaces && !hasVertices)
            modelMode = ModelMode::NONE;
    }

    if (_selectMode == SelectMode::BOX) {
        if (modelMode == ModelMode::NONE)
            modelMode = ModelMode::VERTEX;
    }
    else if (_selectMode == SelectMode::LINE) {
        if (SunshineUi::workMode() == WorkMode::LAYOUT) {
            if (panel->_hoverMesh.data() != 0) {

                if (panel->_hoverMesh->isSelected())
                    selectToggle = SelectToggle::OFF;
                else
                    selectToggle = SelectToggle::ON;
            }
        }
        else if (SunshineUi::workMode() == WorkMode::MODEL) {
            if (modelMode == ModelMode::NONE) {
                // anything happen to be under the mouse?
                if (panel->_hoverVert) {
                    modelMode = ModelMode::VERTEX;
                    if (panel->_hoverVert->isSelected())
                        selectToggle = SelectToggle::OFF;
                    else
                        selectToggle = SelectToggle::ON;
                }
                else if (panel->_hoverFace) {
                    modelMode = ModelMode::FACE;
                    if (panel->_hoverFace->isSelected())
                        selectToggle = SelectToggle::OFF;
                    else
                        selectToggle = SelectToggle::ON;
                }
                else {
                    modelMode = ModelMode::FACE;
                    selectToggle = SelectToggle::ON;
                }
            } else {
                std::cout << "need to decide on select toggle" << std::endl;
            }
        }
        /*
        if (Sunshine::geometryMode() == GeometryMode::OBJECT) {
            if (mesh) {
                if (mesh->isSelected())
                    selectToggle = SelectToggle::OFF;
                else
                    selectToggle = SelectToggle::ON;
            }
        }
        else if (Sunshine::geometryMode() == GeometryMode::VERTEX) {
            VertexP vertex = VertexUtil::closestVertexOnFace(rayOrig, rayDir, mesh, face);
            if (vertex->isSelected())
                selectToggle = SelectToggle::OFF;
            else
                selectToggle = SelectToggle::ON;
        }
        else if (Sunshine::geometryMode() == GeometryMode::EDGE) {}
        else if (Sunshine::geometryMode() == GeometryMode::FACE) {
            if (face) {
                if (face->isSelected())
                    selectToggle = SelectToggle::OFF;
                else
                    selectToggle = SelectToggle::ON;
            }
        }

        processLineSelection(panel, event);
        */
    }
    panel->_hoverMesh = MeshP(0);
    panel->_hoverFace = FaceP(0);
    panel->_hoverVert = VertexP(0);
}

void BasicSelect::mouseReleased(PanelGL *panel, QMouseEvent *event)
{
    bool shiftDown = event->modifiers() & Qt::ShiftModifier;
    bool controlDown = event->modifiers() & Qt::ControlModifier;

    if (BasicSelect::selectMode() == SelectMode::BOX)
        processBoxSelection(panel, !shiftDown, !controlDown);

    selectToggle = SelectToggle::NONE;
}

void BasicSelect::mouseDragged(PanelGL* panel, QMouseEvent* event)
{
    if (_selectMode == SelectMode::LINE)
        processLineSelection(panel, event);

    current = event->pos();

    minX = std::min(pick.x(), current.x());
    minY = std::min(panel->height() - pick.y(), panel->height() - current.y());
    maxX = std::max(pick.x(), current.x());
    maxY = std::max(panel->height() - pick.y(), panel->height() - current.y());
}

void BasicSelect::processBoxSelection(PanelGL *panel, bool newSelection, bool selectValue)
{
    if (SunshineUi::workMode() == WorkMode::LAYOUT) {
        QHashIterator<int,MeshP> meshes = panel->scene()->meshes();
        while (meshes.hasNext()) {
            meshes.next();
            int meshKey = meshes.key();
            MeshP mesh = meshes.value();
            if (newSelection && selectValue)
                mesh->setSelected(!selectValue);
            QMatrix4x4 objToWorld = mesh->objectToWorld();
            QHashIterator<int,VertexP> vertices = mesh->vertices();
            while (vertices.hasNext()) {
                vertices.next();
                int key = vertices.key();
                VertexP vertex = vertices.value();

                Point3 objectP = vertex->pos();
                Point3 worldP = objToWorld.map(objectP);
                Point3 screenP = panel->project(worldP);

                if (screenP.x() >= minX && screenP.x() <= maxX && screenP.y() >= minY && screenP.y() <= maxY) {
                    mesh->setSelected(selectValue);
                    break;
                }
            }
        }
    } else {
        // grab whatever the work
        QHashIterator<int,MeshP> meshes = panel->scene()->meshes();
        while (meshes.hasNext()) {
            meshes.next();
            int meshKey = meshes.key();
            MeshP mesh = meshes.value();
            if (mesh->isSelected()) {
                QMatrix4x4 objToWorld = mesh->objectToWorld();
                if (modelMode == ModelMode::VERTEX) {
                    QHashIterator<int,VertexP> vertices = mesh->vertices();
                    while (vertices.hasNext()) {
                        vertices.next();
                        int key = vertices.key();
                        VertexP vertex = vertices.value();
                        if (newSelection && selectValue)
                            vertex->setSelected(!selectValue);

                        Point3 objectP = vertex->pos();
                        Point3 worldP = objToWorld.map(objectP);
                        Point3 screenP = panel->project(worldP);

                        if (screenP.x() >= minX && screenP.x() <= maxX && screenP.y() >= minY && screenP.y() <= maxY) {
                            vertex->setSelected(selectValue);
                        }
                    }
                }
                else if (modelMode == ModelMode::EDGE) {
                    std::cerr << "need to implement: box selecting edges" << std::endl;
                }
                else if (modelMode == ModelMode::FACE) {
                    std::cerr << "need to implement: box selecting faces" << std::endl;
                }
            }
        }
    }
}

void BasicSelect::processLineSelection(PanelGL *panel, QMouseEvent *event) {
    SceneP scene = panel->scene();
    Vector3 rayOrig = panel->camera()->eye();
    Point3 rayDir = panel->computeRayDirection(event->pos());
    if (SunshineUi::workMode() == WorkMode::LAYOUT) {
    } else {
        if (modelMode == ModelMode::FACE) {
            FaceUtil::FaceHit faceHit = FaceUtil::closestFace(scene, rayOrig, rayDir, TRUE);
            if (faceHit.nearFace)
                faceHit.nearFace->setSelected(selectToggle == SelectToggle::ON);
        }
        else if (modelMode == ModelMode::VERTEX) {
            VertexUtil::VertexHit vertexHit = VertexUtil::closestVertex(panel, event, TRUE);
            if (vertexHit.vertex)
                vertexHit.vertex->setSelected(selectToggle == SelectToggle::ON);

        }
    }
}
