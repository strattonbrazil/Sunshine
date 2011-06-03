#include "camera.h"

//#include <iostream>
//using namespace std;

namespace MoveType {
    enum { ROTATING, PANNING, TRUCKING, NOT_MOVING };
}

Camera::Camera(QString name) : Transformable(), name(name)
{
    resetLook();
    _center = Point3(10,6,10);
    _yRot = 225;
    _upRot = 45;
    _distance = _center.length();
    updateLook();
    moveType = MoveType::NOT_MOVING;
}

Point3 Camera::eye() { return _center; }
Vector3 Camera::upDir() { return _rotate.rotatedVector(Vector3(0,1,0)); }
Point3 Camera::lookat() { return _center + lookDir(); }
Vector3 Camera::lookDir() { return _rotate.rotatedVector(Vector3(0,0,_distance)); }
Vector3 Camera::leftDir() { return Vector3::crossProduct(upDir(), lookDir()); }

QMatrix4x4 Camera::getViewMatrix(Camera* camera, int width, int height)
{
    QMatrix4x4 m;
    m.lookAt(camera->eye(), camera->lookat(), camera->upDir());
    return m;
}

QMatrix4x4 Camera::getProjMatrix(Camera* camera, int width, int height)
{
    // taken from gluPerspective docs
    float aspect = (float)width / (float)height;
    float zNear = 0.1f;
    float zFar = 1000.0f;

    QMatrix4x4 m;
    m.perspective(camera->fov(), aspect, zNear, zFar);
    return m;
}

void Camera::mousePressed(QMouseEvent *event)
{
    pickX = event->pos().x();
    pickY = event->pos().y();

    if (event->button() == Qt::LeftButton) {
        moveType = MoveType::ROTATING;
    }
    else if (event->button() == Qt::MidButton) {
        moveType = MoveType::PANNING;
    } else if (event->button() == Qt::RightButton) {
        moveType = MoveType::TRUCKING;
    } else {
        moveType = MoveType::NOT_MOVING;
    }
}

void Camera::mouseReleased(QMouseEvent *event)
{
    moveType = MoveType::NOT_MOVING;
}

void Camera::mouseDragged(QMouseEvent *event)
{
    int xDiff = pickX - event->pos().x();
    int yDiff = pickY - event->pos().y();

    if (moveType == MoveType::ROTATING) {
        Vector3 origLook = eye() + lookDir();
        setYRot(yRot() + xDiff * 0.5f);
        setUpRot(upRot() + yDiff * -0.5f);

        // move eye to look at original focal point (Maya style)
        Vector3 lookAway = lookDir() * -1;
        setCenter(origLook + lookAway);
    }
    else if (moveType == MoveType::PANNING) {
        float panScale = 0.05f;

        Vector3 f = (lookat() - eye()).normalized();
        Vector3 s = Vector3::crossProduct(f, upDir());
        Vector3 mUp = Vector3::crossProduct(s, f);

        //val mUp = new Vector3(upDir)
        mUp = mUp * -1.0f * yDiff * panScale;

        Vector3 left = leftDir() * -1.0f * xDiff * panScale;

        setCenter(center() + mUp + left);
    } else if (moveType == MoveType::TRUCKING) {
        Point3 at = lookat();
        Vector3 l = lookDir() * -0.01f * yDiff;

        setCenter(l + center());
    }

    pickX = event->pos().x();
    pickY = event->pos().y();
}
