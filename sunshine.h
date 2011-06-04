#ifndef SUNSHINE_H
#define SUNSHINE_H

#include <QMainWindow>

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

private:
    Ui::Sunshine*              ui;
};

#endif // SUNSHINE_H
