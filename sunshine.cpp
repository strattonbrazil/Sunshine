#include "sunshine.h"
#include "ui_sunshine.h"
#include "register.h"
#include "primitive.h"
#include <aqsis/aqsis.h>
#include <aqsis/ri/ri.h>

Sunshine::Sunshine(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Sunshine)
{
    renderWidget = NULL;

    clearScene();
    ui->setupUi(this);
}

Sunshine::~Sunshine()
{
    delete ui;
}

void Sunshine::changeEvent(QEvent *e)
{

    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Sunshine::clearScene()
{
    Register::clearScene();
    setupDefaultCameras();
    setupDefaultMeshes();
    setupDefaultLights();
}

void Sunshine::setupDefaultCameras()
{
    Register::createCamera("persp");
    Register::createCamera("side");
    Register::createCamera("top");
    Register::createCamera("front");
}

void Sunshine::setupDefaultMeshes()
{
    Mesh::buildByIndex(primitive::cubePrimitive(1.0f, 1.0f, 1.0f));
}

void Sunshine::setupDefaultLights()
{
}

void Sunshine::on_renderButton_clicked()
{
    if (renderWidget == NULL)
        renderWidget = new RenderWidget();

    char* fileName = "/tmp/test.tif";
    renderWidget->show();

    RiBegin(RI_NULL);
    // Output image
    RiDisplay(fileName, "file", "rgb", RI_NULL);
    RiFormat(800, 800, 1);

    // Camera type & position
    float fov = 45;
    RiProjection("perspective", "fov", &fov, RI_NULL);
    RiTranslate(0, 0, 3);

    RiWorldBegin();
    // Geometry
    float intensity = 1.0f;
    RiLightSource("pointlight", "intensity", &intensity, RI_NULL);
    float Ks = 1.0f;
    RiSurface("plastic", "Ks", &Ks, RI_NULL);
    for (int j = 0; j < 20; ++j) {
        const int ringNum = 5;
        for(int i = 0; i < ringNum; ++i) {
            RiTransformBegin();
            RiRotate(i*360/ringNum, 0, 0, 1);
            RiTranslate(1, 0, 0);
            RiSphere(0.2f, -0.2f, 0.2f, 360, RI_NULL);
            RiTransformEnd();
        }
        RiRotate(10, 0, 0, 1);
        RiScale(0.8f, 0.8f, 0.8f);
        RiTranslate(0, 0, 0.3);
    }
    RiWorldEnd();

    RiEnd();

    renderWidget->open(QString(fileName));

    std::cout << "Render to window" << std::endl;
}
