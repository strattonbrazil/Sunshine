#ifndef PANELGL_H
#define PANELGL_H

#include <GL/glew.h>
#include <QGLWidget>
#include <QGLBuffer>
#include "shader.h"
#include "util.h"
#include "camera.h"
#include "geometry.h"
#include "scene.h"
#include "worktool.h"
//#include "select.h"
#include "sunshine.h"

class Mesh;
class Triangle;
class PanelGL;
class Sunshine;
class WorkTool;
typedef QSharedPointer<WorkTool> WorkToolP;

//class BasicSelect;
//typedef QSharedPointer<BasicSelect> BasicSelectP;

class MeshRenderer
{
public:
                          MeshRenderer(QString meshName);
    void                  render(PanelGL* panel);
    void                  renderFaces(PanelGL* panel);
    void                  renderVertices(PanelGL* panel);
    void                  loadVBOs(PanelGL* panel, MeshP mesh);
private:
    QString                   _meshName;
    bool                  _validVBOs;
    GLuint                _vboIds[3];
};
typedef QSharedPointer<MeshRenderer> MeshRendererP;

class MeshGrid
{
public:
                            MeshGrid() {}
                            MeshGrid(int xres, int yres, QList<Triangle> triangles);
    QList<Triangle>         trianglesByPoint(QPoint p);
private:
    int                           xCells;
    int                           yCells;
    static const int              PIXEL_SIZE = 40;
    QHash<int,QList<Triangle> >   cells;
};

class PanelGL : public QGLWidget
{
    Q_OBJECT
public:
                             PanelGL(SceneP scene, Sunshine* sunshine);
                             PanelGL(const PanelGL &panel);
    void             	     initializeGL();
    void               	     paintGL();
    void                     paintBackground();
    void 	             resizeGL(int width, int height);
    void                     initFBO(int width, int height);
    QGLFormat                defaultFormat();
    QGLShaderProgramP        getFlatShader() { return _flatShader; }
    QGLShaderProgramP        getMeshShader() { return _meshShader; }
    QGLShaderProgramP        getVertexShader() { return _vertexShader; }
    CameraP                  camera() const { return _camera; }
    void                     enterEvent(QEvent *);
    void                     mousePressEvent(QMouseEvent* event);
    void                     mouseReleaseEvent(QMouseEvent* event);
    void                     mouseMoveEvent(QMouseEvent* event);
    void                     mouseDragEvent(QMouseEvent* event);
    Point3                   project(Point3 p);
    Point3                   unproject(Point3 p);
    Vector3                  computeRayDirection(QPoint p);
    SceneP                   scene() const { return _scene; }
    Sunshine*                sunshine() const { return _sunshine; }
    void                     showContextMenu(QMouseEvent* event);
    QPoint                   centerMouse(bool mock);
    void                     setArrowCursor();
    void                     setBlankCursor();
    void                     renderBeautyPass();

    // for preselection
    MeshP                    _hoverMesh;
    FaceP                    _hoverFace;
    VertexP                  _hoverVert;
    MeshGrid                 _meshGrid;

public slots:
    void                     initWorkTool(WorkTool* tool, QString command, int button);

private:
    void                     init();
    void                     ravageMouse();
    void                     buildMeshGrid();
    void                     renderWorkPass();
    bool                     _ravagingMouse;
    bool                     _validShaders;
    QVector<GLubyte>         _selectionBuffer;
    GLuint                   _fbo, _beautyTexture, _indexTexture, _depthTexture;
    bool                     _validSelectionBuffer;
    CameraP                  _camera;
    QGLShaderProgramP        _flatShader;
    QGLShaderProgramP        _meshShader;
    QGLShaderProgramP        _vertexShader;
    QHash<QString,MeshRendererP> _meshRenderers;
    SceneP                   _scene;
    WorkTool*                _workTool;
    //BasicSelectP             _basicSelect;
    Sunshine*                _sunshine;
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



#endif // PANELGL_H
