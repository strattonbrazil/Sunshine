#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <QGLShaderProgram>

using namespace std;

class ShaderFactory
{
public:
    static QGLShaderProgram* buildFlatShader(QObject *parent);
    static QGLShaderProgram* buildShader(QObject *parent);
};

#endif // SHADER_H
