#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include "util.h"
#include <QMouseEvent>
#include <QSharedPointer>
#include <aqsis/ri/ri.h>

class Camera;
typedef QSharedPointer<Camera> CameraP;

typedef struct myRotatePair {
    QVector4D rot1;
    QVector4D rot2;
} RotatePair;

class Camera : public Transformable
{
public:

    Camera(QString name);
    static QMatrix4x4            getViewMatrix(CameraP camera, int width, int height);
    static QMatrix4x4            getProjMatrix(CameraP camera, int width, int height);
    /*
    Point3                       eye();
    Vector3                      upDir();
    Point3                       lookat();
    Vector3                      lookDir();
    Vector3                      leftDir();
    */
    void                         lookTransform(RtMatrix &t);
    void                         flipYZ(RtMatrix m);
    RotatePair                   aim(Vector3 dir);

    QString                      name;
    int                          pickX;
    int                          pickY;
    int                          moveType;
    void                         mousePressed(QMouseEvent* event);
    void                         mouseReleased(QMouseEvent* event);
    void                         mouseDragged(QMouseEvent* event);


    Point3                       tmpEye;
};
typedef QSharedPointer<Camera> CameraP;

#endif // CAMERA_H
