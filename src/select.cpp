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

    bool componentsHighlighted = panel->_hoverFace != 0 || panel->_hoverVert != 0;

    // selecting objects
    if (SunshineUi::workMode() == WorkMode::OBJECT && !componentsHighlighted) {
        _selectMode = SelectMode::BOX;
    }
    else {
        if (panel->_hoverMesh == 0 || !(panel->_hoverMesh->isSelected()))
            _selectMode = SelectMode::BOX;
        else
            _selectMode = SelectMode::LINE;
    }

    // figure out if model mode should be NONE
    /*
    if (SunshineUi::workMode() == WorkMode::VERTEX) {
        bool hasFaces = FALSE;
        bool hasVertices = FALSE;
        foreach(QString meshName, panel->scene()->meshes()) {
            MeshP mesh = panel->scene()->mesh(meshName);
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
    */

    if (_selectMode == SelectMode::BOX) {
        /*
        if (modelMode == ModelMode::NONE)
            modelMode = ModelMode::VERTEX;
            */
    }
    else if (_selectMode == SelectMode::LINE) {
        // figure out if turning on or turning off components during drag
        //
        if (SunshineUi::workMode() == WorkMode::OBJECT) {
            if (panel->_hoverMesh != 0 && panel->_hoverMesh->isSelected()) {
                if (panel->_hoverVert != 0) {
                    if (panel->_hoverVert->isSelected())
                        selectToggle = SelectToggle::OFF;
                    else
                        selectToggle = SelectToggle::ON;
                    _dragWorkMode = WorkMode::VERTEX;
                }
                else if (panel->_hoverFace != 0) {
                    if (panel->_hoverFace->isSelected())
                        selectToggle = SelectToggle::OFF;
                    else
                        selectToggle = SelectToggle::ON;
                    _dragWorkMode = WorkMode::FACE;
                }
            }
            else
                _dragWorkMode = WorkMode::OBJECT;
        }
        /*
        else if (SunshineUi::workMode() == WorkMode::VERTEX) {
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
        */

        /*
        if (Sunshine::workMode() == WorkMode::OBJECT) {
            if (mesh) {
                if (mesh->isSelected())
                    selectToggle = SelectToggle::OFF;
                else
                    selectToggle = SelectToggle::ON;
            }
        }
        */
        if (_dragWorkMode == WorkMode::VERTEX) {
            //VertexUtil::VertexHit vertexHit = VertexUtil::closestVertex(rayOrig, rayDir, panel->_hoverMesh, panel->_hoverFace);
            if (panel->_hoverVert != 0) {
                if (panel->_hoverVert->isSelected())
                    selectToggle = SelectToggle::OFF;
                else
                    selectToggle = SelectToggle::ON;
            }
        }
        else if (_dragWorkMode == WorkMode::FACE) {
            if (panel->_hoverFace != 0) {
                if (panel->_hoverFace->isSelected())
                    selectToggle = SelectToggle::OFF;
                else
                    selectToggle = SelectToggle::ON;
            }
        }

        processLineSelection(panel, event);
    }


}

void BasicSelect::mouseReleased(PanelGL *panel, QMouseEvent *event)
{
    bool shiftDown = event->modifiers() & Qt::ShiftModifier;
    bool controlDown = event->modifiers() & Qt::ControlModifier;

    if (BasicSelect::selectMode() == SelectMode::BOX)
        processBoxSelection(panel, !shiftDown, !controlDown);

    selectToggle = SelectToggle::NONE;
    _selectMode = SelectMode::NONE;
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

    panel->_hoverMesh = MeshP(0);
    panel->_hoverFace = FaceP(0);
    panel->_hoverVert = VertexP(0);
}

void BasicSelect::mouseDoubleClicked(PanelGL *panel, QMouseEvent *event)
{
    //if (SunshineUi::workMode() != WorkMode::OBJECT) {
        if (panel->_hoverMesh != 0 && !(panel->_hoverMesh->isSelected())) {
            panel->_hoverMesh->setSelected(TRUE);
        }
    //}
}

void BasicSelect::postDrawOverlay(PanelGL *panel)
{
    if (_selectMode == SelectMode::NONE)
        return;

    const int width = panel->width();
    const int height = panel->height();

    if (selectMode() == SelectMode::BOX) {
        CameraP camera = panel->camera();
        QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera, width, height);
        QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera, width, height);
        QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
        QMatrix4x4 objToWorld;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glColor4f(1,.2f,1,.2f);
        glBegin(GL_LINE_LOOP);
        {
            glVertex2f(minX,minY);
            glVertex2f(minX,maxY);
            glVertex2f(maxX,maxY);
            glVertex2f(maxX,minY);
        }
        glEnd();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        {
            glVertex2f(minX,minY);
            glVertex2f(minX,maxY);
            glVertex2f(maxX,maxY);
            glVertex2f(maxX,minY);
        }
        glEnd();
        glDisable(GL_BLEND);
    }
}

void BasicSelect::processBoxSelection(PanelGL *panel, bool newSelection, bool selectValue)
{
    if (SunshineUi::workMode() == WorkMode::OBJECT) {
        foreach(QString meshName, panel->scene()->meshes()) {
            MeshP mesh = panel->scene()->mesh(meshName);

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
        // grab whatever the work mode is
        foreach(QString meshName, panel->scene()->meshes()) {
            MeshP mesh = panel->scene()->mesh(meshName);

            if (mesh->isSelected()) {
                QMatrix4x4 objToWorld = mesh->objectToWorld();
                if (SunshineUi::workMode() == WorkMode::VERTEX) {
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
                /*
                else if (modelMode == ModelMode::EDGE) {
                    std::cerr << "need to implement: box selecting edges" << std::endl;
                }
                */
                else if (SunshineUi::workMode() == WorkMode::FACE) {
                    QHashIterator<int,FaceP> faces = mesh->faces();
                    while (faces.hasNext()) {
                        faces.next();
                        int key = faces.key();
                        FaceP face = faces.value();
                        if (newSelection && selectValue)
                            face->setSelected(!selectValue);

                        EdgeP edge = face->edge();
                        do {
                            VertexP vertex = edge->vert();

                            Point3 objectP = vertex->pos();
                            Point3 worldP = objToWorld.map(objectP);
                            Point3 screenP = panel->project(worldP);

                            if (screenP.x() >= minX && screenP.x() <= maxX && screenP.y() >= minY && screenP.y() <= maxY) {
                                face->setSelected(selectValue);
                            }
                            edge = edge->next();
                        } while (edge != face->edge());


                    }
                }
            }
        }
    }
}

void BasicSelect::processLineSelection(PanelGL *panel, QMouseEvent *event) {
    SceneP scene = panel->scene();
    Vector3 rayOrig = panel->camera()->eye();
    Point3 rayDir = panel->computeRayDirection(event->pos());
    if (_dragWorkMode == WorkMode::OBJECT) {
        if (panel->_hoverFace != 0 || panel->_hoverVert != 0) {

        }
    }
    else if (_dragWorkMode == WorkMode::FACE) {
        FaceUtil::FaceHit faceHit = FaceUtil::closestFace(scene, rayOrig, rayDir, TRUE);
        if (faceHit.nearFace)
            faceHit.nearFace->setSelected(selectToggle == SelectToggle::ON);
    }
    else if (_dragWorkMode == WorkMode::VERTEX) {
        VertexUtil::VertexHit vertexHit = VertexUtil::closestVertex(panel, event, TRUE);
        if (vertexHit.vertex)
            vertexHit.vertex->setSelected(selectToggle == SelectToggle::ON);
    }
}
