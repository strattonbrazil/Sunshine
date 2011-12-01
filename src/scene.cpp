#include "scene.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDir>
#include <QStandardItem>
#include <PythonQt.h>

#include "sunshine.h"
#include "exceptions.h"

#include "python_bindings.h"
#include "object_tools.h"

QList<QString> Scene::assetsByType(int assetType)
{
    QList<QString> meshAssets;
    foreach(QString assetName, _assets.keys()) {
        if (_assets[assetName]->assetType() == assetType)
            meshAssets << assetName;
    }
    return meshAssets;
}

QList<QString> Scene::meshes()
{
    return assetsByType(AssetType::MESH_ASSET);
}

QList<QString> Scene::lights()
{
    return assetsByType(AssetType::LIGHT_ASSET);
}

QList<QString> Scene::materials()
{
    return assetsByType(AssetType::MATERIAL_ASSET);
}

/*
Camera* Scene::fetchCamera(QString name)
{
    QHashIterator<int,Camera*> cams = cameras();
    while (cams.hasNext()) {
        cams.next();
        cams.key();
        Camera* cam = cams.value();
        if (cam->name == name)
            return cam;
    }

    std::cerr << "Cannot find camera: " << name.toStdString() << std::endl;
    throw KeyErrorException();
    //return Camera*();
}
*/


QList<QString> Scene::importExtensions()
{
    QList<QString> extensions;
    QVariant result = pyContext.call("MeshImporter.extensions");
    if (!result.isValid())
        std::cerr << "Scene::importExtensions() - invalid return value";
    QList<QVariant> exts = result.toList();
    foreach(QVariant ext, exts) {
        extensions << ext.toString();
    }

    return extensions;
}

void Scene::importFile(QString fileName)
{
    QVariant sceneV = qVariantFromValue(this);

    QVariantList fileArgs;
    fileArgs << qVariantFromValue(this);
    fileArgs << fileName;

    pyContext.call("MeshImporter.processFile", fileArgs);

    /*
    try {
        object processFileFunc = _pyMainModule.attr("MeshImporter").attr("processFile");
        processFileFunc(shared_from_this(), fileName);
    } catch (boost::python::error_already_set const &) {
        QString perror = parse_python_exception();
        std::cerr << "Error in Python: " << perror.toStdString() << std::endl;
    }
    */
}

Scene::Scene()
{
    //Py_Initialize();

    //_pyMainModule = import("__main__");
    //_pyMainNamespace = _pyMainModule.attr("__dict__");

    createPythonBindings();

    //evalPythonFile(":/plugins/meshImporter.py");
    //evalPythonFile(":/plugins/objImporter.py");

    pyContext = PythonQt::self()->getMainModule();

    // do something
    pyContext.evalScript("def multiply(a,b):\n  return a*b;\n");
    QVariantList args;
    args << 42 << 47;
    QVariant result = pyContext.call("multiply", args);
    std::cout << result.toString().toStdString() << std::endl;

    pyContext.evalFile(":/plugins/meshImporter.py");
    pyContext.evalFile(":/plugins/objImporter.py");



    _tools << new TranslateTransformable();
}

QString Scene::addAsset(QString name, Bindable* asset)
{
    QString unique = uniqueName(name);
    _assets[unique] = asset;

    QIcon icon;
    if (asset->assetType() == AssetType::MESH_ASSET)
        icon = QIcon(":/icons/mesh_icon.png");
    else if (asset->assetType() == AssetType::MATERIAL_ASSET)
        icon = QIcon(":/icons/material_icon.png");
    else if (asset->assetType() == AssetType::CAMERA_ASSET)
        icon = QIcon(":/icons/camera_icon.png");
    else if (asset->assetType() == AssetType::LIGHT_ASSET)
        icon = QIcon(":/icons/point_light_icon.png");

    appendRow(new QStandardItem(icon, unique));

    SunshineUi::selectAsset(unique);

    return unique;
}

/*
Material* Scene::createMaterial(QString name, Material* material)
{
    if (!_defaultMaterial)
        _defaultMaterial = material;

    QString unique = uniqueName(name);
    _materials[unique] = material;

    appendRow(new QStandardItem(QIcon(":/icons/material_icon.png"), unique));

    QList<QStandardItem*> materialRow;
    materialRow << new QStandardItem("");
    materialRow << new QStandardItem(unique);
    materialRow << new QStandardItem("-------");
    _shaderTreeModel.appendRow(materialRow);

    SunshineUi::updateSceneHierarchy(shared_from_this());

    return material;
}
*/

bool Scene::setData(const QModelIndex &index, const QVariant &value, int role)
{
    /*
    QString oldName = index.data(role).toString();
    QString newName = value.toString();
    if (_names.contains(newName)) // must maintain unique names
        return false;

    _names.remove(oldName);
    _names.insert(newName);
    */
    return true;

    return QStandardItemModel::setData(index, value, role);
}

/*
Camera* Scene::createCamera(QString name)
{
    //std::cout << "Creating camera: " << name.toStdString() << std::endl;

    int key = uniqueCameraKey();
    QString unique = uniqueName(name);
    _cameras[key] = Camera*(new Camera(unique));
    _names += unique;

    this->appendRow(new QStandardItem(QIcon(":/icons/camera_icon.png"), unique));
    SunshineUi::updateSceneHierarchy(shared_from_this());

    return _cameras[key];
}

Mesh* Scene::createMesh(QString name)
{
    name = uniqueName(name);
    _meshes[name] = Mesh*(new Mesh());
    _names += name;

    this->appendRow(new QStandardItem(QIcon(":/icons/mesh_icon.png"), name));
    SunshineUi::updateSceneHierarchy(shared_from_this());

    return _meshes[name];
}

Light* Scene::createLight(QString name, Light* light)
{
    name = uniqueName(name);
    _lights[name] = light;
    _names += name;

    this->appendRow(new QStandardItem(QIcon(":/icons/point_light_icon.png"), name));

    return _lights[name];
}

int Scene::uniqueCameraKey()
{
    int counter = 0;
    while (_cameras.contains(counter))
        counter++;
    return counter;
}
*/

/*
int Scene::uniqueMeshKey()
{
    int counter = 0;
    while (_meshes.contains(counter))
        counter++;
    return counter;
}
*/

QString Scene::uniqueName(QString prefix)
{
    int counter = 1;
    QString name = prefix;
    while (_assets.contains(name)) {
        name = QString("%1%2").arg(prefix).arg(counter);
        counter++;
    }

    return name;
}

