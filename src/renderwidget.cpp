#include "renderwidget.h"
#include "ui_renderwidget.h"
#include <QLayout>

RenderWidget::RenderWidget(QDialog *parent) :
    QDialog(parent),
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
