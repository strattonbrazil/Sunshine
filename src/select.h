#ifndef BASICSELECT_H
#define BASICSELECT_H

#include "panelgl.h"
#include "sunshine.h"
#include "util.h"

namespace SelectToggle { enum { NONE, ON, OFF }; };

class PanelGL;

class BasicSelect
{
public:
    int               selectMode() { return _selectMode; }
    void              mousePressed(PanelGL* panel, QMouseEvent* event);
    void              mouseReleased(PanelGL* panel, QMouseEvent* event);
    void              mouseDragged(PanelGL* panel, QMouseEvent* event);
    int               minX, minY, maxX, maxY;
    int               modelMode;
private:
    QPoint            pick;
    QPoint            current;
    int               selectToggle;
    int               _selectMode;
    void              processBoxSelection(PanelGL* panel, bool newSelection, bool selectValue);
    void              processLineSelection(PanelGL* panel, QMouseEvent* event);
};
typedef QSharedPointer<BasicSelect> BasicSelectP;

#endif // BASICSELECT_H
