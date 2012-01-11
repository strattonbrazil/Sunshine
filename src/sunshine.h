#ifndef SUNSHINE_H
#define SUNSHINE_H

#include <QMainWindow>
//#include <v8.h>
#include "renderwidget.h"
#include "settings.h"
#include "scene.h"
//#include "Python.h"
//#include "sunshineui.h"
#include "worktool.h"
#include <QToolButton>
#include "shader_tree.h"

namespace Ui {
    class Sunshine;
}

//namespace GeometryMode { enum { OBJECT, VERTEX, EDGE, FACE }; };
//namespace SelectMode { enum { BASIC, LINE, BOX }; };

class PanelGL;
class CursorTool;

namespace SunshineUi {
    Scene* activeScene();
    CursorTool* cursorTool();
    int workMode();
    void updateSceneHierarchy(Scene* scene);
    void updatePanels();
    Bindable* renderSettings();
    void showBindableAttributes(Bindable* bindable);
    void selectAsset(QString assetName);
};


class Sunshine : public QMainWindow {
    Q_OBJECT
public:
                               Sunshine(QWidget *parent = 0);
                               ~Sunshine();
    void                       clearScene();
    void                       setupDefaultMaterials();
    void                       setupDefaultCameras();
    void                       setupDefaultMeshes();
    void                       setupDefaultLights();
    Scene*                     activeScene() { return _scene; }
    CursorTool*                cursorTool();
    //    int                        workMode();
    //    int                        selectMode();
    //    bool                       selectOccluded();
    void                       updateSceneHierarchy(Scene* scene);
    void                       updatePanels();
    Bindable*                  renderSettings() { return _renderSettings; }
    void                       showBindableAttributes(Bindable* bindable);
    void                       selectAsset(QString assetName);
protected:
    void                       changeEvent(QEvent *e);
private slots:
    void                       on_renderRequest();
    void                       on_renderSettingsButton_clicked();
    void                       on_importAction_triggered();
    void                       on_layoutModeButton_released();
    void                       on_modelModeButton_released();
    void                       on_lineSelectButton_released();
    void                       on_boxSelectButton_clicked();
    void                       on_selectOccludedButton_clicked();
    void                       on_cursorToolChanged(QAbstractButton*);
    void                       on_materialSelection_changed(const QModelIndex &, const QModelIndex &);
    void                       on_sceneHierarchySelection_changed(const QModelIndex &, const QModelIndex &);
    void                       showShaderGraph();

    void                       createPointLight();
    void                       createSpotLight();
    void                       createCube();
    void                       createPlane();

private:
    QList<PanelGL*>                   _panels;
    QButtonGroup*                     _cursorButtonGroup;
    QHash<QToolButton*,CursorTool*>   _cursorTools;
    void                              updateMode();
    Ui::Sunshine*                     ui;
    Scene*                            _scene;
    RenderWidget*                     _renderWidget;
    SettingsWidget*                   _renderSettingsWidget;
    AttributeEditor*                  _propertyEditorModel;
    Bindable*                         _renderSettings;
    ShaderTreeWindow*                 _shaderTreeWindow;
};

#endif // SUNSHINE_H
