#ifndef REGISTER_H
#define REGISTER_H

#include "util.h"
#include "camera.h"
#include "geometry.h"
#include <QScriptEngine>

typedef QSharedPointer<QScriptEngine> QScriptEngineP;

class Register
{
public:
    static MeshP                       mesh(int key);
    static void                        clearScene();
    static CameraP                     createCamera(QString name);
    static MeshP                       createMesh(QString name);
    static void                        setMesh(int meshKey, MeshP mesh) { validate(); instance->_meshes[meshKey] = mesh; }
    static QHashIterator<int, MeshP>   meshes() { validate(); return QHashIterator<int,MeshP>(instance->_meshes); }
    static QHashIterator<int, CameraP> cameras() { validate(); return QHashIterator<int,CameraP>(instance->_cameras); }
    static CameraP                     fetchCamera(QString name);

protected:
                                       Register();
    static void                        validate();
    int                                uniqueCameraKey(); // TODO: replace these functions with one unique-key finder
    int                                uniqueMeshKey();
    QString                            uniqueName(QString prefix);

private:
    static Register*                   instance;
    QHash<int,MeshP>                   _meshes;
    QHash<int,CameraP>                 _cameras;
    //QHash<int,Light*>      _lights;
    QSet<QString>                      _names;
    QScriptEngineP                     _engine;
};

#endif // REGISTER_H
