#include "primitive.h"

namespace primitive {
    PrimitiveParts cubePrimitive(float width, float height, float depth)
    {
        float hx = width / 2;
        float hy = height / 2;
        float hz = depth / 2;

        // create the vertices
        Point3 p0(hx,hy,hz);
        Point3 p1(hx,hy,-hz);
        Point3 p2(-hx,hy,-hz);
        Point3 p3(-hx,hy,hz);
        Point3 p4(hx,-hy,hz);
        Point3 p5(hx,-hy,-hz);
        Point3 p6(-hx,-hy,-hz);
        Point3 p7(-hx,-hy,hz);

        QList<int> f0 = QList<int>() << 0 << 1 << 2 << 3;
        QList<int> f1 = QList<int>() << 4 << 5 << 1 << 0;
        QList<int> f2 = QList<int>() << 6 << 2 << 1 << 5;
        QList<int> f3 = QList<int>() << 7 << 3 << 2 << 6;
        QList<int> f4 = QList<int>() << 7 << 4 << 0 << 3;
        QList<int> f5 = QList<int>() << 4 << 7 << 6 << 5;

        PrimitiveParts parts;
        parts.points = QVector<Point3>() << p0 << p1 << p2 << p3 << p4 << p5 << p6 << p7;
        parts.faces = QVector<QList<int> >() << f0 << f1 << f2 << f3 << f4 << f5;
        return parts;
    }

    PrimitiveParts planePrimitive(float width, float depth) {
        float hx = width / 2;
        float hz = depth / 2;

        // create the vertices
        Point3 p0(hx,0,hz);
        Point3 p1(hx,0,-hz);
        Point3 p2(-hx,0,-hz);
        Point3 p3(-hx,0,hz);

        QList<int> f0 = QList<int>() << 0 << 1 << 2 << 3;

        PrimitiveParts parts;
        parts.points = QVector<Point3>() << p0 << p1 << p2 << p3;
        parts.faces = QVector<QList<int> >() << f0;
        return parts;
    }
};
