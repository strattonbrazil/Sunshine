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
    //void                            setMesh(QString name, MeshP mesh) { _meshes[name] = mesh; }
    QList<QString>                  meshes() { return _meshes.keys(); }
    QHashIterator<int, CameraP>     cameras() { return QHashIterator<int,CameraP>(_cameras); }
    QList<QString>                  materials() { return _materials.keys(); }
    MeshP                           mesh(QString name) { return _meshes[name]; }
    MaterialP                       material(QString name) { return _materials[name]; }
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

        //std::cout << content.toStdString() << std::endl;

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
