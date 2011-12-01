#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include "transformable.h"
#include <QMouseEvent>
#include <aqsis/ri/ri.h>

typedef struct myRotatePair {
    QVector4D rot1;
    QVector4D rot2;
} RotatePair;

class Camera : public Transformable
{
    Q_OBJECT
public:
    int                          assetType() { return AssetType::CAMERA_ASSET; }
    Camera();
    static QMatrix4x4            getViewMatrix(Camera* camera, int width, int height);
    //static QMatrix4x4            getProjMatrix(Camera* camera, int width, int height, float dx=0, float dy=0);
    static QMatrix4x4            getProjMatrix(Camera* camera, int width, int height, float pixdx=0, float pixdy=0);

    void                         lookTransform(RtMatrix &t);
    void                         flipYZ(RtMatrix m);
    RotatePair                   aim(Vector3 dir);

    int                          pickX;
    int                          pickY;
    int                          moveType;
    void                         mousePressed(QMouseEvent* event);
    void                         mouseReleased(QMouseEvent* event);
    void                         mouseDragged(QMouseEvent* event);


    Point3                       tmpEye;
};

#endif // CAMERA_H
