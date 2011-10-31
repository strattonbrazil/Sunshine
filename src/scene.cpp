#include "scene.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDir>
#include <QStandardItem>
//#include <PythonQt.h>

#include "sunshine.h"
#include "exceptions.h"

#include "python_bindings.h"
#include "object_tools.h"

CameraP Scene::fetchCamera(QString name)
{
    QHashIterator<int,CameraP> cams = cameras();
    while (cams.hasNext()) {
        cams.next();
        cams.key();
        CameraP cam = cams.value();
        if (cam->name == name)
            return cam;
    }

    std::cerr << "Cannot find camera: " << name.toStdString() << std::endl;
    throw KeyErrorException();
    //return CameraP();
}


QList<QString> Scene::importExtensions()
{
    QList<QString> extensions;

    //object list = exec("MeshImporter.extensions()", _pyMainNamespace);

    try {
        object pyList = eval("MeshImporter.extensions()", _pyMainNamespace);
        std::vector<std::string> vec(len(pyList));
        for (std::size_t i = 0; i < vec.size(); ++i) {
          vec[i] = extract<std::string>(pyList[i]);
          extensions << QString(vec[i].c_str());
        }
    } catch (boost::python::error_already_set const &) {
        QString perror = parse_python_exception();
        std::cerr << "Error in Python: " << perror.toStdString() << std::endl;
    }

    return extensions;
}

void Scene::importFile(QString fileName)
{
    try {
        object processFileFunc = _pyMainModule.attr("MeshImporter").attr("processFile");
        processFileFunc(shared_from_this(), fileName);
    } catch (boost::python::error_already_set const &) {
        QString perror = parse_python_exception();
        std::cerr << "Error in Python: " << perror.toStdString() << std::endl;
    }
}

Scene::Scene()
{
    Py_Initialize();
    _pyMainModule = import("__main__");
    _pyMainNamespace = _pyMainModule.attr("__dict__");

    createPythonBindings();

    evalPythonFile(":/plugins/meshImporter.py");
    evalPythonFile(":/plugins/objImporter.py");

    _tools << WorkToolP(new TranslateTransformable());
}

MaterialP Scene::createMaterial(QString name, MaterialP material)
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

CameraP Scene::createCamera(QString name)
{
    //std::cout << "Creating camera: " << name.toStdString() << std::endl;

    int key = uniqueCameraKey();
    QString unique = uniqueName(name);
    _cameras[key] = CameraP(new Camera(unique));
    _names += unique;

    this->appendRow(new QStandardItem(QIcon(":/icons/camera_icon.png"), unique));
    SunshineUi::updateSceneHierarchy(shared_from_this());

    return _cameras[key];
}

MeshP Scene::createMesh(QString name)
{
    name = uniqueName(name);
    _meshes[name] = MeshP(new Mesh(shared_from_this(), name));
    _names += name;

    this->appendRow(new QStandardItem(QIcon(":/icons/mesh_icon.png"), name));
    SunshineUi::updateSceneHierarchy(shared_from_this());

    return _meshes[name];
}

LightP Scene::createLight(QString name, LightP light)
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
    while (_names.contains(name)) {
        name = QString("%1%2").arg(prefix).arg(counter);
        counter++;
    }
    _names += name;
    return name;
}

