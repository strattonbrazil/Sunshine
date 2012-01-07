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
#include "light.h"
#include "modeltest.h"
#include <PythonQt.h>

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

    PythonQt::init(PythonQt::IgnoreSiteModule);



    ui->setupUi(this);

    _shaderTreeWindow = new ShaderTreeWindow();

    clearScene();

    PanelGL* panel = new PanelGL(_scene, this);
    _panels << panel;

    ui->tabs->addTab(panel, "tab 1");
    //QObject::connect(&(ui->renderButton), SIGNAL(valueChanged(int)),
     //                     &b, SLOT(setValue(int)));

    connect(ui->renderAction, SIGNAL(triggered()),this,SLOT(on_renderRequest()));
    connect(ui->renderSettingsAction, SIGNAL(triggered()),this,SLOT(on_renderSettingsButton_clicked()));



    connect(ui->shaderGraphAction, SIGNAL(triggered()),this,SLOT(showShaderGraph()));

    _renderWidget = 0;
    _renderSettingsWidget = new SettingsWidget();

    QList<CursorTool*> cursorTools;
    cursorTools << new PointTool();
    cursorTools << new DrawBoxTool();
    cursorTools << new TranslateTool();
    cursorTools << new RotateTool();
    cursorTools << new PushPullTool();

    QToolButton* firstButton = 0;
    _cursorButtonGroup = new QButtonGroup();
    foreach(CursorTool* tool, cursorTools) {
        QToolButton* button = new QToolButton();

        //QMenu* menu = new QMenu();
        //menu->addAction("Some action");

        //button->style()->styleHint()
        //button->style()
        //styleHint(QStyle::SH_ToolButton_PopupDelay, 0, parent);
        //button->setPopupMode(QToolButton::MenuButtonPopup);
        //button->setMenu(menu);

        if (firstButton == 0) firstButton = button;
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
    on_cursorToolChanged(firstButton);

    _propertyEditorModel = new AttributeEditor();
    ui->propertyTable->setModel(_propertyEditorModel);
    ui->propertyTable->setItemDelegate(new AttributeItemDelegate());

    _renderSettings = new RenderSettings();


    ui->createMenu->addAction("Point Light", this, SLOT(createPointLight()));
    ui->createMenu->addAction("Spot Light", this, SLOT(createSpotLight()));
    ui->createMenu->addSeparator();
    ui->createMenu->addAction("Cube", this, SLOT(createCube()));
    ui->createMenu->addAction("Plane", this, SLOT(createPlane()));

}

Sunshine::~Sunshine()
{
    delete ui;
    delete _shaderTreeWindow;
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
    _scene = new Scene();
    //activeRegister = RegisterP(new Register());
    setupDefaultMaterials();
    setupDefaultCameras();
    setupDefaultMeshes();
    setupDefaultLights();

    ui->sceneHierarchyTree->setModel(_scene->hierarchyModel());
    connect(ui->sceneHierarchyTree->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(on_sceneHierarchySelection_changed(const QModelIndex &, const QModelIndex &)));

    _shaderTreeWindow->setMaterialModel(_scene->shaderTreeModel());

    //ui->shaderTree->setModel(_scene->shaderTreeModel());
    //ui->shaderTree->setColumnWidth(0, 25);

    //connect(ui->shaderTree->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
     //       this, SLOT(on_materialSelection_changed(const QModelIndex &, const QModelIndex &)));


    ui->shaderTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->shaderTree,
            SIGNAL(customContextMenuRequested(const QPoint &)),
            _scene->shaderTreeModel(),
            SLOT(contextMenu(const QPoint &)));
    //std::cout << ui->sceneHierarchyTree << std::endl;
}

void Sunshine::setupDefaultMaterials()
{
    for (int i = 0; i < 5; i++)
        _scene->addAsset("phong", new PhongNode());
    //_scene->addAsset("phong", new PhongMaterial());
    //_scene->addAsset("otherPhong", new PhongMaterial());
}

void Sunshine::setupDefaultCameras()
{
    _scene->addAsset("persp", new Camera());
    _scene->addAsset("side", new Camera());
    _scene->addAsset("top", new Camera());
    _scene->addAsset("front", new Camera());
}

void Sunshine::setupDefaultMeshes()
{
    Mesh* first = Mesh::buildByIndex(primitive::cubePrimitive(1.0f, 1.0f, 1.0f));
    first->setCenter(Point3(0,2,0));
    _scene->addAsset("cube1", first);

    Mesh* second = Mesh::buildByIndex(primitive::cubePrimitive(1.2f, 0.8f, 1.2f));
    second->setCenter(Point3(0,0,3));
    _scene->addAsset("cube2", second);

    Mesh* plane = Mesh::buildByIndex(primitive::planePrimitive(12,24));
    _scene->addAsset("plane", plane);
}

void Sunshine::setupDefaultLights()
{
    //Light* point(new PointLight());
    //_scene->addAsset("point", point);
    //point->setCenter(Point3(0,6,0));

    Light* spot(new SpotLight());
    _scene->addAsset("spot", spot);
    spot->setCenter(Point3(0,1,0));

    SpotLight* ack = qobject_cast<SpotLight*>(spot);

    //std::cout << ack->spotDir() << std::endl;

    Light* ambient(new AmbientLight());
    _scene->addAsset("ambient", ambient);

    //Light* spot = _scene->createLight("spot", Light*(new SpotLight()));
    //spot->orient(Point3(8,4,-8), Point3(0,0,0), Vector3(0,1,0));
}

void Sunshine::on_renderRequest()
{

    RenderUtil::renderGL(_panels[0]);


    /*
    if (_renderWidget == NULL)
        _renderWidget = new RenderWidget();

    char* fileName = "/tmp/test.tif";
    _renderWidget->show();

    std::cout << (*_renderSettingsWidget)["xres"].toInt() << std::endl;
    //std::cout << _renderSettingsWidget->getValue("xres").toString().toStdString() << std::endl;


    Camera* activeCamera = _scene->fetchCamera("persp");

    RiBegin(RI_NULL);

    // Output image
    RiDisplay(fileName, "file", "rgb", RI_NULL);
    RiFormat((*_renderSettingsWidget)["xres"].toInt(),
             (*_renderSettingsWidget)["yres"].toInt(),
             1);


    //Camera* activeCamera = Register::cameras()->next();

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
        QHashIterator<int,Mesh*> meshes = _scene->meshes();
        while (meshes.hasNext()) {
            meshes.next();
            int meshKey = meshes.key();
            Mesh* mesh = meshes.value();
            RiTransformBegin(); // object-to-world
            {
                const int numTriangles = mesh->numTriangles();
                QHashIterator<int,Face*> i = mesh->faces();
                while (i.hasNext()) { // render each face
                    i.next();
                    Face* face = i.value();
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
    _propertyEditorModel->update(_renderSettings);

    ui->propertyTable->expandAll();
    ui->propertyTable->resizeColumnToContents(0);
}

void Sunshine::on_importAction_triggered()
{
    QString extFilter;
    QList<QString> extensions = _scene->importExtensions();

    foreach (QString ext, extensions) {
        extFilter += QString("*") + ext + " ";
    }
    //std::cout << extFilter << std::endl;

    QString importDir = QDir::homePath();
    QString modelDir = importDir + "/models";
    if (QDir(modelDir).exists())
        importDir = modelDir;

    QString fileName = QFileDialog::getOpenFileName(this, QString("Open File - ") + extFilter,
                                                    importDir,
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

CursorTool* Sunshine::cursorTool()
{
    QToolButton* button = qobject_cast<QToolButton*>(_cursorButtonGroup->checkedButton());
    return _cursorTools[button];
}

void Sunshine::updateSceneHierarchy(Scene* scene)
{
    //ui->sceneHierarchyTree
}

void Sunshine::updatePanels()
{
    foreach (PanelGL* panel, _panels)
        panel->update();
}

namespace SunshineUi {
    Scene* activeScene() { return activeMainWindow->activeScene(); }
    int workMode() { return activeMainWindow->cursorTool()->workMode(); }
    CursorTool* cursorTool() { return activeMainWindow->cursorTool(); }
    void updateSceneHierarchy(Scene* scene) { return activeMainWindow->updateSceneHierarchy(scene); }
    void updatePanels() { activeMainWindow->updatePanels(); }
    Bindable* renderSettings() { return activeMainWindow->renderSettings(); }
    void showBindableAttributes(Bindable *bindable) { activeMainWindow->showBindableAttributes(bindable); }
    void selectAsset(QString assetName) { activeMainWindow->selectAsset(assetName); }
}

void Sunshine::on_selectOccludedButton_clicked()
{
    this->updatePanels();
}

void Sunshine::on_cursorToolChanged(QAbstractButton* button)
{
    // clear out the frame/layout
    QLayoutItem* child;
    while ((child = ui->cursorToolsLayout->takeAt(0)) != 0) {}

    if (cursorTool()->toolbar() != 0)
        ui->cursorToolsLayout->addWidget(cursorTool()->toolbar());

    this->updatePanels();
}

void Sunshine::on_materialSelection_changed(const QModelIndex &current, const QModelIndex &previous)
{
    QString materialName = _scene->shaderTreeModel()->data(current.sibling(current.row(), 1), Qt::DisplayRole).toString();
    Material* material = _scene->material(materialName);

    _propertyEditorModel->update(material);
    ui->propertyTable->expandAll();
    ui->propertyTable->resizeColumnToContents(0);
}

void Sunshine::on_sceneHierarchySelection_changed(const QModelIndex &current, const QModelIndex &previous)
{
    // get the selection if its bindable
    QString bindableName = _scene->hierarchyModel()->data(current, Qt::DisplayRole).toString();
    Bindable* bindable = _scene->light(bindableName);
    if (bindable == 0) bindable = _scene->mesh(bindableName);
    if (bindable == 0) bindable = _scene->material(bindableName);
    //if (bindable == 0) bindable = _scene->material(bindableName);

    // update the property editor
    if (bindable != 0) {
        _propertyEditorModel->update(bindable);

        // try selecting the bindable
        //
        foreach(QString lightName, _scene->lights()) {
            _scene->light(lightName)->setSelected(false);
        }
        foreach(QString meshName, _scene->meshes()) {
            _scene->mesh(meshName)->setSelected(false);
        }

        Mesh* mesh = qobject_cast<Mesh*>(bindable);
        if (mesh != 0) mesh->setSelected(true);
        Light* light = qobject_cast<Light*>(bindable);
        if (light != 0) light->setSelected(true);

        updatePanels();
    }

    ui->propertyTable->expandAll();
    ui->propertyTable->resizeColumnToContents(0);
}

void Sunshine::showBindableAttributes(Bindable *bindable)
{
    _propertyEditorModel->update(bindable);
    ui->propertyTable->expandAll();
    ui->propertyTable->resizeColumnToContents(0);
}

void Sunshine::selectAsset(QString assetName)
{
    if (ui->propertyTable->selectionModel() == 0)
        return;

    /*
    QList<QStandardItem*> items = _scene->findItems(assetName);
    if (items.size() > 0) {
        QModelIndex index = _scene->indexFromItem(items[0]);
        std::cout << index.row() << std::endl;
        ui->propertyTable->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
        std::cout << ui->propertyTable->selectionModel()->currentIndex().row() << std::endl;
        //ui->propertyTable->selectionModel()->select(QItemSelection(index, index), QItemSelectionModel::Select);

        //ui->propertyTable->setCurrentIndex(index);
        //ui->propertyTable->selectionModel()->select(index,
          //                                                   QItemSelectionModel::Select);
    }
    */

}

void Sunshine::createPointLight()
{
    _scene->addAsset("point", new PointLight());
}

void Sunshine::createSpotLight()
{
    _scene->addAsset("spot", new SpotLight());
}

void Sunshine::createCube()
{
    Mesh* mesh = Mesh::buildByIndex(primitive::cubePrimitive(1.0f, 1.0f, 1.0f));
    _scene->addAsset("cube",mesh);
}

void Sunshine::createPlane()
{
    Mesh* mesh = Mesh::buildByIndex(primitive::planePrimitive(8,8));
    _scene->addAsset("plane",mesh);
}

void Sunshine::showShaderGraph()
{
    _shaderTreeWindow->show();
}
