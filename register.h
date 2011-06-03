#ifndef REGISTER_H
#define REGISTER_H

#include "util.h"
#include "camera.h"
#include "geometry.h"

class Register
{
public:
    static MeshP          mesh(int key);
    static void           clearScene();
    static CameraP        createCamera(QString name);
protected:
                          Register();
    static void           validate();
    int                   uniqueCameraKey();
    QString               uniqueName(QString prefix);
private:
    static Register*      instance;
    QHash<int,MeshP>       _meshes;
    QHash<int,CameraP>     _cameras;
    //QHash<int,Light*>      _lights;
    QSet<QString>         _names;
};

#endif // REGISTER_H
