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

namespace Ui {
        class Sunshine;
}

//namespace GeometryMode { enum { OBJECT, VERTEX, EDGE, FACE }; };
//namespace SelectMode { enum { BASIC, LINE, BOX }; };

class PanelGL;
class CursorTool;
typedef QSharedPointer<CursorTool> CursorToolP;

namespace SunshineUi {
    CursorToolP cursorTool();
    int workMode();
    int selectMode();
    bool selectOccluded();
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
    CursorToolP                cursorTool();
    int                        workMode();
    int                        selectMode();
    bool                       selectOccluded();

protected:
    void                       changeEvent(QEvent *e);
private slots:
    void                       on_renderButton_clicked();
    void                       on_renderSettingsButton_clicked();
    void                       on_importAction_triggered();
    void                       on_layoutModeButton_released();
    void                       on_modelModeButton_released();
    void                       on_lineSelectButton_released();
    void                       on_boxSelectButton_clicked();
    void                       on_selectOccludedButton_clicked();
    void                       on_cursorToolChanged(QAbstractButton* button);

private:
    QList<PanelGL*>                   _panels;
    QButtonGroup*                     _cursorButtonGroup;
    QHash<QToolButton*,CursorToolP>   _cursorTools;
    void                              updateMode();
    Ui::Sunshine*                     ui;
    SceneP                            _scene;
    RenderWidget*                     _renderWidget;
    SettingsWidget*                   _renderSettingsWidget;
};

#endif // SUNSHINE_H
