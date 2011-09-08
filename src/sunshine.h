#ifndef SUNSHINE_H
#define SUNSHINE_H

#include <QMainWindow>
//#include <v8.h>
#include "renderwidget.h"
#include "settings.h"
#include "scene.h"
//#include "Python.h"
//#include "sunshineui.h"

namespace Ui {
        class Sunshine;
}

//namespace GeometryMode { enum { OBJECT, VERTEX, EDGE, FACE }; };
//namespace SelectMode { enum { BASIC, LINE, BOX }; };
namespace SunshineUi {
    int workMode();
    int selectMode();
};

class Sunshine : public QMainWindow {
    Q_OBJECT
public:
                               Sunshine(QWidget *parent = 0);
                               ~Sunshine();
    void                       clearScene();
    void                       setupDefaultCameras();
    void                       setupDefaultMeshes();
    void                       setupDefaultLights();
    int                        workMode();
    int                        selectMode();

protected:
    void                       changeEvent(QEvent *e);
private slots:
    void                       on_renderButton_clicked();
    void                       on_renderSettingsButton_clicked();
    void                       on_importAction_triggered();
    void                       on_layoutModeButton_released();
    void                       on_modelModeButton_released();
    void                       on_tweakModeButton_released();
    void                       on_lineSelectButton_released();
    void                       on_boxSelectButton_clicked();

private:
    void                       updateMode();
    Ui::Sunshine*              ui;
    SceneP                     _scene;
    RenderWidget*              _renderWidget;
    SettingsWidget*            _renderSettingsWidget;
};

#endif // SUNSHINE_H
