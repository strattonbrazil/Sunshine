#include "shader.h"
#include "util.h"

//#include <iostream>
//using namespace std;

QGLShaderProgramP ShaderFactory::buildFlatShader(QObject *parent)
{
    QString vertSource("in vec3 vertex;\n" \
                       "in vec4 color;\n" \
                       "uniform mat4 objToWorld;\n" \
                       "uniform mat4 cameraPV;\n" \
                       "void main() {\n" \
                       "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                       "  gl_FrontColor = color;\n" \
                       "}\n");

    QString fragSource("uniform vec4 overrideColor;\n" \
                       "uniform float overrideStrength;\n" \
                       "void main() {\n" \
                       "  gl_FragColor = (1.0-overrideStrength) * gl_Color + overrideStrength * overrideColor;\n" \
                       "}\n");


    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource);

    QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    fragShader->compileSourceCode(fragSource);

    QGLShaderProgramP program = QGLShaderProgramP(new QGLShaderProgram(parent));
    program->addShader(vertShader);
    program->addShader(fragShader);

    program->link();

    //cout << program->log() << endl;
    //cout << QString("Log end--") << endl;

    return program;
}

QGLShaderProgramP ShaderFactory::buildMeshShader(QObject *parent)
{
    QString vertSource("#version 120\n" \
                       "#extension GL_EXT_gpu_shader4 : enable\n" \
                       "in vec3 vertex;\n" \
                       "in vec4 color;\n" \
                       "in vec4 selectIndex;\n" \
                       "in vec3 normal;\n" \
                       "in float excludeEdge;\n" \
                       "varying vec3 vertWorldPos;\n" \
                       "varying vec3 vertWorldNormal;\n" \
                       "varying float vertExcludeEdge;\n" \
                       "uniform mat4 objToWorld;\n" \
                       "uniform mat4 cameraPV;\n" \
                       "uniform mat4 normalToWorld;\n" \
                       "void main() {\n" \
                       "  vertWorldPos = (objToWorld * vec4(vertex,1.0)).xyz;\n" \
                       "  vertWorldNormal = (normalToWorld * vec4(normal,1.0)).xyz;\n" \
                       "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                       "  vertExcludeEdge = excludeEdge;\n" \
                       "  gl_FrontColor = color;\n" \
                       "}\n");

    QString geomSource("#version 120\n" \
                       "#extension GL_EXT_gpu_shader4 : enable\n" \
                       "#extension GL_EXT_geometry_shader4 : enable\n" \
                       "varying in vec3 vertWorldPos[3];\n" \
                       "varying in vec4 vertSelectIndex[3];\n" \
                       "varying in vec3 vertWorldNormal[3];\n" \
                       "varying in float vertExcludeEdge[3];\n" \
                       "varying out vec3 worldNormal;\n" \
                       "varying out vec3 worldPos;\n" \
                       "varying out vec4 selectIndex;\n" \
                       "uniform vec2 WIN_SCALE;\n" \
                       "noperspective varying vec3 dist;\n" \
                       "void main(void)\n" \
                   "{\n" \
                   "  float MEW = 100.0; // max edge width\n" \
                   "  // adapted from 'Single-Pass Wireframe Rendering'\n"
                   "  vec2 p0 = WIN_SCALE * gl_PositionIn[0].xy/gl_PositionIn[0].w;\n" \
                   "  vec2 p1 = WIN_SCALE * gl_PositionIn[1].xy/gl_PositionIn[1].w;\n" \
                   "  vec2 p2 = WIN_SCALE * gl_PositionIn[2].xy/gl_PositionIn[2].w;\n" \
                   "  vec2 v0 = p2-p1;\n" \
                   "  vec2 v1 = p2-p0;\n" \
                   "  vec2 v2 = p1-p0;\n" \
                   "  float area = abs(v1.x*v2.y - v1.y * v2.x);\n" \
                   "  dist = vec3(area/length(v0),vertExcludeEdge[1]*MEW,vertExcludeEdge[2]*MEW);\n" \
                   "  worldPos = vertWorldPos[0];\n" \
                   "  worldNormal = vertWorldNormal[0];\n" \
                   "  gl_Position = gl_PositionIn[0];\n" \
                   "  gl_FrontColor = gl_FrontColorIn[0];\n" \
                   "  selectIndex = vertSelectIndex[0];\n" \
                   "  EmitVertex();\n" \
                   "  dist = vec3(vertExcludeEdge[0]*MEW,area/length(v1),vertExcludeEdge[2]*MEW);\n" \
                   "  worldPos = vertWorldPos[1];\n" \
                   "  worldNormal = vertWorldNormal[1];\n" \
                   "  gl_Position = gl_PositionIn[1];\n" \
                   "  gl_FrontColor = gl_FrontColorIn[1];\n" \
                   "  selectIndex = vertSelectIndex[1];\n" \
                   "  EmitVertex();\n" \
                   "  dist = vec3(vertExcludeEdge[0]*MEW,vertExcludeEdge[1]*MEW,area/length(v2));\n" \
                   "  worldPos = vertWorldPos[2];\n" \
                   "  worldNormal = vertWorldNormal[2];\n" \
                   "  gl_Position = gl_PositionIn[2];\n" \
                   "  gl_FrontColor = gl_FrontColorIn[2];\n" \
                   "  selectIndex = vertSelectIndex[2];\n" \
                   "  EmitVertex();\n" \
                   "  EndPrimitive();\n" \
                   "}\n");

    QString fragSource("#version 120\n" \
                       "#extension GL_EXT_gpu_shader4 : enable\n" \
                       "varying vec3 worldPos;\n" \
                       "varying vec3 worldNormal;\n" \
                       "varying vec4 selectIndex;\n" \
                       "noperspective varying vec3 dist;\n" \
                       "uniform vec3 cameraPos;\n" \
                       "uniform vec3 lightDir;\n" \
                       "uniform vec4 singleColor;\n" \
                       "uniform float isSingleColor;\n" \
                       "void main() {\n" \
                       "    // determine frag distance to closest edge\n" \
                       "    float nearD = min(min(dist[0],dist[1]),dist[2]);\n" \
                       "    float edgeIntensity = exp2(-1*nearD*nearD);\n" \
                       "    vec3 L = lightDir;\n" \
                       "    vec3 V = normalize(cameraPos - worldPos);\n" \
                       "    vec3 N = normalize(worldNormal);\n" \
                       "    vec3 H = normalize(L+V);\n" \
                       "    vec4 color = isSingleColor*singleColor + (1.0-isSingleColor)*gl_Color;\n" \
                       "    float amb = 0.6;\n" \
                       "    vec4 ambient = color * amb;\n" \
                       "    vec4 diffuse = color * (1.0 - amb) * max(dot(L, N), 0.0);\n" \
                       "    vec4 specular = vec4(0.0);\n" \
                       "    gl_FragData[0] = (edgeIntensity * vec4(0.1,0.1,0.1,1.0)) + ((1.0-edgeIntensity) * vec4(ambient + diffuse + specular));\n" \
                       "    gl_FragData[1] = selectIndex;\n" \
                       "    //gl_FragColor = vec4(nearD*0.1);\n" \
                       "}\n");

    //std::cout << vertSource.toStdString() << std::endl;
    //std::cout << geomSource.toStdString() << std::endl;
    //std::cout << fragSource.toStdString() << std::endl;

    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource);

    QGLShader* geomShader = new QGLShader(QGLShader::Geometry);
    geomShader->compileSourceCode(geomSource);

    QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    fragShader->compileSourceCode(fragSource);

    QGLShaderProgramP program = QGLShaderProgramP(new QGLShaderProgram(parent));
    program->addShader(vertShader);
    program->addShader(geomShader);
    program->addShader(fragShader);

    return program;
}

QGLShaderProgramP ShaderFactory::buildVertexShader(QObject *parent)
{
    QString vertSource("#version 130\n" \
                       "in vec3 vertex;\n" \
                       "in float colorIndex;\n" \
                       "uniform vec4 colors[4]; // unselected, highlighted, selected, highlighted\n" \
                       "uniform mat4 objToWorld;\n" \
                       "uniform mat4 cameraPV;\n" \
                       "void main() {\n" \
                       "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                       "  gl_FrontColor = colors[int(colorIndex)];\n" \
                       "  //gl_FrontColor = vec4(colorIndex);\n" \
                       "}\n");

    QString fragSource("uniform vec4 overrideColor;\n" \
                       "uniform float overrideStrength;\n" \
                       "void main() {\n" \
                       "  gl_FragColor = (1.0-overrideStrength) * gl_Color + overrideStrength * overrideColor;\n" \
                       "}\n");


    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource);

    //QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    //fragShader->compileSourceCode(fragSource);

    QGLShaderProgramP program = QGLShaderProgramP(new QGLShaderProgram(parent));
    program->addShader(vertShader);
    //program->addShader(fragShader);

    program->link();

    return program;
}

QGLShaderProgramP ShaderFactory::buildPropertyShader(QObject *parent)
{
    QString vertSource("in vec3 vertex;\n" \
                       "in vec4 color;\n" \
                       "uniform mat4 objToWorld;\n" \
                       "uniform mat4 cameraPV;\n" \
                       "void main() {\n" \
                       "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                       "  gl_FrontColor = color;\n" \
                       "}\n");

    QString fragSource("uniform vec4 overrideColor;\n" \
                       "uniform float overrideStrength;\n" \
                       "void main() {\n" \
                       "  gl_FragColor = (1.0-overrideStrength) * gl_Color + overrideStrength * overrideColor;\n" \
                       "}\n");


    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource);

    QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    fragShader->compileSourceCode(fragSource);

    QGLShaderProgramP program = QGLShaderProgramP(new QGLShaderProgram(parent));
    program->addShader(vertShader);
    program->addShader(fragShader);

    program->link();

    //cout << program->log() << endl;
    //cout << QString("Log end--") << endl;

    return program;
}

QGLShaderProgramP ShaderFactory::buildMaterialShader(MaterialP material, QObject *parent)
{
    // write vertex shader
    //
    QString vertSource;
    vertSource += "uniform mat4 objToWorld;\n" \
                  "uniform mat4 cameraPV;\n" \
                  "in vec3 vertex;\n";
    //foreach(MaterialAttribute att, material->attributes()) {
    //    vertSource += "// check";
    //}
    vertSource += "void main() {\n" \
                  "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                  "}\n";

    // write fragment shader
    //
    QString fragSource;
    fragSource += "uniform vec4 overrideColor;\n" \
                  "uniform float overrideStrength;\n" \
                  "void main() {\n" \
                  "  gl_FragColor = (1.0-overrideStrength) * gl_Color + overrideStrength * overrideColor;\n" \
                  "}\n";


    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource);

    QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    fragShader->compileSourceCode(fragSource);

    QGLShaderProgramP program = QGLShaderProgramP(new QGLShaderProgram(parent));
    program->addShader(vertShader);
    program->addShader(fragShader);

    program->link();

    std::cout << "-- vert source -----" << std::endl;
    std::cout << vertSource.toStdString() << std::endl;

    std::cout << "\n-- frag source -----" << std::endl;
    std::cout << fragSource.toStdString() << std::endl;

    //cout << program->log() << endl;
    //cout << QString("Log end--") << endl;

    return program;
}
