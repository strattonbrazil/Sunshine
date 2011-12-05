#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <QGLShaderProgram>
#include "light.h"
#include "material.h"

//using namespace std;

class ShaderFactory
{
public:
    static QGLShaderProgram* buildShader(QObject* parent, QString vertFile, QString fragFile);
    static QGLShaderProgram* buildFlatShader(QObject* parent);
    static QGLShaderProgram* buildMeshShader(QObject* parent);
    static QGLShaderProgram* buildVertexShader(QObject *parent);
    static QGLShaderProgram* buildPropertyShader(QObject *parent);
    static QGLShaderProgram* buildMaterialShader(Light* light, Material* material, QObject* parent);
    static QGLShaderProgram* buildDistanceShader(QObject* parent);
};

#endif // SHADER_H
