#ifndef SCENE_H
#define SCENE_H

#include "util.h"
#include "camera.h"
#include "geometry.h"
#include <QFileInfo>

/*
extern "C"
{
    #include "lua.h"
    #include <lualib.h>
}

#include <luabind/luabind.hpp>
*/

class Scene
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
    //QList<QString>              importExtensions();
    //void                        importFile(QString fileName);
                                Scene();
protected:
    int                                uniqueCameraKey(); // TODO: replace these functions with one unique-key finder
    int                                uniqueMeshKey();
    QString                            uniqueName(QString prefix);

private:
    QHash<int,MeshP>                   _meshes;
    QHash<int,CameraP>                 _cameras;
    //QHash<int,Light*>      _lights;
    QSet<QString>                      _names;
//    PythonQtObjectPtr                  _context;
public slots:
    void                               pythonStdOut(const QString &s) { std::cout << s.toStdString() << std::flush; }
    void                               pythonStdErr(const QString &s) { std::cout << s.toStdString() << std::flush; }
};
typedef QSharedPointer<Scene> SceneP;

#endif // SCENE_H
