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
    QString xres("{ 'var' : 'xres', 'name' : 'Image Width', 'type' : 'int', 'min' : 1, 'max' : 4096, 'value' : 1024 }");
    QString yres("{ 'var' : 'yres', 'name' : 'Image Height', 'type' : 'int', 'min' : 1, 'max' : 4096, 'value' : 768 }");
    QString samplingWidth("{ 'var' : 'samplingWidth', 'name' : 'Sampling Width', 'type' : 'float', 'min' : 1, 'max' : 8, 'value' : 2 }");
    QString exposure("{ 'var' : 'exposure', 'name' : 'Exposure', 'type' : 'float', 'min' : 0.001, 'max' : 1000, 'value' : 1.0 }");

    QStringList atts;
    atts << xres << yres << samplingWidth << exposure;

    addAttributes(atts);
}
