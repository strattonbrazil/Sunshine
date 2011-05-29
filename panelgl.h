#ifndef PANELGL_H
#define PANELGL_H

#include <GL/glew.h>
#include <QGLWidget>
#include <QGLBuffer>
#include "shader.h"
#include "util.h"
#include "camera.h"


class PanelGL : public QGLWidget
{
public:
                          PanelGL();
    void             	  initializeGL();
    void            	  paintGL();
    void 	          resizeGL(int width, int height);
    QGLFormat             defaultFormat();
    Camera*               camera;
    QGLShaderProgram*     getFlatShader() { return _flatShader; }
private:
    bool                  _validShaders;
    QGLShaderProgram*     _flatShader;
    QGLShaderProgram*     _dummyShader;
};

struct LineSegment {
    Point3 p1;
    Point3 p2;
    float r;
    float g;
    float b;
};

class LineRenderer
{
public:
                          LineRenderer(QVector<LineSegment> segments, float lineWidth);
    void                  render(PanelGL* panel);
    void                  loadVBOs(PanelGL* panel);
private:
    bool                  _validVBOs;
    float                 _lineWidth;
    GLuint                _vboIds[2];
};

#endif // PANELGL_H
