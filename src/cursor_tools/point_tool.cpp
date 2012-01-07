#include "cursor_tools.h"

#include <QToolBar>
#include <QButtonGroup>

PointTool::PointTool()
{
    _toolbar = new QToolBar();

    QButtonGroup* workModeGroup = new QButtonGroup();

    objectModeButton = new QToolButton();
    objectModeButton->setIcon(QIcon(":/icons/object_mode.png"));
    objectModeButton->setCheckable(TRUE);
    objectModeButton->setChecked(TRUE);
    objectModeButton->setToolTip("Object Mode");
    workModeGroup->addButton(objectModeButton);
    _toolbar->addWidget(objectModeButton);
    vertexModeButton = new QToolButton();
    vertexModeButton->setIcon(QIcon(":/icons/vertex_mode.png"));
    vertexModeButton->setCheckable(TRUE);
    vertexModeButton->setToolTip("Vertex Mode");
    workModeGroup->addButton(vertexModeButton);
    _toolbar->addWidget(vertexModeButton);
    edgeModeButton = new QToolButton();
    edgeModeButton->setIcon(QIcon(":/icons/edge_mode.png"));
    edgeModeButton->setCheckable(TRUE);
    edgeModeButton->setToolTip("Edge Mode");
    workModeGroup->addButton(edgeModeButton);
    _toolbar->addWidget(edgeModeButton);
    faceModeButton = new QToolButton();
    faceModeButton->setIcon(QIcon(":/icons/face_mode.png"));
    faceModeButton->setCheckable(TRUE);
    faceModeButton->setToolTip("Face Mode");
    workModeGroup->addButton(faceModeButton);
    _toolbar->addWidget(faceModeButton);

    connect(workModeGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(on_workModeChanged(QAbstractButton*)));
}

PointTool::~PointTool()
{
    delete _toolbar;
}

int PointTool::workMode()
{
    if (vertexModeButton->isChecked()) return WorkMode::VERTEX;
    else if (edgeModeButton->isChecked()) return WorkMode::EDGE;
    else if (faceModeButton->isChecked()) return WorkMode::FACE;
    else return WorkMode::OBJECT;
}

void PointTool::mousePressed(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect.mousePressed(panel, event);
}

void PointTool::mouseDoubleClicked(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect.mouseDoubleClicked(panel, event);
}

void PointTool::mouseDragged(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect.mouseDragged(panel, event);
}

void PointTool::mouseReleased(PanelGL *panel, QMouseEvent *event)
{
    _basicSelect.mouseReleased(panel, event);
}

void PointTool::postDrawOverlay(PanelGL *panel)
{
    _basicSelect.postDrawOverlay(panel);
}

void PointTool::on_workModeChanged(QAbstractButton *button)
{
    SunshineUi::updatePanels();
}

