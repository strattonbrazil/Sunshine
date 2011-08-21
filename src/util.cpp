#include "util.h"

//#include <iostream>
//using namespace std;

std::ostream& operator<< (std::ostream& o, Vector3 const& v)
{
   return o << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
}

std::ostream& operator<< (std::ostream& o, QVector4D const& v)
{
   return o << "(" << v.x() << ", " << v.y() << ", " << v.z() << ", " << v.w() << ")";
}

Quat4::Quat4() : QQuaternion()
{

}

Quat4::Quat4(float s, Vector3 v) : QQuaternion(s, v)
{

}

Quat4::Quat4(float w, float x, float y, float z) : QQuaternion(w,x,y,z)
{

}

Quat4::Quat4(QQuaternion q) : QQuaternion(q)
{

}

Quat4 Quat4::fromMatrix(float m00, float m01, float m02,
                        float m10, float m11, float m12,
                        float m20, float m21, float m22) {
    // taken from http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
    float tr = m00 + m11 + m22;

    float w,x,y,z;

    if (tr > 0) {
        float S = (float)sqrt(tr + 1.0) * 2; // S=4*qw
        w = 0.25f * S;
        x = (m21 - m12) / S;
        y = (m02 - m20) / S;
        z = (m10 - m01) / S;
    } else if ((m00 > m11) & (m00 > m22)) {
        float S = (float)sqrt(1.0 + m00 - m11 - m22) * 2; // S=4*qx
        w = (m21 - m12) / S;
        x = 0.25f * S;
        y = (m01 + m10) / S;
        z = (m02 + m20) / S;
    } else if (m11 > m22) {
        float S = (float)sqrt(1.0 + m11 - m00 - m22) * 2; // S=4*qy
        w = (m02 - m20) / S;
        x = (m01 + m10) / S;
        y = 0.25f * S;
        z = (m12 + m21) / S;
    } else {
        float S = (float)sqrt(1.0 + m22 - m00 - m11) * 2; // S=4*qz
        w = (m10 - m01) / S;
        x = (m02 + m20) / S;
        y = (m12 + m21) / S;
        z = 0.25f * S;
    }

    return Quat4(w,x,y,z);
}

QMatrix4x4 Quat4::matrix() {
    // I'm being lazy here.  Eventually transpose it by hand...
    float w = scalar();
    return QMatrix4x4(1.0f - 2.0f * ( y() * y() + z() * z() ),
                      2.0f * (x() * y() + z() * w),
                      2.0f * (x() * z() - y() * w),
                      0.0f,
                      2.0f * ( x() * y() - z() * w ),
                      1.0f - 2.0f * ( x() * x() + z() * z() ),
                      2.0f * (z() * y() + x() * w ),
                      0.0f,
                      2.0f * ( x() * z() + y() * w ),
                      2.0f * ( y() * z() - x() * w ),
                      1.0f - 2.0f * ( x() * x() + y() * y() ),
                      0.0f,
                      0,
                      0,
                      0,
                      1.0f).transposed();
}

Transformable::Transformable()
{
    _startRotate = QQuaternion();
    _yRot = 0.0f;
    _upRot = 0.0f;
    _fov = 60.0f;
    _distance = 1.0f;

    _scale = Vector3(1,1,1);

}

void Transformable::resetLook()
{
    //
    _startRotate = _rotate;
    _yRot = 0.0f;
    _upRot = 0.0f;
}

void Transformable::updateLook()
{
    // setups the rotation based on the two-camera controls
    //Quat4 ySpin = Quat4(_yRot, _startRotate.rotatedVector(Vector3(0,1,0)));
    Quat4 ySpin = Quat4::fromAxisAndAngle(_startRotate.rotatedVector(Vector3(0,1,0)), _yRot);
    Vector3 tmpVec = ySpin.rotatedVector(_startRotate.rotatedVector(Vector3(1,0,0)));
    //Quat4 upSpin = Quat4(_upRot, tmpVec);
    Quat4 upSpin = Quat4::fromAxisAndAngle(tmpVec, _upRot);
    const Quat4 mv = upSpin * ySpin;
    _rotate = mv * _startRotate;
    _rotate.normalize();
}

void Transformable::orient(Point3 eye, Point3 reference, Vector3 up)
{
    // similar to gluLookAt
    _center = eye;

    Vector3 lookDir = (reference - eye).normalized();
    Vector3 leftDir = Vector3::crossProduct(up, lookDir).normalized();
    Vector3 modifiedUp = Vector3::crossProduct(lookDir, leftDir).normalized();

    _rotate = Quat4::fromMatrix(leftDir.x(), modifiedUp.x(), lookDir.x(),
                                leftDir.y(), modifiedUp.y(), lookDir.y(),
                                leftDir.z(), modifiedUp.z(), lookDir.z());

}

QMatrix4x4 Transformable::objectToWorld()
{
    QMatrix4x4 t(1.0f, 0.0f, 0.0f, _center.x(),
                 0.0f, 1.0f, 0.0f, _center.y(),
                 0.0f, 0.0f, 1.0f, _center.z(),
                 0.0f, 0.0f, 0.0f, 1.0f);
    QMatrix4x4 r = _rotate.matrix();
    QMatrix4x4 s(_scale.x(), 0.0f, 0.0f, 0.0f,
                 0.0f, _scale.y(), 0.0f, 0.0f,
                 0.0f, 0.0f, _scale.z(), 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f);
    return t*r*s;
}


void printMatrix(QMatrix4x4 m)
{
    for (int i = 0; i < 4; i++) {
        Vector4 row = m.row(i);
        printf("%f %f %f %f\n", row.x(), row.y(), row.z(), row.w());
    }
    printf("\n");
    fflush(stdout);
}

void printVector3(Vector3 v)
{
    printf("<%f,%f,%f>\n", v.x(), v.y(), v.z());
    fflush(stdout);
}

void printQuat(QQuaternion q)
{
    printf("<%f,%f,%f,%f>\n", q.x(), q.y(), q.z(), q.scalar());
    fflush(stdout);
}

/*
void loadLuaFile(lua_State* state, QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QString lines = file.readAll();
    //std://:cout << lines.toStdString() << std::endl;
    try {
        std::cout << "executing " << fileName.toStdString() << "...\n-----------" << std::endl;
        std::cout << lines.toStdString().c_str() << std::endl;
        luaL_dostring(state, lines.toStdString().c_str());
    } catch (const std::exception &TheError) {
        std::cout << "*" << std::endl;
        std::cout << TheError.what() << std::endl;
    }

    file.close();
}
*/
