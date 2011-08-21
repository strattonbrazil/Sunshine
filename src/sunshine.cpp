#include "sunshine.h"
#include "ui_sunshine.h"
#include "scene.h"
#include "primitive.h"
#include <aqsis/aqsis.h>
#include <aqsis/ri/ri.h>
#include "settings.h"
#include <QFileDialog>
#include "panelgl.h"
#include "sunshineui.h"

//int Sunshine::_geometryMode = GeometryMode::OBJECT;
//extern RegisterP Sunshine::activeRegister;

void say_hello(const char* name) {
    std::cout << "Hello " <<  name << "!\n";
}

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>

using namespace boost::python;

/*
BOOST_PYTHON_MODULE(libsunshine)
{
    def("say_hello", say_hello);
    class_<PanelGL>("PanelGL","panelgl docs uh",init<Scene*>());
    class_<Scene>("Scenes","scene docs uh");
    class_<SunshineUi>("SunshineUis","sunshine ui uh",init<Scene*>());
}
*/

Sunshine::Sunshine(QWidget *parent) : QMainWindow(parent), ui(new Ui::Sunshine)
{
    int x = 5;
    clearScene();
    ui->setupUi(this);



    ui->tabs->addTab(new PanelGL(_scene), "tab 1");
    //QObject::connect(&(ui->renderButton), SIGNAL(valueChanged(int)),
     //                     &b, SLOT(setValue(int)));



}


Sunshine::~Sunshine()
{
    delete ui;
    //if (!_renderWidget) delete _renderWidget;
    //if (!_renderSettingsWidget) delete _renderSettingsWidget;
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
    _scene = SceneP(new Scene());
    //activeRegister = RegisterP(new Register());
    setupDefaultCameras();
    setupDefaultMeshes();
    setupDefaultLights();
}

void Sunshine::setupDefaultCameras()
{
    _scene->createCamera("persp");
    _scene->createCamera("side");
    _scene->createCamera("top");
    _scene->createCamera("front");
}

void Sunshine::setupDefaultMeshes()
{
    Mesh::buildByIndex(_scene, primitive::cubePrimitive(1.0f, 1.0f, 1.0f));
}

void Sunshine::setupDefaultLights()
{
}

void Sunshine::on_renderButton_clicked()
{
#if 0
    if (_renderWidget == NULL)
        _renderWidget = new RenderWidget();

    char* fileName = "/tmp/test.tif";
    _renderWidget->show();

    std::cout << _renderSettingsWidget->getValue("xres").toString().toStdString() << std::endl;


    CameraP activeCamera = activeRegister->fetchCamera("persp");


    RiBegin(RI_NULL);


    // Output image
    RiDisplay(fileName, "file", "rgb", RI_NULL);
    RiFormat(_renderSettingsWidget->getValue("xres").toInt(),
             _renderSettingsWidget->getValue("yres").toInt(),
             1);


    //CameraP activeCamera = Register::cameras()->next();

    //while (meshes.hasNext()) {
    //    meshes.next();

    // Camera type & position
    float fov = activeCamera->fov();
    //float fov = 45;
    RiProjection("perspective", "fov", &fov, RI_NULL);

    RtMatrix flipYZ;
    activeCamera->flipYZ(flipYZ);
    RiTransform(flipYZ);

    RtMatrix cameraTransform;
    //activeCamera->lookTransform(cameraTransform);
    //RiConcatTransform(cameraTransform);

    RotatePair pair = activeCamera->aim(activeCamera->lookDir());
    RiRotate(pair.rot1.x(), pair.rot1.y(), pair.rot1.z(), pair.rot1.w());
    RiRotate(pair.rot2.x(), pair.rot2.y(), pair.rot2.z(), pair.rot2.w());

    std::cout << pair.rot1 << std::endl;
    std::cout << pair.rot2 << std::endl;

    RiTranslate(-activeCamera->eye().x(),
                -activeCamera->eye().y(),
                -activeCamera->eye().z());


    //RiTransform(cameraTransform);
    //RtMatrix
    //RiTransform(transform);
    //RiTranslate(0, 0, 3);
    /*
    RtPoint camPosition = { activeCamera->eye().x(), activeCamera->eye().y(), activeCamera->eye().z() };
    RtPoint camDirection = { activeCamera->lookDir().x(), activeCamera->lookDir().y(), activeCamera->lookDir().z() };
    RiLookAt(activeCamera->eye(),
             activeCamera->lookDir(),
             activeCamera->upDir());
             */

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

   _renderWidget->open(QString(fileName));

   // std::cout << "Render to window" << std::endl;
#endif
}



void Sunshine::on_renderSettingsButton_clicked()
{
    std::cout << "Bringing up render settings" << std::endl;
    //_renderSettingsWidget->show();

}

void Sunshine::on_importAction_triggered()
{
    /*
    QString extFilter;
    QList<QString> extensions = activeRegister->importExtensions();
    foreach (QString ext, extensions) {
        std::cout << "ext: " << ext.toStdString() << std::endl;
        extFilter += QString("*") + ext + " ";
    }
    std::cout << extFilter.toStdString() << std::endl;

    QString fileName = QFileDialog::getOpenFileName(this, QString("Open File - ") + extFilter,
                                                    "/home",
                                                    QString("Meshes (") + extFilter + ")");
                                                    */
    //if (fileName != "")
      //  activeRegister->importFile(fileName);
}
