#include "sunshine.h"
#include "ui_sunshine.h"
#include "scene.h"
#include "primitive.h"
#include "settings.h"
#include <QFileDialog>
#include <QCursor>
#include "panelgl.h"
#include "sunshineui.h"
#include "object_tools.h"
#include "cursor_tools.h"
#include "render_util.h"
#include "material.h"
#include "light.h"
#include "modeltest.h"

void say_hello(const char* name) {
    std::cout << "Hello " <<  name << "!\n";
}

Sunshine* activeMainWindow = 0;

Sunshine::Sunshine(QWidget *parent) : QMainWindow(parent), ui(new Ui::Sunshine)
{
    activeMainWindow = this;


    ui->setupUi(this);

    //_shaderTreeWindow = new ShaderTreeWindow();

    clearScene();

    PanelGL* panel = new PanelGL(_scene, this);
    _panels << panel;

    ui->tabs->addTab(panel, "tab 1");
    //QObject::connect(&(ui->renderButton), SIGNAL(valueChanged(int)),
     //                     &b, SLOT(setValue(int)));

    connect(ui->renderAction, SIGNAL(triggered()),this,SLOT(renderRequest()));
    connect(ui->renderSettingsAction, SIGNAL(triggered()),this,SLOT(renderSettingsButton_clicked()));

    connect(ui->growAction, SIGNAL(triggered()),this, SLOT(growRequest()));
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
            this, SLOT(cursorToolChanged(QAbstractButton*)));
    cursorToolChanged(firstButton);

    _propertyEditorModel = new AttributeEditor();
    ui->propertyTable->setModel(_propertyEditorModel);
    ui->propertyTable->setItemDelegate(new AttributeItemDelegate());

    _renderSettings = new RenderSettings();

    // parse default aqsis shaders
    QString shaderPath(getenv("AQSIS_SHADER_PATH"));
    if (shaderPath == "")
        shaderPath = "/usr/local/share/aqsis/shaders/surface";
    QStringList sPaths = shaderPath.split(":");
    foreach(QString path, sPaths) {
        std::cout << "shader dir: " << path << std::endl;
        QStringList entries = QDir(path).entryList();
        foreach(QString entry, entries) {
            if (entry.endsWith(".slx"))
                Material::registerAqsisShader(path + "/" + entry);
        }
    }

    ui->createMenu->addAction("Point Light", this, SLOT(createPointLight()));
    ui->createMenu->addAction("Spot Light", this, SLOT(createSpotLight()));
    ui->createMenu->addSeparator();
    ui->createMenu->addAction("Cube", this, SLOT(createCube()));
    ui->createMenu->addAction("Plane", this, SLOT(createPlane()));
    ui->createMenu->addAction("Sphere", this, SLOT(createSphere()));
    ui->createMenu->addSeparator();
    foreach(QString materialName, Material::materialTypes()) {
        ui->createMenu->addAction(materialName, this, SLOT(createMaterial()));
    }
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

class ContextMenuFilter : public QObject
{
public:
    bool eventFilter( QObject *o, QEvent *e ) {
        if (e->type() == QEvent::MouseButtonPress) {
            std::cout << "here" << std::endl;
            QMouseEvent* event = dynamic_cast<QMouseEvent*>(e);
            if (event->button() == Qt::RightButton)
                return TRUE;
        }
        return FALSE;
    }
};

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
    connect(ui->sceneHierarchyTree, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showSceneHierarchyContextMenu(const QPoint&)));
    ui->sceneHierarchyTree->setContextMenuPolicy(Qt::CustomContextMenu);

    //_shaderTreeWindow->setMaterialModel(_scene->shaderTreeModel());

    ui->shaderTree->setModel(_scene->shaderTreeModel());
    //ui->shaderTree->setColumnWidth(0, 25);

    connect(ui->shaderTree->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(on_materialSelection_changed(const QModelIndex &, const QModelIndex &)));

    ui->shaderTree->installEventFilter(new ContextMenuFilter());
    ui->shaderTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->shaderTree,
            SIGNAL(customContextMenuRequested(const QPoint &)),
            this, //_scene->shaderTreeModel(),
            SLOT(shaderTreeContextMenu(const QPoint &)));
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
    /*
    Mesh* first = Mesh::buildByIndex(primitive::cubePrimitive(1.0f, 1.0f, 1.0f));
    first->setCenter(Point3(0,2,0));
    _scene->addAsset("cube1", first);

    Mesh* second = Mesh::buildByIndex(primitive::cubePrimitive(1.2f, 0.8f, 1.2f));
    second->setCenter(Point3(0,0,3));
    _scene->addAsset("cube2", second);

    Mesh* plane = Mesh::buildByIndex(primitive::planePrimitive(12,24));
    _scene->addAsset("plane", plane);
    */
    Mesh* plane = Mesh::buildByIndex(primitive::planePrimitive(12,12,2,3));
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

void Sunshine::renderRequest()
{
    if (_renderWidget == NULL)
        _renderWidget = new RenderWidget();

    //_renderWidget->show();

    //RenderUtil::renderGL(_panels[0]);
    const int XRES = SunshineUi::renderSettings()->attributeByName("Image Width")->property("value").value<int>();
    const int YRES = SunshineUi::renderSettings()->attributeByName("Image Height")->property("value").value<int>();
    //const int XRES = 637;
    //const int YRES = 408;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    RenderUtil::renderAqsis(_scene, XRES, YRES);
    QApplication::restoreOverrideCursor();

    //_renderWidget->open(QString(fileName));
}



void Sunshine::renderSettingsButton_clicked()
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
    std::cout << extFilter << std::endl;

    QString importDir = QDir::homePath();
    QString modelDir = importDir + "/Models";
    if (QDir(modelDir).exists())
        importDir = modelDir;

    QString fileName = QFileDialog::getOpenFileName(this, QString("Open File - ") + extFilter,
                                                    importDir,
                                                    QString("Meshes (") + extFilter + ")");

    if (fileName != "")
        _scene->importFile(fileName);
}

void Sunshine::layoutModeButton_released()
{
    updateMode();
}

void Sunshine::modelModeButton_released()
{
    updateMode();
}

void Sunshine::lineSelectButton_released()
{

}

void Sunshine::boxSelectButton_clicked()
{

}

void Sunshine::growRequest()
{
    std::cout << "try to expand selection of faces, edges, or vertices" << std::endl;
}

void Sunshine::showSceneHierarchyContextMenu(const QPoint &p)
{
    QMenu menu;
    menu.addAction("Look through");
    QAction* action = menu.exec(ui->sceneHierarchyTree->mapToGlobal(p));

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
    QModelIndex selectedMaterialIndex() { return activeMainWindow->selectedMaterialIndex(); }
    void expandMaterialIndex(QModelIndex index, bool expand) { activeMainWindow->expandMaterialIndex(index, expand); }
}

void Sunshine::selectOccludedButton_clicked()
{
    this->updatePanels();
}

void Sunshine::cursorToolChanged(QAbstractButton* button)
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
    QString materialName = _scene->shaderTreeModel()->data(current.sibling(current.row(), ShaderTreeModel::NAME_COLUMN), Qt::DisplayRole).toString();
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

void Sunshine::shaderTreeContextMenu(const QPoint &p)
{
    QMenu *menu = new QMenu();
    if (this->ui->shaderTree->selectionModel()->selectedRows().size() > 0) {
        QStringList materialTypes = _scene->materialTypes();
        foreach(QString matType, materialTypes) {
            menu->addAction("Add \"" + matType + "\" node", this, SLOT(on_newMaterialNode()));
        }

        //menu->addAction(tr("Test Item"));
    }
    //menu->addAction(tr("Test Item"));//, this, SLOT(test_slot()));
    if (menu->actions().size() > 0) {
        QAction* action = menu->exec(ui->shaderTree->mapToGlobal(p));
        if (action != 0) {
            if (action->text().startsWith("Add")) {
                QString matType = action->text().split("\"")[1];

                Material* material = _scene->buildMaterial(matType);
                _scene->addAsset(matType.toLower(), material);
            }
        }
    }
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
    Mesh* mesh = Mesh::buildByIndex(primitive::planePrimitive(8,8,1,1));
    _scene->addAsset("plane",mesh);
}

void Sunshine::createSphere()
{
    Mesh* mesh = Mesh::buildByIndex(primitive::spherePrimitive(1.0));
    _scene->addAsset("sphere", mesh);
}

void Sunshine::createMaterial()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action != 0) {
        Material* material = Material::buildByType(action->text());
        _scene->addAsset(action->text(), material);
    }
}

void Sunshine::showShaderGraph()
{
    //_shaderTreeWindow->show();
}

QModelIndex Sunshine::selectedMaterialIndex()
{
    if (ui->shaderTree->selectionModel() == 0)
        return QModelIndex();

    foreach(QModelIndex index, ui->shaderTree->selectionModel()->selection().indexes()) {
        return index;
    }
    return QModelIndex();
}

void Sunshine::expandMaterialIndex(QModelIndex index, bool expand)
{
    ui->shaderTree->setExpanded(index, expand);
}
