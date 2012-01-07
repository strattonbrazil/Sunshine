#include "shader.h"
#include "util.h"

//#include <iostream>
//using namespace std;

#define VERTEX_CHECK \
"vec4 Ap, Bp, A_p, B_p;\n" \
"if (p0.z > 0) {\n" \
"  if (p1.z > 0) {\n" \
"    if (p2.z > 0) { // case 0\n" \
"      // easy standard case\n" \
"    }\n" \
"    else { // case 1\n" \
"      Ap = p0;\n" \
"      Bp = p1;\n" \
"      A_p = p2;\n" \
"      B_p = p2;\n" \
"    }\n" \
"  }\n" \
"  else {\n" \
"    if (p2.z > 0) { // case 2\n" \
"      Ap = p0;\n" \
"      Bp = p2;\n" \
"      A_p = p1;\n" \
"      B_p = p1;\n" \
"    }\n" \
"    else { // case 3\n" \
"      Ap = p0;\n" \
"      Bp = p0;\n" \
"      A_p = p1;\n" \
"      B_p = p2;\n" \
"    }\n" \
"  }\n" \
"}\n" \
"else {\n" \
"  if (p1.z > 0) {\n" \
"    if (p2.z > 0) { // case 4\n" \
"      Ap = p1;\n" \
"      Bp = p2;\n" \
"      A_p = p0;\n" \
"      B_p = p0;\n" \
"    }\n" \
"    else { // case 5\n" \
"      Ap = p1;\n" \
"      Bp = p1;\n" \
"      A_p = p0;\n" \
"      B_p = p2;\n" \
"    }\n" \
"  }\n" \
"  else {\n" \
"    if (p2.z > 0) { // case 6\n" \
"      Ap = p2;\n" \
"      Bp = p2;\n" \
"      A_p = p0;\n" \
"      B_p = p1;\n" \
"    }\n" \
"    else { // case 7\n" \
"      // triangle not visible\n" \
"    }\n" \
"  }\n" \
"}\n"

QGLShaderProgram* ShaderFactory::buildShader(QObject *parent, QString vertFile, QString fragFile)
{
    QFile vertF(vertFile);
    QFile fragF(fragFile);
    vertF.open(QIODevice::ReadOnly);
    fragF.open(QIODevice::ReadOnly);
    QString vertSource = QTextStream(&vertF).readAll();
    QString fragSource = QTextStream(&fragF).readAll();

    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource);

    QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    fragShader->compileSourceCode(fragSource);

    QGLShaderProgram* program = new QGLShaderProgram(parent);
    program->addShader(vertShader);
    program->addShader(fragShader);

    program->link();

    //cout << program->log() << endl;
    //cout << QString("Log end--") << endl;

    return program;
}

QGLShaderProgram* ShaderFactory::buildFlatShader(QObject *parent)
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

    QGLShaderProgram* program = new QGLShaderProgram(parent);
    program->addShader(vertShader);
    program->addShader(fragShader);

    program->link();

    //cout << program->log() << endl;
    //cout << QString("Log end--") << endl;

    return program;
}

QGLShaderProgram* ShaderFactory::buildMeshShader(QObject *parent)
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
                   "  float MEW = 10.0; // max edge width\n" \
                   "  // adapted from 'Single-Pass Wireframe Rendering'\n"
                   "  vec2 p0 = WIN_SCALE * gl_PositionIn[0].xy/gl_PositionIn[0].w;\n" \
                   "  vec2 p1 = WIN_SCALE * gl_PositionIn[1].xy/gl_PositionIn[1].w;\n" \
                   "  vec2 p2 = WIN_SCALE * gl_PositionIn[2].xy/gl_PositionIn[2].w;\n" \
                   "  vec2 v0 = p2.xy-p1.xy;\n" \
                   "  vec2 v1 = p2.xy-p0.xy;\n" \
                   "  vec2 v2 = p1.xy-p0.xy;\n" \
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

    //std::cout << geomSource << std::endl;
    QString fragSource("#version 120\n" \
                       "#extension GL_EXT_gpu_shader4 : enable\n" \
                       "#define STIPPLE_SIZE 2\n" \
                       "varying vec3 worldPos;\n" \
                       "varying vec3 worldNormal;\n" \
                       "varying vec4 selectIndex;\n" \
                       "noperspective varying vec3 dist;\n" \
                       "uniform vec3 cameraPos;\n" \
                       "uniform vec3 lightDir;\n" \
                       "uniform vec4 singleColor;\n" \
                       "uniform float isSingleColor;\n" \
                       "uniform float stipple;\n" \
                       "uniform float stippleFaces;\n" \
                       "uniform float edgeThickness;\n" \
                       "void main() {\n" \
                       "    // determine frag distance to closest edge\n" \
                       "    float nearD = min(min(dist[0],dist[1]),dist[2]);\n" \
                       "    float edgeIntensity = exp2(-edgeThickness*nearD*nearD);\n" \
                       "    vec3 L = lightDir;\n" \
                       "    vec3 V = normalize(cameraPos - worldPos);\n" \
                       "    vec3 N = normalize(worldNormal);\n" \
                       "    vec3 H = normalize(L+V);\n" \
                       "    vec4 color = isSingleColor*singleColor + (1.0-isSingleColor)*gl_Color;\n" \
                       "    float amb = 0.6;\n" \
                       "    vec4 ambient = color * amb;\n" \
                       "    vec4 diffuse = color * (1.0 - amb) * max(dot(L, N), 0.0);\n" \
                       "    vec4 specular = vec4(0.0);\n" \
                       "    "
                       "    float stippleMaskX = int(mod(gl_FragCoord.x / STIPPLE_SIZE, 2.0)) * int(mod((gl_FragCoord.x-STIPPLE_SIZE+1) / STIPPLE_SIZE, 2.0));\n" \
                       "    float stippleMaskY = int(mod(gl_FragCoord.y / STIPPLE_SIZE, 2.0)) * int(mod((gl_FragCoord.y-STIPPLE_SIZE+1) / STIPPLE_SIZE, 2.0));\n" \
                       "    vec4 combined = mix(vec4(ambient + diffuse + specular), vec4(0.3,0,0.3,1), stippleFaces * stippleMaskX * stippleMaskY);\n" \
                       "    gl_FragData[0] = (edgeIntensity * vec4(0.1,0.1,0.1,1.0)) + ((1.0-edgeIntensity) * combined);\n" \
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

    QGLShaderProgram* program = new QGLShaderProgram(parent);
    program->addShader(vertShader);
    program->addShader(geomShader);
    program->addShader(fragShader);

    return program;
}

QGLShaderProgram* ShaderFactory::buildVertexShader(QObject *parent)
{
    QString vertSource("#version 130\n" \
                       "in vec3 vertex;\n" \
                       "in float colorIndex;\n" \
                       "in float visible;\n" \
                       "uniform vec4 colors[4]; // unselected, highlighted, selected, highlighted\n" \
                       "uniform mat4 objToWorld;\n" \
                       "uniform mat4 cameraPV;\n" \
                       "void main() {\n" \
                       "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                       "  gl_FrontColor = colors[int(colorIndex)];\n" \
                       "  gl_FrontColor.a = visible;\n" \
                       "  //gl_FrontColor = vec4(colorIndex);\n" \
                       "}\n");

    QString fragSource("uniform vec4 overrideColor;\n" \
                       "uniform float overrideStrength;\n" \
                       "in float discardFragment;"
                       "void main() {\n" \
                       "  discard;\n" \
                       "  if (discardFragment > 0.5) discard;\n" \
                       "  gl_FragColor = (1.0-overrideStrength) * gl_Color + overrideStrength * overrideColor;\n" \
                       "}\n");


    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource);

    //QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    //fragShader->compileSourceCode(fragSource);

    QGLShaderProgram* program = new QGLShaderProgram(parent);
    program->addShader(vertShader);
    //program->addShader(fragShader);

    program->link();

    return program;
}

QGLShaderProgram* ShaderFactory::buildPropertyShader(QObject *parent)
{
    QString vertSource("in vec3 vertex;\n" \
                       "in vec4 color;\n" \
                       "uniform mat4 objToWorld;\n" \
                       "uniform mat4 cameraPV;\n" \
                       "void main() {\n" \
                       "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                       "  gl_FrontColor = color;\n" \
                       "}\n");

    QString fragSource("void main() {\n" \
                       "  gl_FragColor = gl_Color;\n" \
                       "}\n");


    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource);

    QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    fragShader->compileSourceCode(fragSource);

    QGLShaderProgram* program = new QGLShaderProgram(parent);
    program->addShader(vertShader);
    program->addShader(fragShader);

    program->link();

    //cout << program->log() << endl;
    //cout << QString("Log end--") << endl;

    return program;
}

QGLShaderProgram* ShaderFactory::buildMaterialShader(Light* light, Material* material, QObject *parent)
{
    QHash<QString,QString> typeToGL;
    typeToGL["color"] = "vec3";
    typeToGL["float"] = "float";
    typeToGL["point3"] = "vec3";
    typeToGL["vector3"] = "vec3";
    typeToGL["samplerCubeShadow"] = "samplerCubeShadow";
    typeToGL["sampler2DShadow"] = "sampler2DShadow";
    typeToGL["mat4"] = "mat4";

    // write vertex shader
    //
    QString vertSource;
    vertSource += "#version 130\n" \
                  "uniform mat4 objToWorld;\n" \
                  "uniform mat4 cameraPV;\n" \
                  "uniform mat4 normalToWorld;\n" \
                  "in vec3 vertex;\n" \
                  "in vec3 vertNormal;\n" \
                  "out vec3 worldPos;\n" \
                  "out vec3 fragNormal;\n";
    vertSource += "void main() {\n" \
                  "  worldPos = (objToWorld * vec4(vertex,1.0)).xyz;\n" \
                  "  fragNormal = (normalToWorld * vec4(vertNormal,1.0)).xyz;\n" \
                  "  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);\n" \
                  "}\n";

    // write fragment shader
    //
    QString fragSource;
    fragSource += "#version 130\n";
    fragSource += "#extension GL_EXT_gpu_shader4 : enable\n";
    fragSource += "#define zShadowNear 0.1\n";
    fragSource += "#define zShadowFar 100.0\n";

    //for (int i = 0; i < material->constantAttributes()->attributeCount(); i++) {
    /*
    foreach(Attribute attribute, material->glslFragmentConstants()) {
        //Attribute attribute = material->constantAttributes()->at(i);
        QString type = typeToGL[attribute->property("type").toString()];
        QString var = attribute->property("var").toString();
        fragSource += QString("uniform %1 %2;\n").arg(type).arg(var);
    }
    */
    foreach(Attribute attribute, light->glslFragmentConstants()) {
        QString type = typeToGL[attribute->property("type").toString()];
        QString varName = attribute->property("glslFragmentConstant").toString();
        if (varName == "" || varName == "true")
            varName = attribute->property("var").toString();
        fragSource += QString("uniform %1 %2;\n").arg(type).arg(varName);
    }
    fragSource += "in vec3 worldPos;\n";
    fragSource += "in vec3 fragNormal;\n";

    fragSource += "void main() {\n";
    fragSource += "  vec3 normal = normalize(fragNormal);\n";
    fragSource += light->glslFragmentBegin();
    //fragSource += material->glslFragmentCode(); //"  gl_FragColor = vec4(normal,1);\n";
    fragSource += light->glslFragmentEnd();
    fragSource += "}\n";


    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource);

    QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    fragShader->compileSourceCode(fragSource);

    QGLShaderProgram* program = new QGLShaderProgram(parent);
    program->addShader(vertShader);
    program->addShader(fragShader);

    program->link();


    //std::cout << "-- vert source -----" << std::endl;
    //std::cout << vertSource.toStdString() << std::endl;

    std::cout << "\n-- frag source -----" << std::endl;
    std::cout << fragSource.toStdString() << std::endl;


    //cout << program->log() << endl;
    //cout << QString("Log end--") << endl;

    return program;
}


QGLShaderProgram* ShaderFactory::buildDistanceShader(QObject *parent)
{
    QString vertSource("#define zShadowNear 0.1\n" \
                       "#define zShadowFar 100.0\n" \
                       "uniform mat4 objToLight;\n" \
                       "uniform vec4 d0;\n" \
                       "//uniform mat4 worldToLight;\n" \
                       "in vec3 vertex;\n" \
                       "//out vec3 parabP;\n" \
                       "void main() {\n" \
                       "  float zScale = zShadowFar;\n" \
                       "  float zBias = 0.005;\n" \
                       "  //vec4 d0 = vec4(0,0,1,0);\n" \
                       "  vec4 P = (objToLight * vec4(vertex,1.0));\n" \
                       "  P = P / P.w;\n" \
                       "  float alpha = 0.5 + (P.z/zScale);\n" \
                       "  float lengthP = length(P);\n" \
                       "  P = P / lengthP;\n" \
                       "  P = P + d0;\n" \
                       "  P.x = P.x / P.z;\n" \
                       "  P.y = P.y / P.z;\n" \
                       "  P.z = (lengthP-zShadowNear)/(zShadowFar-zShadowNear) + zBias;\n" \
                       "  P.w = 1.0;\n" \
                       "  gl_Position = P;\n" \
                       "}\n");

    QString fragSource("uniform vec3 origin;\n" \
                       "#define zShadowNear 0.1\n" \
                       "#define zShadowFar 1000.0\n" \
                       "in vec3 parabP;\n" \
                       "void main() {\n" \
                       "  //gl_FragData[0] = vec4(1,0,0,0);\n" \
                       "  gl_FragDepth = (distance(origin,world)-zShadowNear)/(zShadowFar-zShadowNear);\n" \
                       "}\n");


    QGLShader* vertShader = new QGLShader(QGLShader::Vertex);
    vertShader->compileSourceCode(vertSource);

    //QGLShader* fragShader = new QGLShader(QGLShader::Fragment);
    //fragShader->compileSourceCode(fragSource);

    QGLShaderProgram* program = new QGLShaderProgram(parent);
    program->addShader(vertShader);
    //program->addShader(fragShader);

    program->link();

    //std::cout << program->log() << std::endl;
    //std::cout << QString("Log end--") << std::endl;

    return program;
}
