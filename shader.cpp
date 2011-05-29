#include "shader.h"

#include <iostream>
using namespace std;

QGLShaderProgram* ShaderFactory::buildFlatShader(QObject *parent)
{
    string vertSource = string("in vec3 vertex;\n" \
                               "in vec4 color;\n" \
                               "in vec2 a_texcoord;\n" \
                               "uniform mat4 objToWorld;\n" \
                               "uniform mat4 cameraPV;\n" \
                               "void main() {\n" \
                               "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0) + vec4(0.0001*a_texcoord.r);\n" \
                               "  gl_FrontColor = color;\n" \
                               "}\n");

    string fragSource = string("uniform vec4 overrideColor;\n" \
                               "uniform float overrideStrength;\n" \
                               "void main() {\n" \
                               "  gl_FragColor = (1.0-overrideStrength) * gl_Color + overrideStrength * overrideColor;\n" \
                               "}\n");
/*
    string vertSource = string("uniform mat4 objToWorld;\n" \
                               "uniform mat4 cameraPV;\n" \
                               "void main() {\n" \
                               "  gl_Position = cameraPV * objToWorld * gl_Vertex;\n" \
                               "  //gl_Position = ftransform();\n" \
                               "  //gl_Position = gl_Vertex;\n" \
                               "  gl_FrontColor = vec4(0, 1, 0, 1);\n" \
                               "}\n");

    string fragSource = string("void main() {\n" \
                               "  gl_FragColor = vec4(1,0,0,0);\n" \
                               "}\n");
                               */

    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource.c_str());

    QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    fragShader->compileSourceCode(fragSource.c_str());

    QGLShaderProgram* program = new QGLShaderProgram(parent);
    program->addShader(vertShader);
    program->addShader(fragShader);

    program->link();

    cout << program->log().toStdString() << endl;
    cout << "Log end--" << endl;

    return program;
}

QGLShaderProgram* ShaderFactory::buildShader(QObject *parent)
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

    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource.c_str());

    QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    fragShader->compileSourceCode(fragSource.c_str());

    QGLShaderProgram* program = new QGLShaderProgram(parent);
    program->addShader(vertShader);
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

