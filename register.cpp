#include "register.h"

#include <QCoreApplication>
#include <QStringList>

Register* Register::instance = NULL;

CameraP Register::fetchCamera(QString name)
{
    validate();
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
    _engine = QScriptEngineP(new QScriptEngine());
}

MeshP Register::mesh(int key)
{
    validate();
    return MeshP(instance->_meshes[key]);
}

void Register::clearScene()
{
    validate();
    instance->_meshes.clear();
    instance->_cameras.clear();
    instance->_names.clear();

    QCoreApplication::addLibraryPath("/home/stratton/sunshine/");

    // output library path
    foreach (QString path, QCoreApplication::libraryPaths()) {
        std::cout << path.toStdString() << std::endl;
    }

    // import any plugins
    instance->_engine = QScriptEngineP(new QScriptEngine());
    QScriptValue sc = instance->_engine->importExtension("core.import.obj");
    if (instance->_engine->hasUncaughtException()) {
        int line = instance->_engine->uncaughtExceptionLineNumber();
        std::cerr << "uncaught exception at line" << line << ":" << sc.toString().toStdString() << std::endl;
        //return;
    }

    sc = instance->_engine->evaluate("core.import.obj.import.extension();");
    if (instance->_engine->hasUncaughtException()) {
        int line = instance->_engine->uncaughtExceptionLineNumber();
        std::cerr << "uncaught exception at line" << line << ":" << sc.toString().toStdString() << std::endl;
    }
}

CameraP Register::createCamera(QString name)
{
    validate();
    std::cout << "Creating camera: " << name.toStdString() << std::endl;

    int key = instance->uniqueCameraKey();
    QString unique = instance->uniqueName(name);
    instance->_cameras[key] = CameraP(new Camera(unique));
    instance->_names += unique;
    return instance->_cameras[key];
}

MeshP Register::createMesh(QString name)
{
    int key = instance->uniqueMeshKey();
    QString unique = instance->uniqueName(name);
    instance->_meshes[key] = MeshP(new Mesh(key,unique));
    instance->_names += unique;
    return instance->_meshes[key];
}

void Register::validate()
{
    if (!instance) {
        instance = new Register();
    }
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
