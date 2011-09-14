#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <QGLShaderProgram>

//using namespace std;

typedef QSharedPointer<QGLShaderProgram> QGLShaderProgramP;

class ShaderFactory
{
public:
    static QGLShaderProgramP buildFlatShader(QObject *parent);
    static QGLShaderProgramP buildMeshShader(QObject *parent);
    static QGLShaderProgramP buildVertexShader(QObject *parent);
};

#endif // SHADER_H
