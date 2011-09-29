#ifndef PROJECT_UTIL_H
#define PROJECT_UTIL_H

#include "geometry.h"

namespace ProjectUtil {
    Point3 unproject(float winX, float winY, float winZ, QMatrix4x4 modelView, QMatrix4x4 proj, int* viewport);
    Point3 project(float objX, float objY, float objZ, QMatrix4x4 modelView, QMatrix4x4 proj, int* viewport);


};

#endif // PROJECT_UTIL_H
