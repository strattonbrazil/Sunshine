#ifndef PANELGL_H
#define PANELGL_H

#include <GL/glew.h>
#include <QGLWidget>
#include <QGLBuffer>
#include "shader.h"
#include "util.h"
#include "camera.h"
#include "geometry.h"


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
    void                  mousePressEvent(QMouseEvent* event);
    void                  mouseReleaseEvent(QMouseEvent* event);
    void                  mouseMoveEvent(QMouseEvent* event);
    void                  mouseDragEvent(QMouseEvent* event);
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
    QVector<LineSegment>  _segments;
    bool                  _validVBOs;
    float                 _lineWidth;
    GLuint                _vboIds[2];
};

class MeshRenderer
{
public:
                          MeshRenderer(int meshKey);
    void                  render(PanelGL* panel);
    void                  loadVBOs(PanelGL* panel, Mesh* mesh);
private:
    int                   _meshKey;
    bool                  _validVBOs;
    GLuint                _vboIds[5];
};

#endif // PANELGL_H
