#include "sunshine.h"
#include "ui_sunshine.h"
#include "register.h"

Sunshine::Sunshine(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Sunshine)
{
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

void Sunshine::clearScene(void)
{
    Register::clearScene();
    setupDefaultCameras();
    setupDefaultMeshes();
    setupDefaultLights();
}

void Sunshine::setupDefaultCameras(void)
{
    Register::createCamera("persp");
    Register::createCamera("side");
    Register::createCamera("top");
    Register::createCamera("front");
}

void Sunshine::setupDefaultMeshes(void)
{

}

void Sunshine::setupDefaultLights(void)
{
}
