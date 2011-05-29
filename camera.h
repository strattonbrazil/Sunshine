#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include "util.h"

class Camera : Transformable
{
public:
    Camera();
    static QMatrix4x4            getViewMatrix(Camera* camera, int width, int height);
    static QMatrix4x4            getProjMatrix(Camera* camera, int width, int height);
    Point3                       eye();
    Vector3                      upDir();
    Point3                       lookat();
    Vector3                      lookDir();
    Vector3                      leftDir();
    Point3                       center;
};

#endif // CAMERA_H
