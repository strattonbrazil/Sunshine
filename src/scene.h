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

class SceneHierarchyModel : public QStandardItemModel
{
    Q_OBJECT

};

class Scene : public QObject
{
    Q_OBJECT
public:
    //void                            deleteMesh(QString name) { _meshes.remove(name); }
    void                            clearScene();
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
        return 0;
        //return material(materials()[0]);
    }
    QString                         assetName(Bindable* bindable) { return _assets.key(bindable); }
    QList<QString>                  importExtensions();
    void                            importFile(QString fileName);


    bool                            hasMeshSelected();
                                Scene();
                                QList<WorkTool*>                   _tools;
    bool                            setData(const QModelIndex &index, const QVariant &value, int role);

    QAbstractItemModel*             hierarchyModel() { return &_hierarchyModel; }
    QAbstractItemModel*             shaderTreeModel() { return &_shaderTreeModel; }
protected:
    int                                uniqueCameraKey(); // TODO: replace these functions with one unique-key finder
    //int                                uniqueMeshKey();
    QString                            uniqueName(QString prefix);

private:
    QHash<QString,Bindable*>           _assets;
    Material*                          _defaultMaterial;
    PythonQtObjectPtr                  pyContext;
    ShaderTreeModel                    _shaderTreeModel;
    SceneHierarchyModel                _hierarchyModel;
public slots:
    QString                            addAsset(QString name, Bindable* asset);
    void                               addMesh(QString name, Mesh* mesh) { addAsset(name, (Bindable*)mesh); }
    void                               pythonStdOut(const QString &s) { std::cout << s.toStdString() << std::flush; }
    void                               pythonStdErr(const QString &s) { std::cout << s.toStdString() << std::flush; }
};
Q_DECLARE_METATYPE(Scene*)

/*
class PythonQtWrapper_Scene : public QObject
{
    Q_OBJECT
public slots:
    Scene* new_Scene() { return new Scene(); }
    void delete_Scene(Scene* scene) { delete scene; }
    void addAsset(Scene* scene, QString name, Bindable* asset) {
        scene->addAsset(name, asset);

    }
};
*/

#endif // SCENE_H
