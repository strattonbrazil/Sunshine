#ifndef SCENE_H
#define SCENE_H

#include "util.h"
#include "camera.h"
#include "geometry.h"
#include <QFileInfo>

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>

#include "exceptions.h"

using namespace boost::python;

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
    QList<QString>              importExtensions();
    void                        importFile(QString fileName);
    void                        evalPythonFile(QString fileName) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
        QString content = file.readAll();
        file.close();

        std::cout << content.toStdString() << std::endl;

        try {
            object ignored = exec(content.toStdString().c_str(), _pyMainNamespace);
        } catch (boost::python::error_already_set const &) {
            QString perror = parse_python_exception();
            std::cerr << "Error in Python: " << perror.toStdString() << std::endl;
        }
    }
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
    object                             _pyMainModule;
    object                             _pyMainNamespace;
public slots:
    void                               pythonStdOut(const QString &s) { std::cout << s.toStdString() << std::flush; }
    void                               pythonStdErr(const QString &s) { std::cout << s.toStdString() << std::flush; }
};
typedef QSharedPointer<Scene> SceneP;

#endif // SCENE_H
