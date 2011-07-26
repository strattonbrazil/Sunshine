#include "register.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDir>
#include <PythonQt.h>

CameraP Register::fetchCamera(QString name)
{
    QHashIterator<int,CameraP> cams = cameras();
    while (cams.hasNext()) {
        cams.next();
        cams.key();
        CameraP cam = cams.value();
        if (cam->name == name)
            return cam;
    }
    return CameraP();
}

Register::Register()
{
    PythonQt::init();
}

MeshP Register::mesh(int key)
{
    return MeshP(_meshes[key]);
}

/*
void Register::clearScene()
{
    _meshes.clear();
    _cameras.clear();
    _names.clear();

    QCoreApplication::addLibraryPath("/home/stratton/sunshine/");

    // output library path
    foreach (QString path, QCoreApplication::libraryPaths()) {
        std::cout << path.toStdString() << std::endl;
    }

    // import any plugins
    instance->_engine = QScriptEngineP(new QScriptEngine());
    QScriptValue sc = instance->_engine->importExtension("core.importer.obj");
    if (instance->_engine->hasUncaughtException()) {
        sc = instance->_engine->uncaughtException();
        int line = instance->_engine->uncaughtExceptionLineNumber();
        std::cerr << "uncaught exception at line: " << line << " (" << sc.toString().toStdString() << ")" << std::endl;
        //return;
    }

    sc = instance->_engine->evaluate("core.importer.obj.extension;");
    if (instance->_engine->hasUncaughtException()) {
        int line = instance->_engine->uncaughtExceptionLineNumber();
        std::cerr << "uncaught exception at line" << line << ":" << sc.toString().toStdString() << std::endl;
    }
    std::cerr << sc.toString().toStdString() << std::endl;
}
*/

CameraP Register::createCamera(QString name)
{
    std::cout << "Creating camera: " << name.toStdString() << std::endl;

    int key = uniqueCameraKey();
    QString unique = uniqueName(name);
    _cameras[key] = CameraP(new Camera(unique));
    _names += unique;
    return _cameras[key];
}

MeshP Register::createMesh(QString name)
{
    int key = uniqueMeshKey();
    QString unique = uniqueName(name);
    _meshes[key] = MeshP(new Mesh(key,unique));
    _names += unique;
    return _meshes[key];
}

int Register::uniqueCameraKey()
{
    int counter = 0;
    while (_cameras.contains(counter))
        counter++;
    return counter;
}

int Register::uniqueMeshKey()
{
    int counter = 0;
    while (_meshes.contains(counter))
        counter++;
    return counter;
}

QString Register::uniqueName(QString prefix)
{
    int counter = 1;
    QString name = prefix;
    while (_names.contains(name)) {
        name = QString("%s%d").arg(prefix).arg(counter);
        counter++;
    }
    _names += name;
    return name;
}
