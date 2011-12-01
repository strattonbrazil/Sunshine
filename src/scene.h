#ifndef SCENE_H
#define SCENE_H

#include "util.h"
#include "camera.h"
#include "geometry.h"
#include "material.h"
#include <QFileInfo>
#include <QStandardItemModel>

/*
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/enable_shared_from_this.hpp>
*/

#include "exceptions.h"
#include "worktool.h"
#include "contextmenu.h"
#include "shader.h"

//using namespace boost::python;

class WorkTool;
class ContextAction;

class Scene : public QStandardItemModel
{
    Q_OBJECT
public:
    //void                            deleteMesh(QString name) { _meshes.remove(name); }
    void                            clearScene();
    QString                         addAsset(QString name, Bindable* asset);
    //Material*                       createMaterial(QString name, Material* m);
    //Camera*                         createCamera(QString name);
    //Mesh*                           createMesh(QString name);
    //Light*                          createLight(QString name, Light* light);
    //void                            setMesh(QString name, Mesh* mesh) { _meshes[name] = mesh; }
    QList<QString>                  assetsByType(int assetType);
    QList<QString>                  meshes();// { return _meshes.keys(); }
    QList<QString>                  cameras();// { return QHashIterator<int,Camera*>(_cameras); }
    QList<QString>                  materials();// { return _materials.keys(); }
    QList<QString>                  lights();// { return _lights.keys(); }
    Mesh*                           mesh(QString name) { if (_assets.contains(name)) return qobject_cast<Mesh*>(_assets[name]); else return 0; }
    Material*                       material(QString name) { if (_assets.contains(name)) return qobject_cast<Material*>(_assets[name]); else return 0; }
    Light*                          light(QString name) { if (_assets.contains(name)) return qobject_cast<Light*>(_assets[name]); else return 0; }
    Camera*                         camera(QString name) { if (_assets.contains(name)) return qobject_cast<Camera*>(_assets[name]); else return 0; }
    Material*                       defaultMaterial() {
        return material(materials()[0]);
    }
    //Camera*                         fetchCamera(QString name);
    ShaderTreeModel*                shaderTreeModel() { return &_shaderTreeModel; }
    QList<QString>                  importExtensions();
    void                            importFile(QString fileName);
    /*
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
    */

                                Scene();
                                QList<WorkTool*>                   _tools;
    bool                            setData(const QModelIndex &index, const QVariant &value, int role);


protected:
    int                                uniqueCameraKey(); // TODO: replace these functions with one unique-key finder
    //int                                uniqueMeshKey();
    QString                            uniqueName(QString prefix);

private:
    QHash<QString,Bindable*>           _assets;
    //QHash<QString,Mesh*>               _meshes;
    //QHash<int,Camera*>                 _cameras;
    //QHash<QString,Material*>           _materials;
    //QHash<QString,Light*>              _lights;
    //QSet<QString>                      _names;
//    PythonQtObjectPtr                  _context;
    //object                             _pyMainModule;
    //object                             _pyMainNamespace;
    Material*                          _defaultMaterial;
    ShaderTreeModel                    _shaderTreeModel;
    PythonQtObjectPtr                  pyContext;

public slots:
    void                               pythonStdOut(const QString &s) { std::cout << s.toStdString() << std::flush; }
    void                               pythonStdErr(const QString &s) { std::cout << s.toStdString() << std::flush; }
};
//Q_DECLARE_METATYPE(QSharedPointer<Scene>);
Q_DECLARE_METATYPE(Scene*)


//typedef boost::shared_ptr<Scene> Scene*;

#endif // SCENE_H
