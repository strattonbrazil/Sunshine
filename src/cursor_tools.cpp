#include "cursor_tools.h"

void PointTool::mousePressed(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect->mousePressed(panel, event);
}

void PointTool::mouseDragged(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect->mouseDragged(panel, event);
}

void PointTool::mouseReleased(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect->mouseReleased(panel, event);
}

void PointTool::postDrawOverlay(PanelGL *panel)
{
    _basicSelect->postDrawOverlay(panel);
}
