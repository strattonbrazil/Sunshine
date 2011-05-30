#include "panelgl.h"
#include "camera.h"

// possible reference
// http://code.google.com/p/opencamlib/source/browse/trunk/cpp_examples/qt_opengl_vbo/glwidget.h?spec=svn688&r=688

#include <QVarLengthArray>
#include <QMouseEvent>

#include <iostream>
using namespace std;

LineRenderer* mainGrid = NULL;
int workMode = WorkMode::FREE;
int workButton = -1;

PanelGL::PanelGL() : QGLWidget(PanelGL::defaultFormat())
{
    setMouseTracking(true);
    _validShaders = false;

    camera = new Camera();

    if (mainGrid == NULL) {
        int range[] = {-10,10};
        int numSegments = range[1]-range[0]+1;

        QVector<LineSegment> segments(numSegments*2);
        for (int i = 0; i < numSegments*2; i += 2) {
            segments[i].p1 = Point3(range[0]+i/2, 0, 10);
            segments[i].p2 = Point3(range[0]+i/2, 0, -10);
            segments[i].r = 0.4f;
            segments[i].g = 0.4f;
            segments[i].b = 0.4f;

            segments[i+1].p1 = Point3(-10, 0, range[0]+i/2);
            segments[i+1].p2 = Point3(10, 0, range[0]+i/2);
            segments[i+1].r = 0.4f;
            segments[i+1].g = 0.4f;
            segments[i+1].b = 0.4f;
        }
        mainGrid = new LineRenderer(segments, 1);
    }
}

QGLFormat PanelGL::defaultFormat()
{
    QGLFormat format;
    format.setVersion(3,2);
    format.setProfile(QGLFormat::CompatibilityProfile);
    return format;
}

bool glewInitialized = false;

void PanelGL::initializeGL()
{
    if (!glewInitialized) {
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            /* Problem: glewInit failed, something is seriously wrong. */
            cerr << "Error: " << glewGetErrorString(err) << endl;
        }
    }
}

void PanelGL::paintGL()
{   
    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glEnable(GL_DEPTH_TEST);

    if (!_validShaders) {
        _dummyShader = ShaderFactory::buildShader(this);
        _flatShader = ShaderFactory::buildFlatShader(this);
        _validShaders = true;
    }

    // render the grid
    mainGrid->render(this);

    //glDisable(GL_DEPTH_TEST);
}

void PanelGL::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
}

LineRenderer::LineRenderer(QVector<LineSegment> segments, float lineWidth)
{
    _validVBOs = FALSE;
    _segments = segments;
    _lineWidth = lineWidth;
}

struct VertexColorData
{
    Vector3 position;
    Vector4 color;
};

void LineRenderer::render(PanelGL* panel)
{
    if (!_validVBOs) {
        glGenBuffers(2, _vboIds);

        _validVBOs = TRUE;
    }

    loadVBOs(panel);

    Camera* camera = panel->camera;
    QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
    QMatrix4x4 objToWorld;

    QGLShaderProgram* flatShader = panel->getFlatShader();

    glLineWidth(_lineWidth);
    flatShader->bind();
    flatShader->setUniformValue("objToWorld", objToWorld);
    flatShader->setUniformValue("cameraPV", cameraProjViewM);
    flatShader->setUniformValue("overrideStrength", 0.0f);

    int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = flatShader->attributeLocation("vertex");
    flatShader->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColorData), (const void *)offset);
    // Offset for texture coordinate
    offset += sizeof(QVector3D);
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int colorLocation = flatShader->attributeLocation("color");
    flatShader->enableAttributeArray(colorLocation);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(VertexColorData), (const void *)offset);
    // Draw cube geometry using indices from VBO 1
    //glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINES, _segments.size()*2, GL_UNSIGNED_SHORT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    flatShader->release();
}

void LineRenderer::loadVBOs(PanelGL* panel)
{
    VertexColorData vertices[_segments.size()*2];
    for (int i = 0; i < _segments.size(); i++) {
        vertices[2*i] = { _segments[i].p1, QVector4D(_segments[i].r, _segments[i].g, _segments[i].b, 1.0) };
        vertices[2*i+1] = { _segments[i].p2, QVector4D(_segments[i].r, _segments[i].g, _segments[i].b, 1.0) };
    }

    GLushort indices[_segments.size()*2];
    for (int i = 0; i < _segments.size()*2; i++)
        indices[i] = i;

    // Transfer vertex data to VBO 0
    glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, _segments.size()*2*sizeof(VertexColorData), vertices, GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _segments.size()*2*sizeof(GLushort), indices, GL_STATIC_DRAW);
}

void PanelGL::mousePressEvent(QMouseEvent* event)
{
    bool altDown = event->modifiers() & Qt::AltModifier;

    if (workMode == WorkMode::TOOL) {
        //workTool.mousePressed(event);
    }
    else if (workMode == WorkMode::FREE && altDown) {
        workMode = WorkMode::CAMERA;
        workButton = event->button();
        camera->mousePressed(event);
    }
    else if (workMode == WorkMode::FREE && event->button() & Qt::LeftButton) {
        workMode = WorkMode::SELECT;
        workButton = event->button();
        //basicSelect = BasicSelect.instance
        //basic_select.mousePressed(self,event)
    }
}

void PanelGL::mouseReleaseEvent(QMouseEvent* event)
{
    if (workMode == WorkMode::TOOL) {
        //workTool->mouseReleased
        workButton = WorkMode::FREE;
        workButton = -1;
    }
    else if (workMode == WorkMode::CAMERA && event->button() == workButton) {
        workMode = WorkMode::FREE;
        workButton = -1;
        camera->mouseReleased(event);
    }
    else if (workMode == WorkMode::SELECT && event->button() == workButton) {
        workMode = WorkMode::FREE;
        workButton = -1;
        //basic_select = BasicSelect.instance
        //basic_select.mouseRelease(self, event)
    }
    else if (workMode == WorkMode::FREE && event->button() == Qt::RightButton) { // popup menu
        cout << "Popup menu" << endl;
    }

    update();
}

void PanelGL::mouseMoveEvent(QMouseEvent* event)
{
    if (workMode == WorkMode::FREE) {

    }
    else if (workMode == WorkMode::TOOL) {
        //work_tool.mouseMoved(event)
        update();
    }
    else {
        mouseDragEvent(event);
    }
}

void PanelGL::mouseDragEvent(QMouseEvent* event)
{
    if (workMode == WorkMode::CAMERA) {
        camera->mouseDragged(event);

    }
    else if (workMode == WorkMode::SELECT) {
        //basic_select = BasicSelect.instance
        //basic_select.mouseDrag(self, event)
    }

    update();

}
