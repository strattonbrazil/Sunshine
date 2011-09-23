#ifndef UTIL_H
#define UTIL_H

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QPoint>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QTextStream>
#include <QFile>
#include <QSharedPointer>
#include <math.h>
#include <iostream>

typedef QVector2D Vector2;
typedef QVector3D Vector3;
typedef QVector4D Vector4;
typedef QVector3D Point3; // bad idea?

std::ostream& operator<< (std::ostream& o, Vector3 const& v);
std::ostream& operator<< (std::ostream& o, QVector4D const& v);
std::ostream& operator<< (std::ostream& o, QPoint const& p);

#define PI 3.14159

class Quat4 : public QQuaternion
{
public:
                        Quat4();
                        Quat4(float w, float x, float y, float z);
                        Quat4(float s, Vector3 v);
                        Quat4(QQuaternion q);

    static Quat4        fromMatrix(float m00, float m01, float m02,
                                   float m10, float m11, float m12,
                                   float m20, float m21, float m22);
    QMatrix4x4          matrix();
};

namespace RotateOrder {
    enum { XYZ, YZX, ZXY, XZY, YXZ, ZYX };
}

/*
float fourQuadrantInverseTangent(Vector3 a, Vector3 b)
{
    Vector3 c = Vector3::crossProduct(a, b);
    return std::atan2(c.length(), a*b);
}
*/

class Transformable
{
public:
                       Transformable();
    void               setYRot(float a) { _yRot = a; updateLook(); }
    float              yRot() { return _yRot; }
    void               setUpRot(float a) { _upRot = a; updateLook(); }
    float              upRot() { return _upRot; }
    float              fov() { return _fov; }
    Point3             eye() { return _center; }
    Vector3            upDir() { return rotate().rotatedVector(Vector3(0,1,0)); }
    Point3             lookat() { return _center + lookDir(); }
    Vector3            lookDir() { return rotate().rotatedVector(Vector3(0,0,_distance)); }
    Vector3            leftDir() { return Vector3::crossProduct(upDir(), lookDir()).normalized(); }
    void               resetLook();
    void               updateLook();
    void               orient(Point3 eye, Point3 reference, Vector3 up);

    Point3             center() { return _center; }
    void               setCenter(Point3 c) { _center = c; }
    Quat4              rotate() { return _rotate;
                                  /*
        // calculate rotate based on vector and rotate order
        Quat4 rotateX = Quat4::fromAxisAndAngle(1, 0, 0, _rotate.x());
        Quat4 rotateY = Quat4::fromAxisAndAngle(0, 1, 0, _rotate.y());
        Quat4 rotateZ = Quat4::fromAxisAndAngle(0, 0, 1, _rotate.z());
        if (_rotateOrder == RotateOrder::XYZ) return rotateZ * rotateY * rotateX;
        else if (_rotateOrder == RotateOrder::YZX) return rotateX * rotateZ * rotateY;
        else if (_rotateOrder == RotateOrder::ZXY) return rotateY * rotateX * rotateZ;
        else if (_rotateOrder == RotateOrder::XZY) return rotateY * rotateZ * rotateX;
        else if (_rotateOrder == RotateOrder::YXZ) return rotateZ * rotateX * rotateY;
        else if (_rotateOrder == RotateOrder::ZYX) return rotateX * rotateY * rotateZ;
                                  */
    }
    //Vector3            rotateVector() { return _rotate; }
    int                rotateOrder() { return _rotateOrder; }
    void               setRotate(Quat4 r) {
        /*
        // figure out the rotation angles for this quaternion
        // taken from "Quaternion to Euler Angle Conversion for Arbitrary Rotation Sequence Using Geometric Methods"

        // build the rotation vectors based on rotation order
        Vector3 v1;
        Vector3 v2;
        Vector3 v3;
        if (_rotateOrder == RotateOrder::YZX) { v1 = Vector3(0,1,0); v2 = Vector3(0,0,1); v3 = Vector3(1,0,0); }

        v3_rot = r.rotatedVector(v3);
        float theta1 =*/
        _rotate = r; resetLook();
    }
    Vector3            scale() { return _scale; }
    void               setScale(Vector3 s) { _scale = s; }

    Point3             centerReference() { return _centerReference; }
    void               setCenterReference(Point3 c) { _centerReference = c; }

    QMatrix4x4         objectToWorld();



protected:
    Point3        _center;
    Vector3       _scale;
    Quat4         _rotate;
    //Vector3       _rotate;
    int           _rotateOrder;

    Point3        _centerReference;

    // specific to camera controls (look through selected)
    Quat4         _startRotate;
    float         _yRot;
    float         _upRot;
    float         _fov;
    float         _distance;
};

void printMatrix(QMatrix4x4 m);
void printVector3(Vector3 v);
void printQuat(QQuaternion q);

const QVector4D SELECTED_COLOR(1,0,0,1);
const QVector4D SELECTED_HOVER_COLOR(1,1,0,1);
const QVector4D UNSELECTED_COLOR(1,1,1,1);
const QVector4D UNSELECTED_HOVER_COLOR(0,1,0,1);

namespace Axis { enum { NoAxis, GlobalX, GlobalY, GlobalZ, LocalX, LocalY, LocalZ, Normal, Screen }; };

namespace WorkMode {
    enum { LAYOUT, MODEL };
}
namespace SelectMode {
    enum { LINE, BOX };
}
namespace ModelMode {
    enum { NONE, VERTEX, EDGE, FACE };
}
#endif // UTIL_H
