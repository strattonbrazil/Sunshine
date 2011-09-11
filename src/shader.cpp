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
                       "in vec3 normal;\n" \
                       "in float hasEdge;\n" \
                       "varying vec3 vertWorldPos;\n" \
                       "varying vec3 vertWorldNormal;\n" \
                       "varying float vertHasEdge;\n" \
                       "uniform mat4 objToWorld;\n" \
                       "uniform mat4 cameraPV;\n" \
                       "uniform mat4 normalToWorld;\n" \
                       "void main() {\n" \
                       "  vertWorldPos = (objToWorld * vec4(vertex,1.0)).xyz;\n" \
                       "  vertWorldNormal = (normalToWorld * vec4(normal,1.0)).xyz;\n" \
                       "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                       " // vertHasEdge = hasEdge;\n" \
                       "  gl_FrontColor = color;\n" \
                       "}\n");

    QString geomSource("#version 120\n" \
                       "#extension GL_EXT_gpu_shader4 : enable\n" \
                       "#extension GL_EXT_geometry_shader4 : enable\n" \
                       "varying in vec3 vertWorldPos[3];\n" \
                       "varying in vec3 vertWorldNormal[3];\n" \
                       "varying in float vertHasEdge[3];\n" \
                       "varying out vec3 worldNormal;\n" \
                       "varying out vec3 worldPos;\n" \
                       "uniform vec2 WIN_SCALE;\n" \
                       "noperspective varying vec3 dist;\n" \
                       "void main(void)\n" \
                   "{\n" \
                   "  // taken from 'Single-Pass Wireframe Rendering'\n"
                   "  vec2 p0 = WIN_SCALE * gl_PositionIn[0].xy/gl_PositionIn[0].w;\n" \
                   "  vec2 p1 = WIN_SCALE * gl_PositionIn[1].xy/gl_PositionIn[1].w;\n" \
                   "  vec2 p2 = WIN_SCALE * gl_PositionIn[2].xy/gl_PositionIn[2].w;\n" \
                   "  vec2 v0 = p2-p1;\n" \
                   "  vec2 v1 = p2-p0;\n" \
                   "  vec2 v2 = p1-p0;\n" \
                   "  float area = abs(v1.x*v2.y - v1.y * v2.x);\n" \
                   "  dist = vec3(area/length(v0),0,0);\n" \
                   "  worldPos = vertWorldPos[0];\n" \
                   "  worldNormal = vertWorldNormal[0];\n" \
                   "  gl_Position = gl_PositionIn[0];\n" \
                   "  EmitVertex();\n" \
                   "  dist = vec3(0,area/length(v1),0);\n" \
                   "  worldPos = vertWorldPos[1];\n" \
                   "  worldNormal = vertWorldNormal[1];\n" \
                   "  gl_Position = gl_PositionIn[1];\n" \
                   "  EmitVertex();\n" \
                   "  dist = vec3(0,0,area/length(v2));\n" \
                   "  worldPos = vertWorldPos[2];\n" \
                   "  worldNormal = vertWorldNormal[2];\n" \
                   "  gl_Position = gl_PositionIn[2];\n" \
                   "  EmitVertex();\n" \
                   "  EndPrimitive();\n" \
                   "}\n");

    QString fragSource("#version 120\n" \
                       "#extension GL_EXT_gpu_shader4 : enable\n" \
                       "varying vec3 worldPos;\n" \
                       "varying vec3 worldNormal;\n" \
                       "noperspective varying vec3 dist;\n" \
                       "uniform vec3 cameraPos;\n" \
                       "uniform vec3 lightDir;\n" \
                       "uniform vec4 singleColor;\n" \
                       "uniform float isSingleColor;\n" \
                       "void main() {\n" \
                       "    // determine frag distance to closest edge\n" \
                       "    float nearD = min(min(dist[0],dist[1]),dist[2]);\n" \
                       "    float edgeIntensity = exp2(-1.0*nearD*nearD);\n" \
                       "    vec3 L = lightDir;\n" \
                       "    vec3 V = normalize(cameraPos - worldPos);\n" \
                       "    vec3 N = normalize(worldNormal);\n" \
                       "    vec3 H = normalize(L+V);\n" \
                       "    vec4 color = isSingleColor*singleColor + (1.0-isSingleColor)*gl_Color;\n" \
                       "    float amb = 0.6;\n" \
                       "    vec4 ambient = color * amb;\n" \
                       "    vec4 diffuse = color * (1.0 - amb) * max(dot(L, N), 0.0);\n" \
                       "    vec4 specular = vec4(0.0);\n" \
                       "    gl_FragColor = (edgeIntensity * vec4(0.1,0.1,0.1,1.0)) + ((1.0-edgeIntensity) * vec4(ambient + diffuse + specular));\n" \
                       "    //gl_FragColor = vec4(nearD*0.1);\n" \
                       "}\n");

    std::cout << vertSource.toStdString() << std::endl;
    std::cout << geomSource.toStdString() << std::endl;
    std::cout << fragSource.toStdString() << std::endl;

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

/*
Shader::Shader(string vertSource, string fragSource)
{
    this->_vertSource = vertSource;
    this->_fragSource = fragSource;
    _valid = false;
}

Shader *Shader::buildFlatShader()
{
    string vertSource = string("in vec3 vertex;\n" \
                               "in vec4 color;\n" \
                               "uniform mat4 objToWorld;\n" \
                               "uniform mat4 cameraPV;\n" \
                               "void main() {\n" \
                               "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                               "  gl_FrontColor = color;\n" \
                               "}\n");

    string fragSource = string("uniform vec4 overrideColor;\n" \
                               "uniform float overrideStrength;\n" \
                               "void main() {\n" \
                               "  gl_FragColor = (1.0-overrideStrength) * gl_Color + overrideStrength * overrideColor;\n" \
                               "}\n");

    return new Shader(vertSource, fragSource);
}

void Shader::validate()
{
    if (!_valid) {
      // compile the vert shader
      vertShader = gl.glCreateShader(GL_VERTEX_SHADER);
      glShaderSource(vertShader, vertSource.length, vertSource, vertLengths, 0);
      glCompileShader(vertShader);

      // compile the frag shader
      fragShader = glCreateShader(GL2ES2.GL_FRAGMENT_SHADER);
      glShaderSource(fragShader, fragSource.length, fragSource, fragLengths, 0);
      glCompileShader(fragShader);

      // create the program
      program = gl.glCreateProgram()
      gl.glAttachShader(program, vertShader)
      gl.glAttachShader(program, fragShader)
      gl.glLinkProgram(program)

      checkShaderObjectInfoLog(gl)
    }

    _valid = true;
}

Shader *Shader::buildShader()
{
    string vertSource = string("in vec3 vertex;\n" \
                               "in vec3 normal;\n" \
                               "in vec4 color;\n" \
                               "varying vec3 worldNormal;\n" \
                               "varying vec3 worldPos;\n" \
                               "uniform vec3 cameraPos;\n" \
                               "uniform mat4 objToWorld;\n" \
                               "uniform mat4 cameraV;\n" \
                               "uniform mat4 cameraP;\n" \
                               "uniform mat4 cameraPV;\n" \
                               "void main() {\n" \
                               "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                               "  gl_FrontColor = color;\n" \
                               "  gl_BackColor = color;\n" \
                               "  worldPos = vertex;\n" \
                               "  worldNormal = normal;\n" \
                               "}\n");

    string fragSource = string("varying vec3 worldNormal;\n" \
                               "varying vec3 worldPos;\n" \
                               "uniform vec3 cameraPos;\n" \
                               "uniform vec3 lightDir;\n" \
                               "uniform vec4 singleColor;\n" \
                               "uniform float isSingleColor;\n" \
                               "void main() {\n" \
                               "    vec3 L = lightDir;\n" \
                               "    vec3 V = normalize(cameraPos - worldPos);\n" \
                               "    vec3 N = normalize(worldNormal);\n" \
                               "    vec3 H = normalize(L+V);\n" \
                               "    vec4 color = isSingleColor*singleColor + (1.0-isSingleColor)*gl_Color;\n" \
                               "    //vec4 color = gl_Color;\n" \
                               "    float amb = .4;\n" \
                               "    vec4 ambient = color * amb;\n" \
                               "    vec4 diffuse = color * (1.0 - amb) * max(dot(L, N), 0.0);\n" \
                               "    vec4 specular = vec4(0);\n" \
                               "    gl_FragColor = vec4(ambient + diffuse + specular);\n" \
                               "}\n");

    return new Shader(vertSource, fragSource);
}
*/

