#ifndef BASICSELECT_H
#define BASICSELECT_H

#include "panelgl.h"
#include "sunshine.h"

namespace SelectToggle { enum { NONE, ON, OFF }; };

class BasicSelect
{
public:
    static int               selectMode() { return SelectMode::BOX; }
    static void              mousePressed(PanelGL* panel, QMouseEvent* event);
    static void              mouseReleased(PanelGL* panel, QMouseEvent* event);
    static void              mouseDragged(PanelGL* panel, QMouseEvent* event);
    static int               minX, minY, maxX, maxY;
private:
                             BasicSelect();
    static QPoint            pick;
    static QPoint            current;
    static int               selectToggle;
    static void              processBoxSelection(PanelGL* panel, bool newSelection, bool selectValue);
    static void              processLineSelection(PanelGL* panel, QMouseEvent* event);
};

#endif // BASICSELECT_H
