#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include "util.h"
#include <QMouseEvent>
#include <QSharedPointer>

class Camera;
typedef QSharedPointer<Camera> CameraP;

class Camera : Transformable
{
public:

    Camera(QString name);
    static QMatrix4x4            getViewMatrix(CameraP camera, int width, int height);
    static QMatrix4x4            getProjMatrix(CameraP camera, int width, int height);
    Point3                       eye();
    Vector3                      upDir();
    Point3                       lookat();
    Vector3                      lookDir();
    Vector3                      leftDir();

    QString                      name;
    int                          pickX;
    int                          pickY;
    int                          moveType;
    void                         mousePressed(QMouseEvent* event);
    void                         mouseReleased(QMouseEvent* event);
    void                         mouseDragged(QMouseEvent* event);
};
typedef QSharedPointer<Camera> CameraP;

#endif // CAMERA_H
