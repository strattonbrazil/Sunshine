#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <QGLShaderProgram>
#include "material.h"

//using namespace std;

typedef QSharedPointer<QGLShaderProgram> QGLShaderProgramP;

class ShaderFactory
{
public:
    static QGLShaderProgramP buildFlatShader(QObject *parent);
    static QGLShaderProgramP buildMeshShader(QObject *parent);
    static QGLShaderProgramP buildVertexShader(QObject *parent);
    static QGLShaderProgramP buildPropertyShader(QObject *parent);
    static QGLShaderProgramP buildMaterialShader(MaterialP material, QObject *parent);
};

#endif // SHADER_H
