#ifndef UTIL_H
#define UTIL_H

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
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

class Transformable
{
public:
                  Transformable();
    void          setYRot(float a) { _yRot = a; updateLook(); }
    float         yRot() { return _yRot; }
    void          setUpRot(float a) { _upRot = a; updateLook(); }
    float         upRot() { return _upRot; }
    float         fov() { return _fov; }
    Point3        eye() { return _center; }
    Vector3       upDir() { return _rotate.rotatedVector(Vector3(0,1,0)); }
    Point3        lookat() { return _center + lookDir(); }
    Vector3       lookDir() { return _rotate.rotatedVector(Vector3(0,0,_distance)); }
    Vector3       leftDir() { return Vector3::crossProduct(upDir(), lookDir()); }
    void          resetLook();
    void          updateLook();
    void          orient(Point3 eye, Point3 reference, Vector3 up);

    Point3        center() { return _center; }
    void          setCenter(Point3 c) { _center = c; }
    Quat4         rotate() { return _rotate; }
    void          setRotate(Quat4 r) { _rotate = r; resetLook(); }
    Vector3       scale() { return _scale; }
    void          setScale(Vector3 s) { _scale = s; }

    Point3        centerReference() { return _centerReference; }
    void          setCenterReference(Point3 c) { _centerReference = c; }

    QMatrix4x4    objectToWorld();



protected:
    Point3        _center;
    Vector3       _scale;
    Quat4         _rotate;

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
    enum { LAYOUT, MODEL, TWEAK };
}
namespace SelectMode {
    enum { LINE, BOX };
}

#endif // UTIL_H
