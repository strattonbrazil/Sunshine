#include "renderwidget.h"
#include "ui_renderwidget.h"
#include <QLayout>

RenderWidget::RenderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RenderWidget)
{
    ui->setupUi(this);

    _viewer = new ImageViewer();

    layout()->addWidget(_viewer);
}

RenderWidget::~RenderWidget()
{
    delete ui;
    delete _viewer;
}

void RenderWidget::open(QString fileName)
{
    _viewer->open(fileName);
}

RenderSettings::RenderSettings()
{
    QString samplingWidth("{ 'var' : 'samplingWidth', 'name' : 'Sampling Width', 'type' : 'float', 'min' : 1, 'max' : 4, 'value' : 1 }");

    QStringList atts;
    atts << samplingWidth;

    addAttributes(atts);
}
