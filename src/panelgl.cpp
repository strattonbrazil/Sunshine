#include "panelgl.h"

#include <QVarLengthArray>
#include <QMouseEvent>
#include <QMenu>
#include <QGLFramebufferObject>
#include "select.h"
#include "project_util.h"
#include "contextmenu.h"
#include "face_util.h"
#include "vertex_util.h"
#include <algorithm>
#include <QGLPixelBuffer>
#include "cursor_tools.h"

namespace MouseMode {
    enum { FREE, CAMERA, TOOL };
}

LineRenderer* mainGrid = NULL;
int mouseMode = MouseMode::FREE;
int activeMouseButton = -1;

struct VertexColorData
{
    Vector3 v1; // actual vertex to process
    Vector4 color;
};

PanelGL::PanelGL(const PanelGL &panel) : QGLWidget(panel.format())
{ // what was this constructor for again? still need it?
    _camera = panel.camera();
    _scene = panel.scene();
    _sunshine = panel.sunshine();

    init();
}

PanelGL::PanelGL(SceneP scene, Sunshine* sunshine) : QGLWidget(PanelGL::defaultFormat())
{
    _scene = scene;
    _camera = _scene->fetchCamera("persp");
    _sunshine = sunshine;

    init();
}

void PanelGL::init()
{
    setMouseTracking(true);
    _validShaders = false;
    _validSelectionBuffer = FALSE;
    _ravagingMouse = FALSE;
    _fbo, _beautyTexture, _indexTexture, _depthTexture = 0;
    //_basicSelect = BasicSelectP(new BasicSelect());

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
    //format.setVersion(3,2);
    format.setAlpha(TRUE);
    format.setStencil(TRUE);
    format.setVersion(3,1);
    format.setProfile(QGLFormat::CoreProfile);
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
            std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        }
    }


}

void PanelGL::paintGL()
{   
    if (!_validShaders) {
        _flatShader = ShaderFactory::buildFlatShader(this);
        _meshShader = ShaderFactory::buildMeshShader(this);
        _vertexShader = ShaderFactory::buildVertexShader(this);
        _validShaders = true;
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);
    glViewport( 0, 0, width(), height());
    renderWorkPass();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    //renderBeautyPass();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _beautyTexture);
    glColor4f(1,1,1,1);
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0, 0);
        glVertex2f(0, 0);
        glTexCoord2f(1, 0);
        glVertex2f(1, 0);
        glTexCoord2f(1, 1);
        glVertex2f(1, 1);
        glTexCoord2f(0, 1);
        glVertex2f(0, 1);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // copy selection indexes to CPU array
    if (!SunshineUi::selectOccluded() && !_validSelectionBuffer) {
        _selectionBuffer.resize(width()*height()*4);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);
        glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
        glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, _selectionBuffer.data());
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        //glBindTexture(GL_TEXTURE_2D, _indexTexture);
        //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, _selectionBuffer.data());

        _validSelectionBuffer = TRUE;
    }

    QPoint p = this->mapFromGlobal(QCursor::pos());
    p = QPoint(p.x(), height() - p.y());
    GLubyte r = _selectionBuffer[(p.x()+width()*p.y())*4+0];
    GLubyte g = _selectionBuffer[(p.x()+width()*p.y())*4+1];
    GLubyte b = _selectionBuffer[(p.x()+width()*p.y())*4+2];
    GLubyte a = _selectionBuffer[(p.x()+width()*p.y())*4+3];
    //std::cout << "(" << (int)r << "," << (int)g << "," << (int)b << "," << (int)a << ")" << std::endl;
}

void PanelGL::renderWorkPass()
{
    CursorToolP cursorTool = SunshineUi::cursorTool();

    GLenum bufs[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
    glDrawBuffers( 2, bufs);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawBuffers( 1, bufs);

    paintBackground();

    glEnable(GL_DEPTH_TEST);

    cursorTool->preDrawOverlay(this);

    // render the grid
    mainGrid->render(this);

    glDrawBuffers( 2, bufs);

    // render all the meshes
    foreach(QString meshName, _scene->meshes()) {
        MeshP mesh = _scene->mesh(meshName);

        if (!_meshRenderers.contains(meshName)) // create the mesh renderer if it doesn't exist for this mesh
            _meshRenderers[meshName] = MeshRendererP(new MeshRenderer(meshName));

        _meshRenderers[meshName]->render(this);
    }

    glDisable(GL_DEPTH_TEST);

    glDrawBuffers(1, bufs);

    cursorTool->postDrawOverlay(this);
}

void PanelGL::renderBeautyPass()
{
    CursorToolP cursorTool = SunshineUi::cursorTool();

    GLenum bufs[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT,
                      GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,};
    glDrawBuffers( 3, bufs);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glDrawBuffers( 3, bufs);

    // render all the meshes
    /*
    QHashIterator<int,MeshP> meshes = _scene->meshes();
    while (meshes.hasNext()) {
        meshes.next();
        int meshKey = meshes.key();
        MeshP mesh = meshes.value();

        if (!_meshRenderers.contains(meshKey)) // create the mesh renderer if it doesn't exist for this mesh
            _meshRenderers[meshKey] = MeshRendererP(new MeshRenderer(meshKey));

        _meshRenderers[meshKey]->render(this);
    }
    */

    glDisable(GL_DEPTH_TEST);

    glDrawBuffers(1, bufs);
}

void PanelGL::paintBackground()
{
    CameraP camera = _camera;
    //QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera, width(), height());
    //QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera, width(), height());
    //QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width(), 0, height(), -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
    {
        glColor4f(.1,.1,.1,0);
        glVertex2f(0,0);
        glVertex2f(width(),0);
        glColor4f(.3,.3,.3,0);
        glVertex2f(width(),height());
        glVertex2f(0,height());
    }
    glEnd();
}

void PanelGL::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
    buildMeshGrid();
    _validSelectionBuffer = FALSE;

    if (width && height)
        initFBO(width, height);
}

void PanelGL::initFBO(int width, int height)
{
    if (_fbo) {
        glDeleteFramebuffersEXT(1, &_fbo);
        glDeleteTextures( 1, &_beautyTexture);
        glDeleteTextures( 1, &_indexTexture);
        glDeleteTextures( 1, &_depthTexture);
    }

    glGenFramebuffersEXT(1, &_fbo);
    glGenTextures(1, &_beautyTexture);
    glGenTextures(1, &_indexTexture);
    glGenTextures(1, &_depthTexture);

    glBindFramebufferEXT(GL_FRAMEBUFFER, _fbo);

    // setup beauty texture
    glBindTexture(GL_TEXTURE_2D, _beautyTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, _beautyTexture, 0);

    // setup index texture
    glBindTexture(GL_TEXTURE_2D, _indexTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _indexTexture, 0);

    // setup depth texture
    glBindTexture( GL_TEXTURE_2D, _depthTexture);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_EXT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, _depthTexture, 0);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, _depthTexture, 0);

    glBindTexture( GL_TEXTURE_2D, 0);

    switch (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)) {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << std::endl;
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED_EXT" << std::endl;
        break;
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

LineRenderer::LineRenderer(QVector<LineSegment> segments, float lineWidth)
{
    _validVBOs = FALSE;
    _segments = segments;
    _lineWidth = lineWidth;
}

void LineRenderer::render(PanelGL* panel)
{
    if (!_validVBOs) {
        glGenBuffers(2, _vboIds);

        _validVBOs = TRUE;
    }

    loadVBOs(panel);

    glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);

    CameraP camera = panel->camera();
    QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
    QMatrix4x4 objToWorld;

    QGLShaderProgramP flatShader = panel->getFlatShader();

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
    glBufferData(GL_ARRAY_BUFFER, _segments.size()*2*sizeof(VertexColorData), vertices, GL_STREAM_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _segments.size()*2*sizeof(GLushort), indices, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

class MeshVertexData
{
public:
                 MeshVertexData() {}
                 MeshVertexData(Vector3 p, Vector4 c, Vector4 n, bool edge) : position(p), color(c), normal(n) {
                     excludeEdge = edge ? 1 : 0;
                 }
    Vector3      position;
    Vector4      color;
    Vector3      normal;
    GLfloat      excludeEdge;
    GLuint       selectIndex;
};

class VertexData
{
public:
                 VertexData() {}
                 VertexData(Vector3 p, float cIndex, float v) : position(p), colorIndex(cIndex), visible(v) {}
    Vector3      position;
    float        colorIndex;
    float        visible;
};

MeshRenderer::MeshRenderer(QString meshName)
{
    _validVBOs = FALSE;
    _meshName = meshName;
}

void MeshRenderer::render(PanelGL* panel)
{
    if (!_validVBOs) {
        glGenBuffers(2, _vboIds);

        _validVBOs = TRUE;
    }
    MeshP mesh = panel->scene()->mesh(_meshName);
    CursorToolP cursorTool = SunshineUi::cursorTool();
    const int drawSettings = cursorTool->drawSettings(panel, mesh);
    loadVBOs(panel, mesh);

    renderFaces(panel);
    if (drawSettings & DrawSettings::DRAW_VERTICES) {
        if (drawSettings & DrawSettings::CULL_BORING_VERTICES) {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
        }
        renderVertices(panel);
        if (drawSettings & DrawSettings::CULL_BORING_VERTICES) {
            glDisable(GL_BLEND);
        }
    }
}

void MeshRenderer::renderFaces(PanelGL *panel)
{
    glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);

    MeshP mesh = panel->scene()->mesh(_meshName);
    mesh->validateNormals();
    const int numTriangles = mesh->numTriangles();

    CameraP camera = panel->camera();
    QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
    QMatrix4x4 objToWorld = mesh->objectToWorld();
    QMatrix4x4 normalToWorld = mesh->normalToWorld();

    QGLShaderProgramP meshShader = panel->getMeshShader();

    // geometry-shader attributes must be applied prior to linking
    meshShader->setGeometryInputType(GL_TRIANGLES);
    meshShader->setGeometryOutputType(GL_TRIANGLES);
    meshShader->setGeometryOutputVertexCount(numTriangles*3);
    meshShader->bind();
    meshShader->setUniformValue("WIN_SCALE", QVector2D(panel->width(),panel->height()));
    meshShader->setUniformValue("objToWorld", objToWorld);
    meshShader->setUniformValue("normalToWorld", normalToWorld);
    meshShader->setUniformValue("cameraPV", cameraProjViewM);
    meshShader->setUniformValue("cameraPos", camera->eye());
    meshShader->setUniformValue("lightDir", -camera->lookDir().normalized());

    CursorToolP cursorTool = SunshineUi::cursorTool();
    const int drawSettings = cursorTool->drawSettings(panel, mesh);

    if (mesh->isSelected()) {
        meshShader->setUniformValue("edgeThickness", 0.4f);
        meshShader->setUniformValue("stippleFaces", 1.0f);
    }
    else {
        meshShader->setUniformValue("edgeThickness", 1.2f);
        meshShader->setUniformValue("stippleFaces", 0.0f);
    }

    if (drawSettings & DrawSettings::USE_OBJECT_COLOR) {
        QVector4D singleColor;
        if (mesh->isSelected() && mesh != panel->_hoverMesh)
            singleColor = UNSELECTED_COLOR;
        else if (mesh->isSelected() && mesh == panel->_hoverMesh)
            singleColor = SELECTED_HOVER_COLOR;
        else if (mesh == panel->_hoverMesh && DrawSettings::HIGHLIGHT_OBJECTS & drawSettings)
            singleColor = UNSELECTED_HOVER_COLOR;
        else
            singleColor = UNSELECTED_COLOR;

        meshShader->setUniformValue("singleColor", singleColor.x(),
                                                   singleColor.y(),
                                                   singleColor.z(),
                                                   singleColor.w());
        meshShader->setUniformValue("isSingleColor", 1.0f);

        if (drawSettings & DrawSettings::STIPPLE_FACES)
            meshShader->setUniformValue("stipple", 0.6f);
        else
            meshShader->setUniformValue("stipple", 1.0f);
    } else {
        meshShader->setUniformValue("singleColor", 0,0,0,0);
        meshShader->setUniformValue("isSingleColor", 0.0f);
        meshShader->setUniformValue("stipple", 1.0f);
    }


    int offset = 0;

    // tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = meshShader->attributeLocation("vertex");
    meshShader->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertexData), (const void *)offset);

    offset += sizeof(QVector3D);

    // tell OpenGL programmable pipeline how to locate vertex color data
    int colorLocation = meshShader->attributeLocation("color");
    meshShader->enableAttributeArray(colorLocation);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVertexData), (const void *)offset);

    offset += sizeof(QVector4D);

    // tell OpenGL programmable pipeline how to locate vertex normal data
    int normalLocation = meshShader->attributeLocation("normal");
    meshShader->enableAttributeArray(normalLocation);
    glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertexData), (const void *)offset);

    offset += sizeof(QVector3D);

    // tell OpenGL programmable pipeline if to draw opposing edge
    int excludeEdgeLocation = meshShader->attributeLocation("excludeEdge");
    meshShader->enableAttributeArray(excludeEdgeLocation);
    glVertexAttribPointer(excludeEdgeLocation, 1, GL_FLOAT, GL_FALSE, sizeof(MeshVertexData), (const void *)offset);

    offset += sizeof(GLfloat);

    // tell OpenGL programmable pipeline if to draw opposing edge
    int selectIndexLocation = meshShader->attributeLocation("selectIndex");
    meshShader->enableAttributeArray(selectIndexLocation);
    glVertexAttribPointer(selectIndexLocation, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(MeshVertexData), (const void *)offset);

    glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    meshShader->release();
}

void MeshRenderer::renderVertices(PanelGL *panel)
{
    MeshP mesh = panel->scene()->mesh(_meshName);
    const int numVertices = mesh->numVertices();

    glBindBuffer(GL_ARRAY_BUFFER, _vboIds[1]);

    CameraP camera = panel->camera();
    QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
    QMatrix4x4 objToWorld = mesh->objectToWorld();

    QGLShaderProgramP vertShader = panel->getVertexShader();

    vertShader->bind();
    vertShader->setUniformValue("objToWorld", objToWorld);
    vertShader->setUniformValue("cameraPV", cameraProjViewM);
    //vertShader->setUniformValue("overrideStrength", 0.0f);
    int colorsLocation = vertShader->uniformLocation("colors");
    QVector4D colorsArray[] = { UNSELECTED_COLOR,
                                UNSELECTED_HOVER_COLOR,
                                 SELECTED_COLOR,
                                 SELECTED_HOVER_COLOR };
    GLfloat colors[16];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (j == 0) colors[i*4+j] = colorsArray[i].x();
            if (j == 1) colors[i*4+j] = colorsArray[i].y();
            if (j == 2) colors[i*4+j] = colorsArray[i].z();
            if (j == 3) colors[i*4+j] = colorsArray[i].w();
        }
    }
    vertShader->setUniformValueArray(colorsLocation, colors, 4, 4);
    //vertShader->setUniformValueArray(colorsLocation, &UNSELECTED_HOVER_COLOR, 4);
    //vertShader->setUniformValueArray(colorsLocation, &SELECTED_COLOR, 4);
    //vertShader->setUniformValueArray(colorsLocation, &SELECTED_HOVER_COLOR, 4);


    int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = vertShader->attributeLocation("vertex");
    vertShader->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
    // Offset for texture coordinate

    offset += sizeof(QVector3D);

    int colorIndexLocation = vertShader->attributeLocation("colorIndex");
    vertShader->enableAttributeArray(colorIndexLocation);
    glVertexAttribPointer(colorIndexLocation, 1, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    offset += sizeof(float);

    int visibilityLocation = vertShader->attributeLocation("visible");
    vertShader->enableAttributeArray(visibilityLocation);
    glVertexAttribPointer(visibilityLocation, 1, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    glDrawArrays(GL_POINTS, 0, numVertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vertShader->release();
}

void MeshRenderer::loadVBOs(PanelGL* panel, MeshP mesh)
{
    CursorToolP cursorTool = SunshineUi::cursorTool();
    const int drawSettings = cursorTool->drawSettings(panel, mesh);

    // load the faces
    {
        const int numTriangles = mesh->numTriangles();
        const int numVertices = mesh->numVertices();

        int triangleCount = 0;
        GLushort indices[numTriangles*3];
        MeshVertexData vertices[numTriangles*3];
        QHashIterator<int,FaceP> i = mesh->faces();
        while (i.hasNext()) {
            i.next();
            FaceP face = i.value();
            QVector4D color;
            if (drawSettings & DrawSettings::USE_OBJECT_COLOR) {
                color = UNSELECTED_COLOR;
            } else {
                if (face->isSelected() && face != panel->_hoverFace)
                    color = SELECTED_COLOR;
                else if (face->isSelected() && face == panel->_hoverFace && mesh == panel->_hoverMesh && drawSettings & DrawSettings::HIGHLIGHT_FACES)
                    color = SELECTED_HOVER_COLOR;
                else if (face == panel->_hoverFace && mesh == panel->_hoverMesh && drawSettings & DrawSettings::HIGHLIGHT_FACES)
                    color = UNSELECTED_HOVER_COLOR;
                else
                    color = UNSELECTED_COLOR;
            }

            QListIterator<Triangle> j = face->buildTriangles();
            while (j.hasNext()) {
                Triangle triangle = j.next();
                vertices[triangleCount*3+0] = MeshVertexData(triangle.a->vert()->pos(),
                                                             color, triangle.a->normal(),
                                                             triangle.b->next() != triangle.c);
                vertices[triangleCount*3+1] = MeshVertexData(triangle.b->vert()->pos(),
                                                             color, triangle.b->normal(),
                                                             triangle.c->next() != triangle.a);
                vertices[triangleCount*3+2] = MeshVertexData(triangle.c->vert()->pos(),
                                                             color, triangle.c->normal(),
                                                             triangle.a->next() != triangle.b);
                triangleCount++;
            }
        }

        // Transfer vertex data to VBO 0
        glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, numTriangles*3*sizeof(MeshVertexData), vertices, GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // load the vertices
    {
        const int numVertices = mesh->numVertices();

        QHashIterator<int,VertexP> i = mesh->vertices();
        int vertexCount = 0;
        VertexData vertices[numVertices];
        while (i.hasNext()) {
            i.next();
            VertexP vertex = i.value();
            float colorIndex;
            if (vertex->isSelected() && vertex == panel->_hoverVert) colorIndex = 3.5;
            else if (vertex->isSelected() && vertex != panel->_hoverVert) colorIndex = 2.5;
            else if (vertex == panel->_hoverVert) colorIndex = 1.5;
            else colorIndex = 0.5;
            float visible = 1.0;
            if (drawSettings & DrawSettings::CULL_BORING_VERTICES && !(vertex->isSelected()) && vertex != panel->_hoverVert)
                visible = 0.0;
            vertices[vertexCount] = VertexData(vertex->pos(), colorIndex, visible);
            vertexCount++;
        }

        // Transfer vertex data to VBO 1
        glPointSize(5);
        glBindBuffer(GL_ARRAY_BUFFER, _vboIds[1]);
        glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(VertexData), vertices, GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void PanelGL::enterEvent(QEvent *)
{
    CursorToolP cursorTool = SunshineUi::cursorTool();
    //setCursor(Qt::ArrowCursor);
    setCursor(cursorTool->cursor());
}

void PanelGL::mousePressEvent(QMouseEvent* event)
{
    bool altDown = event->modifiers() & Qt::AltModifier;

    if (mouseMode == MouseMode::FREE && altDown) {
        mouseMode = MouseMode::CAMERA;
        activeMouseButton = event->button();
        _camera->mousePressed(event);
    }
    else if (mouseMode == MouseMode::FREE && event->button() & Qt::LeftButton) {
        mouseMode = MouseMode::TOOL;
        activeMouseButton = event->button();
        //basicSelect = BasicSelect.instance
        //basic_select.mousePressed(self,event)
        //_basicSelect->mousePressed(this, event);
        CursorToolP cursorTool = SunshineUi::cursorTool();
        cursorTool->mousePressed(this, event);
    }
}

void PanelGL::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (mouseMode == MouseMode::FREE && event->button() & Qt::LeftButton) {
        CursorToolP cursorTool = SunshineUi::cursorTool();
        cursorTool->mouseDoubleClicked(this, event);
        /*
        mouseMode = MouseMode::TOOL;
        activeMouseButton = event->button();
        //basicSelect = BasicSelect.instance
        //basic_select.mousePressed(self,event)
        //_basicSelect->mousePressed(this, event);
        CursorToolP cursorTool = SunshineUi::cursorTool();
        cursorTool->mousePressed(this, event);
        */
    }
}

void PanelGL::mouseReleaseEvent(QMouseEvent* event)
{
    CursorToolP cursorTool = SunshineUi::cursorTool();

    if (mouseMode == MouseMode::CAMERA && event->button() == activeMouseButton) {
        mouseMode = MouseMode::FREE;
        activeMouseButton = -1;
        _camera->mouseReleased(event);
    }
    else if (mouseMode == MouseMode::TOOL && event->button() == activeMouseButton) {
        mouseMode = MouseMode::FREE;
        activeMouseButton = -1;

        cursorTool->mouseReleased(this, event);
    }
    else if (mouseMode == MouseMode::FREE && event->button() == Qt::RightButton && cursorTool->canMouseCancel(this)) {
        cursorTool->cancel(this);
    }
    else if (mouseMode == MouseMode::FREE && event->button() == Qt::RightButton) { // popup menu
        showContextMenu(event);
    }

    buildMeshGrid();
    _validSelectionBuffer = FALSE;

    update();
}

void PanelGL::mouseMoveEvent(QMouseEvent* event)
{
    if (mouseMode == MouseMode::FREE) {
        // calculate preselection
        Point3 rayOrig = camera()->eye();
        Vector3 rayDir = computeRayDirection(event->pos());
        //FaceUtil::FaceHit faceHit = FaceUtil::closestFace(_scene, rayOrig, rayDir, false);

        QList<Triangle> triangles = _meshGrid.trianglesByPoint(QPoint(event->pos().x(), height()-event->pos().y()));
        FaceUtil::FaceHit faceHit = FaceUtil::closestFace(triangles, rayOrig, rayDir, false);
        VertexUtil::VertexHit vertexHit = VertexUtil::closestVertex(this, event, false);

        _hoverMesh = faceHit.nearMesh ? faceHit.nearMesh : MeshP(0);
        _hoverFace = faceHit.nearFace ? faceHit.nearFace : FaceP(0);
        _hoverVert = vertexHit.vertex ? vertexHit.vertex : VertexP(0);

        CursorToolP cursorTool = SunshineUi::cursorTool();
        cursorTool->mouseMoved(this, event);

        update();
    }
    else if (mouseMode == MouseMode::TOOL) {
        mouseDragEvent(event);
        /*
        //_workTool->mouseMoved(event);
        if (_ravagingMouse) { // move mouse back to center
            QPoint mouseDiff = QCursor::pos() - centerMouse(TRUE);
            if (mouseDiff != QPoint(0,0)) {
                _workTool->mouseMoved(event, mouseDiff.x(), mouseDiff.y());
                centerMouse(FALSE);
            }
        }
        update();
        */
    }

    else {
        mouseDragEvent(event);
    }
}

void PanelGL::mouseDragEvent(QMouseEvent* event)
{
    if (mouseMode == MouseMode::CAMERA) {
        _camera->mouseDragged(event);

    }
    else if (mouseMode == MouseMode::TOOL) {
        CursorToolP cursorTool = SunshineUi::cursorTool();
        cursorTool->mouseDragged(this, event);
        //basic_select = BasicSelect.instance
        //basic_select.mouseDrag(self, event)
        //_basicSelect->mouseDragged(this, event);
    }

    update();

}

Point3 PanelGL::project(Point3 p)
{
    int viewport[] = { 0, 0, width(), height() };

    QMatrix4x4 cameraViewM = Camera::getViewMatrix(_camera, width(), height());
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(_camera, width(), height());

    Point3 object = ProjectUtil::project(p.x(), p.y(), p.z(),
                                         cameraViewM,
                                         cameraProjM,
                                         viewport);
    return object;
}

Point3 PanelGL::unproject(Point3 p)
{
    int viewport[4];
    viewport[0] = 0;
    viewport[1] = 0;
    viewport[2] = width();
    viewport[3] = height();

    QMatrix4x4 cameraViewM = Camera::getViewMatrix(_camera, width(), height());
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(_camera, width(), height());

    Point3 object = ProjectUtil::unproject(p.x(), p.y(), p.z(),
                                           cameraViewM,
                                           cameraProjM,
                                           viewport);
    return object;
}

Vector3 PanelGL::computeRayDirection(QPoint p) {
    Point3 pNear = unproject(Point3(p.x(), height() - p.y(), 0.3));
    Point3 pFar = unproject(Point3(p.x(), height() - p.y(), 0.6));
    Vector3 rayDir = pFar - pNear;
    rayDir.normalize();
    return rayDir;
}

void PanelGL::showContextMenu(QMouseEvent *event)
{

    // get all the actions for
    ContextMenu popup;

    SunshineUi::cursorTool()->updateMenu(&popup);

    /*
    QList<ContextAction*> actions;
    foreach (WorkToolP tool, _scene->_tools) {
        if (tool->isViewable(this)) {
            foreach(ContextAction* action, tool->actions()) {
                actions << action;
            }
        }
    }

    foreach(ContextAction* action, actions) {
        action->setParent(&popup);
        connect(action, SIGNAL(triggered()), action, SLOT(itemTriggered()));
        connect(action, SIGNAL(workToolChange(WorkTool*, QString, int)),
                this, SLOT(initWorkTool(WorkTool*, QString, int)));

        popup.addAction(action);
    }
    */

    popup.exec(event->globalPos());
}

void PanelGL::initWorkTool(WorkTool* tool, QString command, int button)
{
    if (tool->init(this, command, button)) {
        if (tool->wantsMouse()) {
            mouseMode = MouseMode::TOOL;
            ravageMouse();
            _workTool = tool;
        }
    }
}

QPoint PanelGL::centerMouse(bool mock)
{
    QPoint goingTo = mapToGlobal(pos()) + QPoint(width()*.5,height()*.5);
    if (!mock)
        QCursor::setPos(goingTo);
    return goingTo;
}

void PanelGL::ravageMouse()
{
    centerMouse(FALSE);
    _ravagingMouse = TRUE;
    setBlankCursor();
}

void PanelGL::setArrowCursor()
{
    QCursor cursor;
    cursor.setShape(Qt::ArrowCursor);
    this->setCursor(cursor);
}

void PanelGL::setBlankCursor()
{
    QCursor cursor;
    cursor.setShape(Qt::BlankCursor);
    this->setCursor(cursor);
}

void PanelGL::buildMeshGrid()
{
    // calculate scene triangles
    //int totalTriangles = 0;
    QList<Triangle> triangles;
    foreach(QString meshName, _scene->meshes()) {
        MeshP mesh = _scene->mesh(meshName);

        QMatrix4x4 objToWorld = mesh->objectToWorld();
        QHashIterator<int,FaceP> i = mesh->faces();
        while (i.hasNext()) {
            i.next();
            FaceP face = i.value();
            QListIterator<Triangle> j = face->buildTriangles();
            while (j.hasNext()) {
                Triangle triangle = j.next();

                triangle.screenP[0] = project(objToWorld.map(triangle.a->vert()->pos()));
                triangle.screenP[1] = project(objToWorld.map(triangle.b->vert()->pos()));
                triangle.screenP[2] = project(objToWorld.map(triangle.c->vert()->pos()));

                triangles << triangle;
            }
        }
    }

    _meshGrid = MeshGrid(this->width(), this->height(), triangles);
}

MeshGrid::MeshGrid(int xres, int yres, QList<Triangle> triangles)
{
    xCells = (int)(ceil((xres / (float)PIXEL_SIZE)));
    yCells = (int)(ceil((yres / (float)PIXEL_SIZE)));
    for (int i = 0; i < xCells*yCells; i++)
        cells[i] = QList<Triangle>();

    /*
    int triangleCount[xCells][yCells];
    for (int i = 0; i < xCells; i++)
        for (int j = 0; j < yCells; j++)
            triangleCount[i][j] = 0;
            */

    foreach (Triangle triangle, triangles) {
        int minCellX = 10000;
        int maxCellX = -1;
        int minCellY = 10000;
        int maxCellY = -1;
        for (int a = 0; a < 3; a++) {
            int cellX = (int)(floor((triangle.screenP[a].x() / (float)PIXEL_SIZE)));
            int cellY = (int)(floor((triangle.screenP[a].y() / (float)PIXEL_SIZE)));
            if (cellX >= 0 && cellX < xCells) {
                minCellX = std::min(minCellX, cellX);
                maxCellX = std::max(maxCellX, cellX);
            }
            if (cellY >= 0 && cellY < yCells) {
                minCellY = std::min(minCellY, cellY);
                maxCellY = std::max(maxCellY, cellY);
            }
        }

        // put this triangle into its respective bins
        for (int i = minCellX; i <= maxCellX; i++) {
            for (int j = minCellY; j <= maxCellY; j++) {
                cells[i*yCells+j] << triangle;
            }
        }
    }
}

QList<Triangle> MeshGrid::trianglesByPoint(QPoint p)
{
    int cellX = (int)(floor((p.x() / (float)PIXEL_SIZE)));
    int cellY = (int)(floor((p.y() / (float)PIXEL_SIZE)));

    return cells[cellX*yCells+cellY];
}
