#ifndef SCENE_H
#define SCENE_H

#include "util.h"
#include "camera.h"
#include "geometry.h"
#include "material.h"
#include <QFileInfo>
#include <QStandardItemModel>

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "exceptions.h"
#include "worktool.h"
#include "contextmenu.h"
#include "shader.h"

using namespace boost::python;

class WorkTool;
class ContextAction;
typedef QSharedPointer<WorkTool> WorkToolP;


class Scene : public boost::enable_shared_from_this<Scene>, public QStandardItemModel
{
public:
    void                            deleteMesh(QString name) { _meshes.remove(name); }
    void                            clearScene();
    MaterialP                       createMaterial(QString name, MaterialP m);
    CameraP                         createCamera(QString name);
    MeshP                           createMesh(QString name);
    LightP                          createLight(QString name, LightP light);
    //void                            setMesh(QString name, MeshP mesh) { _meshes[name] = mesh; }
    QList<QString>                  meshes() { return _meshes.keys(); }
    QHashIterator<int, CameraP>     cameras() { return QHashIterator<int,CameraP>(_cameras); }
    QList<QString>                  materials() { return _materials.keys(); }
    QList<QString>                  lights() { return _lights.keys(); }
    MeshP                           mesh(QString name) { if (_meshes.contains(name)) return _meshes[name]; else return MeshP(0); }
    MaterialP                       material(QString name) { if (_materials.contains(name)) return _materials[name]; else return MaterialP(0); }
    LightP                          light(QString name) { if (_lights.contains(name)) return _lights[name]; else return LightP(0); }
    MaterialP                       defaultMaterial() { return _defaultMaterial; }
    CameraP                         fetchCamera(QString name);
    ShaderTreeModel*                shaderTreeModel() { return &_shaderTreeModel; }
    QList<QString>                  importExtensions();
    void                            importFile(QString fileName);
    void                            evalPythonFile(QString fileName) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
        QString content = file.readAll();
        file.close();

        try {
            object ignored = exec(content.toStdString().c_str(), _pyMainNamespace);
        } catch (boost::python::error_already_set const &) {
            QString perror = parse_python_exception();
            std::cerr << "Error in Python: " << perror.toStdString() << std::endl;
        }
    }

                                Scene();
                                QList<WorkToolP>                   _tools;

protected:
    int                                uniqueCameraKey(); // TODO: replace these functions with one unique-key finder
    //int                                uniqueMeshKey();
    QString                            uniqueName(QString prefix);

private:
    QHash<QString,MeshP>               _meshes;
    QHash<int,CameraP>                 _cameras;
    QHash<QString,MaterialP>           _materials;
    QHash<QString,LightP>              _lights;
    QSet<QString>                      _names;
//    PythonQtObjectPtr                  _context;
    object                             _pyMainModule;
    object                             _pyMainNamespace;
    MaterialP                          _defaultMaterial;
    ShaderTreeModel                    _shaderTreeModel;

public slots:
    void                               pythonStdOut(const QString &s) { std::cout << s.toStdString() << std::flush; }
    void                               pythonStdErr(const QString &s) { std::cout << s.toStdString() << std::flush; }
};
typedef boost::shared_ptr<Scene> SceneP;

#endif // SCENE_H
