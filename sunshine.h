#ifndef SUNSHINE_H
#define SUNSHINE_H

#include <QMainWindow>
#include "renderwidget.h"

namespace Ui {
    class Sunshine;
}

class Sunshine : public QMainWindow {
    Q_OBJECT
public:
                               Sunshine(QWidget *parent = 0);
                               ~Sunshine();
    void                       clearScene();
    void                       setupDefaultCameras();
    void                       setupDefaultMeshes();
    void                       setupDefaultLights();
protected:
    void                       changeEvent(QEvent *e);

private slots:
    void                       on_renderButton_clicked();

private:
    Ui::Sunshine*              ui;
    RenderWidget*              renderWidget;
};

#endif // SUNSHINE_H
