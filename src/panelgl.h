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
#include "select.h"
#include "sunshine.h"

class Mesh;
class PanelGL;
class Sunshine;
class WorkTool;
typedef QSharedPointer<WorkTool> WorkToolP;

class BasicSelect;
typedef QSharedPointer<BasicSelect> BasicSelectP;

class MeshRenderer
{
public:
                          MeshRenderer(int meshKey);
    void                  render(PanelGL* panel);
    void                  loadVBOs(PanelGL* panel, MeshP mesh);
private:
    int                   _meshKey;
    bool                  _validVBOs;
    GLuint                _vboIds[5];
};
typedef QSharedPointer<MeshRenderer> MeshRendererP;

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
    QGLFormat                defaultFormat();
    QGLShaderProgramP        getFlatShader() { return _flatShader; }
    QGLShaderProgramP        getMeshShader() { return _meshShader; }
    CameraP                  camera() const { return _camera; }
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

    // for preselection
    int                      _hoverMeshKey;

public slots:
    void                     initWorkTool(WorkTool* tool, QString command, int button);

private:
    void                     init();
    void                     ravageMouse();
    bool                     _ravagingMouse;
    bool                     _validShaders;
    CameraP                  _camera;
    QGLShaderProgramP        _flatShader;
    QGLShaderProgramP        _meshShader;
    QHash<int,MeshRendererP> _meshRenderers;
    SceneP                   _scene;
    WorkTool*                _workTool;
    BasicSelectP             _basicSelect;
    Sunshine*                _sunshine;

    /*
    MeshP                    _closestMesh;
    FaceP                    _closestFace;
    VertexP                  _closestVertex;
    EdgeP                    _closestEdge;
    */
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
