#include "node_widget.h"
#include "ui_node_widget.h"
#include "sunshine.h"
#include "material.h"

NodeWidget::NodeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeWidget)
{
    ui->setupUi(this);
    _shader = 0;
}

void NodeWidget::setShader(Shader *s)
{
    _shader = s;

    QString shaderName = SunshineUi::activeScene()->assetName(_shader);
    ui->shaderLabel->setText(shaderName);

    foreach(Attribute attribute, _shader->inputs()) {
        QLabel* label = new QLabel(attribute->property("name").toString());
        label->setStyleSheet("QLabel { font-size: 10px; }");
        ui->inputsFrame->layout()->addWidget(label);
    }
}

NodeWidget::~NodeWidget()
{
    delete ui;
}
