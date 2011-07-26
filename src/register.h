#ifndef REGISTER_H
#define REGISTER_H

#include "util.h"
#include "camera.h"
#include "geometry.h"
#include <QScriptEngine>
#include <QFileInfo>
#include <PythonQt.h>

typedef QSharedPointer<QScriptEngine> QScriptEngineP;

class Register
{
public:
    MeshP                       mesh(int key);
    void                        clearScene();
    CameraP                     createCamera(QString name);
    MeshP                       createMesh(QString name);
    void                        setMesh(int meshKey, MeshP mesh) { _meshes[meshKey] = mesh; }
    QHashIterator<int, MeshP>   meshes() { return QHashIterator<int,MeshP>(_meshes); }
    QHashIterator<int, CameraP> cameras() { return QHashIterator<int,CameraP>(_cameras); }
    CameraP                     fetchCamera(QString name);
                                Register();
protected:
    int                                uniqueCameraKey(); // TODO: replace these functions with one unique-key finder
    int                                uniqueMeshKey();
    QString                            uniqueName(QString prefix);

private:
    QHash<int,MeshP>                   _meshes;
    QHash<int,CameraP>                 _cameras;
    //QHash<int,Light*>      _lights;
    QSet<QString>                      _names;
};
typedef QSharedPointer<Register> RegisterP;

#endif // REGISTER_H
