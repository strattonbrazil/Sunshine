#include "camera.h"

//#include <iostream>
//using namespace std;



// used by panning


Camera::Camera()// : Transformable()
{
    resetLook();
    _center = Point3(10.0,6.0,10.0);
    _yRot = 225;
    _upRot = 45;
    _distance = _center.length();
    updateLook();
    _center = -lookDir();
    //updateLook();
    //moveType = MoveType::NOT_MOVING;
}

/*
Point3 Camera::eye() { return _center; }
Vector3 Camera::upDir() {
    return _rotate.rotatedVector(Vector3(0,1,0));
}
Point3 Camera::lookat() { return _center + lookDir(); }
Vector3 Camera::lookDir() { return _rotate.rotatedVector(Vector3(0,0,_distance)); }
Vector3 Camera::leftDir() { return Vector3::crossProduct(upDir(), lookDir()).normalized(); }
*/

QMatrix4x4 Camera::getViewMatrix(Transformable* camera, int width, int height)
{
    QMatrix4x4 m;
    m.lookAt(camera->eye(), camera->lookat(), camera->upDir());
    return m;
}

/*
QMatrix4x4 Camera::getProjMatrix(Camera* camera, int width, int height, float dx, float dy)
{
    // taken from gluPerspective docs
    float aspect = (float)width / (float)height;
    float zNear = 0.1f;
    float zFar = 1000.0f;

    float top = tan(camera->fov()*3.14159/360.0) * zNear;
    //float top = tan(fov*0.5) * zNear;
    float bottom = -top;

    float left = aspect * bottom;
    float right = aspect * top;

    QMatrix4x4 m;
    m.frustum(left+dx, right+dx, bottom+dy, top+dy, zNear, zFar);
    //m.perspective(camera->fov(), aspect, zNear, zFar);
    return m;
}
*/

QMatrix4x4 Camera::getProjMatrix(Transformable* camera, int width, int height, float pixdx, float pixdy)
{
    // taken from gluPerspective docs
    float aspect = (float)width / (float)height;
    float zNear = .1f;
    float zFar = 100.0f;

    float top = tan(camera->fov()*3.14159/360.0) * zNear;
    //float top = tan(fov*0.5) * zNear;
    float bottom = -top;

    float left = aspect * bottom;
    float right = aspect * top;

    //int viewport[4];
    //glGetIntegerv(GL_VIEWPORT, viewport);
    float xwsize = right - left;
    float ywsize = top - bottom;

    // MAINT: width/height should be pulled from viewport if it doesn't match
    // size of render
    float dx = -(pixdx * xwsize / (float)width);
    float dy = -(pixdy * ywsize / (float)height);

    QMatrix4x4 m;
    m.frustum(left+dx, right+dx, bottom+dy, top+dy, zNear, zFar);
    //m.perspective(camera->fov(), aspect, zNear, zFar);
    return m;
}


/*
QMatrix4x4 Camera::getProjMatrix(Camera* camera, int width, int height, float offsetX, float offsetY)
{
    // taken from gluPerspective docs
    float aspect = (float)width / (float)height;
    float zNear = 0.1f;
    float zFar = 1000.0f;


    double fov2, left, right, bottom, top;

        fov2 = ((fovy * Math.PI) / 180.0) / 2.0;

        top = near / (Math.cos(fov2) / Math.sin(fov2));
        bottom = -top;

        right = top * aspect;
        left = -right;

        accFrustum(gl, left, right, bottom, top, near, far, pixdx, pixdy, eyedx,
            eyedy, focus);

    QMatrix4x4 m;
    m.perspective(camera->fov(), aspect, zNear, zFar);
    return m;
}
*/

#define PI 3.14159265359
RotatePair Camera::aim(Vector3 dir)
{ // based on VTK aim
    double xzlen, yzlen, yrot, xrot;

    /*
     * The initial rotation about the y axis is given by the projection of
     * the direction vector onto the x,z plane: the x and z components
     * of the direction.
     */
    xzlen = sqrt(dir.x()*dir.x()+dir.z()*dir.z());
    if (xzlen == 0)
        yrot = (dir.y() < 0) ? 180 : 0;
    else
        yrot = 180*acos(dir.z()/xzlen)/PI;
    /*
     * The second rotation, about the x axis, is given by the projection on
     * the y,z plane of the y-rotated direction vector: the original y
     * component, and the rotated x,z vector from above.
    */
    yzlen = sqrt(dir.y()*dir.y()+xzlen*xzlen);
    xrot = 180*acos(xzlen/yzlen)/PI;       /* yzlen should never be 0 */

    RotatePair pair;

    if (dir.y() > 0) {
        pair.rot1 = QVector4D(xrot, 1.0, 0.0, 0.0);
        //fprintf (filePtr, "Rotate %f %f %f %f\n", xrot, 1.0, 0.0, 0.0);
    } else {
        pair.rot1 = QVector4D(-xrot, 1.0, 0.0, 0.0);
        //fprintf (filePtr, "Rotate %f %f %f %f\n", -xrot, 1.0, 0.0, 0.0);
    }
    /* The last rotation declared gets performed first */
    if (dir.x() > 0) {
        pair.rot2 = QVector4D(-yrot, 0.0, 1.0, 0.0);
        //fprintf (filePtr, "Rotate %f %f %f %f\n", -yrot, 0.0, 1.0, 0.0);
    } else {
        pair.rot2 = QVector4D(yrot, 0.0, 1.0, 0.0);
        //fprintf (filePtr, "Rotate %f %f %f %f\n", yrot, 0.0, 1.0, 0.0);
    }

    return pair;
}

/*
void Camera::flipYZ(RtMatrix m)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) {
                if (i == 0)
                    m[i][j] = 1;
                else
                    m[i][j] = -1;
            } else
                m[i][j] = 0;
        }
    }
}
*/

void Camera::mousePressed(Transformable* camera, CameraScratch &scratch, QMouseEvent *event)
{
    scratch.pickX = event->pos().x();
    scratch.pickY = event->pos().y();

    if (event->button() == Qt::LeftButton) {
        scratch.moveType = MoveType::ROTATING;
    }
    else if (event->button() == Qt::MidButton) {
        scratch.moveType = MoveType::PANNING;

        scratch.origEye = camera->eye();
        //Vector3 f = (lookat() - eye()).normalized();
        //Vector3 s = Vector3::crossProduct(f, upDir());
        scratch.origUp = camera->upDir();
        //origUp = Vector3::crossProduct(s, f);
        scratch.origLeft = camera->leftDir();
    } else if (event->button() == Qt::RightButton) {
        scratch.moveType = MoveType::TRUCKING;
    } else {
        scratch.moveType = MoveType::NOT_MOVING;
    }
}

void Camera::mouseReleased(Transformable* camera, CameraScratch &scratch, QMouseEvent *event)
{
    scratch.moveType = MoveType::NOT_MOVING;
}

void Camera::mouseDragged(Transformable* camera, CameraScratch &scratch, QMouseEvent *event)
{
    int xDiff = scratch.pickX - event->pos().x();
    int yDiff = scratch.pickY - event->pos().y();

    if (scratch.moveType == MoveType::ROTATING) {
        Vector3 origLook = camera->eye() + camera->lookDir();
        camera->setYRot(camera->yRot() + xDiff * 0.5f);
        camera->setUpRot(camera->upRot() + yDiff * -0.5f);

        // move eye to look at original focal point (Maya style)
        Vector3 lookAway = camera->lookDir() * -1;
        camera->setCenter(origLook + lookAway);
    }
    else if (scratch.moveType == MoveType::PANNING) {
        float panScale = 0.05f;

        Vector3 mUp = scratch.origUp * -1.0f * yDiff * panScale;
        Vector3 mLeft = scratch.origLeft * -1.0f * xDiff * panScale;

        camera->setCenter(camera->eye() + mUp + mLeft);
    } else if (scratch.moveType == MoveType::TRUCKING) {
        Point3 at = camera->lookat();
        Vector3 l = camera->lookDir() * -0.01f * yDiff;

        camera->setCenter(l + camera->center());
    }

    scratch.pickX = event->pos().x();
    scratch.pickY = event->pos().y();
}
