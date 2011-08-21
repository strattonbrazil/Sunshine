#ifndef SUNSHINE_H
#define SUNSHINE_H

#include <QMainWindow>
//#include <v8.h>
//#include "renderwidget.h"
#include "settings.h"
#include "scene.h"
//#include "Python.h"

namespace Ui {
    class Sunshine;
}

namespace GeometryMode { enum { OBJECT, VERTEX, EDGE, FACE }; };
namespace SelectMode { enum { BASIC, LINE, BOX }; };

class Sunshine : public QMainWindow {
    Q_OBJECT
public:
                               Sunshine(QWidget *parent = 0);
                               ~Sunshine();
    void                       clearScene();
    void                       setupDefaultCameras();
    void                       setupDefaultMeshes();
    void                       setupDefaultLights();
    static int                 geometryMode() { return _geometryMode; }
    static int                 selectMode() { return _selectMode; }
protected:
    void                       changeEvent(QEvent *e);
private slots:
    void                       on_renderButton_clicked();
    void                       on_renderSettingsButton_clicked();
    void                       on_importAction_triggered();
private:
    Ui::Sunshine*              ui;
    Scene*                     scene;
    //RenderWidget*              _renderWidget;
//    SettingsWidget*            _renderSettingsWidget;
    static int                 _geometryMode;
    static int                 _selectMode;
    //static v8::HandleScope     _handleScope;
};

#endif // SUNSHINE_H
