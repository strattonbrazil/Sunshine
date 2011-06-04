#include "register.h"

Register* Register::instance = NULL;

Register::Register()
{
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
