#include "camera.h"

Camera::Camera() : Transformable()
{
    resetLook();
    _center = Point3(10,6,10);
    _yRot = PI * (-3.0f/4.0f);
    _upRot = 0.44f;
    _distance = _center.length();
    updateLook();
    printQuat(_rotate);
}

Point3 Camera::eye() { return _center; }
Vector3 Camera::upDir() { return _rotate.rotatedVector(Vector3(0,1,0)); }
Point3 Camera::lookat() { return _center + lookDir(); }
Vector3 Camera::lookDir() { return _rotate.rotatedVector(Vector3(0,0,_distance)); }
Vector3 Camera::leftDir() { return Vector3::crossProduct(upDir(), lookDir()); }

QMatrix4x4 Camera::getViewMatrix(Camera* camera, int width, int height)
{
    // take from gluLookAt - http://www.manpagez.com/man/3/gluLookAt/
    Vector3 f = (camera->lookat() - camera->eye()).normalized();
    Vector3 s = Vector3::crossProduct(f, camera->upDir());
    Vector3 u = Vector3::crossProduct(s, f);

    /*
    printVector3(camera->lookDir());
    printVector3(camera->lookat());
    printVector3(camera->eye());
    printVector3(f);
    printVector3(s);
    printVector3(u);
    */

    QMatrix4x4 M(s.x(), s.y(), s.z(),  0.0f,
                 u.x(), u.y(), u.z(),  0.0f,
                 -f.x(),-f.y(),-f.z(), 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f);
    QMatrix4x4 trans;
    trans.translate(-camera->eye().x(), -camera->eye().y(), -camera->eye().z());

    //return M * trans;
    QMatrix4x4 m;
    m.lookAt(camera->eye(), Point3(0,0,0), Vector3(0,1,0));
    return m;
}

QMatrix4x4 Camera::getProjMatrix(Camera* camera, int width, int height)
{
    // taken from gluPerspective docs
    float aspect = (float)width / (float)height;
    float zNear = 0.1f;
    float zFar = 1000.0f;
    /*
    float f = 1.0f / tan(0.5f*camera->fov()*PI / 180.0f);

    // declared here so the matrix below is prettier
    float foo = (zFar+zNear)/(zNear-zFar);
    float bar = (2.0f*zFar*zNear)/(zNear-zFar);
    */

    QMatrix4x4 m;
    m.perspective(camera->fov(), aspect, zNear, zFar);
    return m;
    /*
    return QMatrix4x4(f/aspect, 0.0f, 0.0f, 0.0f,
                      0.0f, f, 0.0f, 0.0f,
                      0.0f, 0.0f, foo, bar,
                      0.0f, 0.0f, -1.0f, 0.0f);
                      */
}

