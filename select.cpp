#include "select.h"
#include "face_util.h"
#include "vertex_util.h"
#include "sunshine.h"
#include <algorithm>

int BasicSelect::minX;
int BasicSelect::maxX;
int BasicSelect::minY;
int BasicSelect::maxY;
QPoint BasicSelect::pick;
QPoint BasicSelect::current;
int BasicSelect::selectToggle;


BasicSelect::BasicSelect()
{
}

void BasicSelect::mousePressed(PanelGL *panel, QMouseEvent *event)
{
    pick = event->pos();
    current = event->pos();

    minX = std::min(pick.x(), current.x());
    minY = std::min(panel->height() - pick.y(), panel->height() - current.y());
    maxX = std::max(pick.x(), current.x());
    maxY = std::max(panel->height() - pick.y(), panel->height() - current.y());

    /*
    Point3 rayOrig = panel->camera()->eye();
    Vector3 rayDir = panel->computeRayDirection(event->pos());

    // record the selection mode, basic object selection defaults to box regions
    FaceUtil::FaceHit faceHit = FaceUtil::closestFace(rayOrig, rayDir, false);
    MeshP mesh = faceHit.nearMesh;
    MeshP face = faceHit.nearFace;

    int selectMode;
    if (Sunshine::selectMode() == SelectMode::BASIC) {
        if (Sunshine::geometryMode() == GeometryMode::OBJECT)
            selectMode = SelectMode::BOX;
        else {
            if (!face)
                selectMode = SelectMode::BOX;
            else
                selectMode = SelectMode::LINE;
        }
    }

    if (selectMode == SelectMode::BOX) {
    }
    else if (selectMode == SelectMode::LINE) {
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
    }
    */

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
    /*
    if @select_mode == SelectMode::LINE
  processLineSelection(panel, event)
end
*/


    current = event->pos();

    minX = std::min(pick.x(), current.x());
    minY = std::min(panel->height() - pick.y(), panel->height() - current.y());
    maxX = std::max(pick.x(), current.x());
    maxY = std::max(panel->height() - pick.y(), panel->height() - current.y());
}

void BasicSelect::processBoxSelection(PanelGL *panel, bool newSelection, bool selectValue) {

}

void BasicSelect::processLineSelection(PanelGL *panel, QMouseEvent *event) {

}
