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
#include "object_tools.h"
#include "cursor_tools.h"
#include "render_util.h"
#include "material.h"
#include "modeltest.h"

void say_hello(const char* name) {
    std::cout << "Hello " <<  name << "!\n";
}

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>

using namespace boost::python;

Sunshine* activeMainWindow = 0;

Sunshine::Sunshine(QWidget *parent) : QMainWindow(parent), ui(new Ui::Sunshine)
{
    activeMainWindow = this;

    ui->setupUi(this);
    clearScene();

    PanelGL* panel = new PanelGL(_scene, this);
    _panels << panel;

    ui->tabs->addTab(panel, "tab 1");
    //QObject::connect(&(ui->renderButton), SIGNAL(valueChanged(int)),
     //                     &b, SLOT(setValue(int)));

    _renderWidget = 0;
    _renderSettingsWidget = new SettingsWidget();

    QList<CursorToolP> cursorTools;
    cursorTools << CursorToolP(new PointTool());
    cursorTools << CursorToolP(new EditTool());
    cursorTools << CursorToolP(new DrawBoxTool());
    cursorTools << CursorToolP(new TranslateTool());
    cursorTools << CursorToolP(new RotateTool());
    cursorTools << CursorToolP(new PushPullTool());

    QToolButton* firstButton = 0;
    _cursorButtonGroup = new QButtonGroup();
    foreach(CursorToolP tool, cursorTools) {
        QToolButton* button = new QToolButton();
        if (!firstButton) firstButton = button;
        _cursorButtonGroup->addButton(button);
        button->setIcon(tool->icon());
        button->setToolTip(tool->label());
        button->setCheckable(TRUE);
        ui->toolBar->addWidget(button);

        _cursorTools[button] = tool;
    }
    firstButton->setChecked(TRUE);
    connect(_cursorButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(on_cursorToolChanged(QAbstractButton*)));

    _propertyEditorModel = new AttributeEditor();
    ui->propertyTable->setModel(_propertyEditorModel);

}

Sunshine::~Sunshine()
{
    delete ui;
    if (_renderSettingsWidget) delete _renderSettingsWidget;
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
    setupDefaultMaterials();
    setupDefaultCameras();
    setupDefaultMeshes();
    setupDefaultLights();

    ShaderTreeModel* m = new ShaderTreeModel();
    //new ModelTest(m);

    ui->sceneHierarchyTree->setModel(_scene.get());
    ui->shaderTree->setModel(_scene->shaderTreeModel());
    ui->shaderTree->setColumnWidth(0, 25);
    connect(ui->shaderTree->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(on_materialSelection_changed(const QModelIndex &, const QModelIndex &)));
    //std::cout << ui->sceneHierarchyTree << std::endl;
}

void Sunshine::setupDefaultMaterials()
{
    _scene->createMaterial("defaultPhong", MaterialP(new PhongMaterial()));
    _scene->createMaterial("otherPhong", MaterialP(new PhongMaterial()));
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

    RenderUtil::renderGL(_panels[0]);


    /*
    if (_renderWidget == NULL)
        _renderWidget = new RenderWidget();

    char* fileName = "/tmp/test.tif";
    _renderWidget->show();

    std::cout << (*_renderSettingsWidget)["xres"].toInt() << std::endl;
    //std::cout << _renderSettingsWidget->getValue("xres").toString().toStdString() << std::endl;


    CameraP activeCamera = _scene->fetchCamera("persp");

    RiBegin(RI_NULL);

    // Output image
    RiDisplay(fileName, "file", "rgb", RI_NULL);
    RiFormat((*_renderSettingsWidget)["xres"].toInt(),
             (*_renderSettingsWidget)["yres"].toInt(),
             1);


    //CameraP activeCamera = Register::cameras()->next();

    //while (meshes.hasNext()) {
    //    meshes.next();

    // Camera type & position
    float fov = activeCamera->fov();
    //float fov = 45;
    RiProjection("perspective", "fov", &fov, RI_NULL);

    // flip coordinate system across YZ
    RtMatrix flipYZ;
    activeCamera->flipYZ(flipYZ);
    RiTransform(flipYZ);

    // setup directional light
    {
        Point3 eye = activeCamera->eye();
        Point3 shineAt = eye + -activeCamera->lookDir().normalized();
        RtPoint from = {eye.x(),eye.y(),eye.z()};
        RtPoint to = {shineAt.x(),shineAt.y(),shineAt.z()};
        RiLightSource("distantlight", "from", from, "to", to, RI_NULL);
    }
    //    RiLightSource("pointlight", "intensity", &intensity, RI_NULL);

    RotatePair pair = activeCamera->aim(activeCamera->lookDir());
    RiRotate(pair.rot1.x(), pair.rot1.y(), pair.rot1.z(), pair.rot1.w());
    RiRotate(pair.rot2.x(), pair.rot2.y(), pair.rot2.z(), pair.rot2.w());

    RiTranslate(-activeCamera->eye().x(),
                -activeCamera->eye().y(),
                -activeCamera->eye().z());

    RiWorldBegin();
    {
        // setup basic material
        float intensity = 1.0f;
        float Ks = 1.0f;
        RiSurface("plastic", "Ks", &Ks, RI_NULL);

        // add meshes to scene
        QHashIterator<int,MeshP> meshes = _scene->meshes();
        while (meshes.hasNext()) {
            meshes.next();
            int meshKey = meshes.key();
            MeshP mesh = meshes.value();
            RiTransformBegin(); // object-to-world
            {
                const int numTriangles = mesh->numTriangles();
                QHashIterator<int,FaceP> i = mesh->faces();
                while (i.hasNext()) { // render each face
                    i.next();
                    FaceP face = i.value();
                    QListIterator<Triangle> j = face->buildTriangles();
                    while (j.hasNext()) { // render each triangle
                        Triangle triangle = j.next();
                        RtPoint points[3] = { { triangle.a->vert()->pos().x(),
                                                triangle.a->vert()->pos().y(),
                                                triangle.a->vert()->pos().z() },
                                              { triangle.b->vert()->pos().x(),
                                                triangle.b->vert()->pos().y(),
                                                triangle.b->vert()->pos().z() },
                                              { triangle.c->vert()->pos().x(),
                                                triangle.c->vert()->pos().y(),
                                                triangle.c->vert()->pos().z() } };
                        RtPoint normals[3] = { { triangle.a->normal().x(),
                                                 triangle.a->normal().y(),
                                                 triangle.a->normal().z() },
                                               { triangle.b->normal().x(),
                                                 triangle.b->normal().y(),
                                                 triangle.b->normal().z() },
                                               { triangle.c->normal().x(),
                                                 triangle.c->normal().y(),
                                                 triangle.c->normal().z() } };
                        RiPolygon(3, "P", (RtPointer)points, "N", (RtPointer)normals, RI_NULL );
                    }
                }

            }
            RiTransformEnd();
        }

        // Geometry
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
    }
    RiWorldEnd();

    RiEnd();

   _renderWidget->open(QString(fileName));
   */
}



void Sunshine::on_renderSettingsButton_clicked()
{
    _renderSettingsWidget->show();
}

void Sunshine::on_importAction_triggered()
{
    QString extFilter;
    QList<QString> extensions = _scene->importExtensions();

    foreach (QString ext, extensions) {
        std::cout << "ext: " << ext.toStdString() << std::endl;
        extFilter += QString("*") + ext + " ";
    }

    QString fileName = QFileDialog::getOpenFileName(this, QString("Open File - ") + extFilter,
                                                    "/home",
                                                    QString("Meshes (") + extFilter + ")");

    if (fileName != "")
        _scene->importFile(fileName);
}

void Sunshine::on_layoutModeButton_released()
{
    updateMode();
}

void Sunshine::on_modelModeButton_released()
{
    updateMode();
}

void Sunshine::on_lineSelectButton_released()
{

}

void Sunshine::on_boxSelectButton_clicked()
{

}

void Sunshine::updateMode()
{
    foreach (PanelGL* panel, _panels)
        panel->update();

    // hide selection mode buttons in tweak mode
    //if (Sunshine::workMode() != WorkMode::TWEAK) ui->selectFrame->show();
    //else ui->selectFrame->hide();
}

CursorToolP Sunshine::cursorTool()
{
    QToolButton* button = qobject_cast<QToolButton*>(_cursorButtonGroup->checkedButton());
    return _cursorTools[button];
}

int Sunshine::workMode()
{
    if (ui->layoutModeButton->isChecked()) return WorkMode::LAYOUT;
    else if (ui->modelModeButton->isChecked()) return WorkMode::MODEL;
}

int Sunshine::selectMode()
{
    if (ui->lineSelectButton->isChecked()) return SelectMode::LINE;
    else if (ui->boxSelectButton->isChecked()) return SelectMode::BOX;
}

bool Sunshine::selectOccluded()
{
    return ui->selectOccludedButton->isChecked();
}

void Sunshine::updateSceneHierarchy(SceneP scene)
{
    //ui->sceneHierarchyTree
}

namespace SunshineUi {
    SceneP activeScene() { return activeMainWindow->activeScene(); }
    int workMode() { return activeMainWindow->workMode(); }
    int selectMode() { return activeMainWindow->selectMode(); }
    bool selectOccluded() { return activeMainWindow->selectOccluded(); }
    CursorToolP cursorTool() { return activeMainWindow->cursorTool(); }
    void updateSceneHierarchy(SceneP scene) { return activeMainWindow->updateSceneHierarchy(scene); }
}

void Sunshine::on_selectOccludedButton_clicked()
{
    foreach (PanelGL* panel, _panels)
        panel->update();
}

void Sunshine::on_cursorToolChanged(QAbstractButton* button)
{
    foreach (PanelGL* panel, _panels)
        panel->update();
}

void Sunshine::on_materialSelection_changed(const QModelIndex &current, const QModelIndex &previous )
{
    QString materialName = _scene->shaderTreeModel()->data(current.sibling(current.row(), 1), Qt::DisplayRole).toString();
    MaterialP material = _scene->material(materialName);

    _propertyEditorModel->update(material->attributes());
    //ui->shaderTree->selectionModel()->
    std::cout << "Material selection changed: " << materialName.toStdString() << std::endl;
}
