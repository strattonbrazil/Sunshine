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
#include <QTime>
#include <QLayout>
#include "cursor_tools.h"

namespace MouseMode {
    enum { FREE, CAMERA, TOOL };
}

LineRenderer* mainGrid = 0;
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

PanelGL::PanelGL(Scene* scene, Sunshine* sunshine) : QGLWidget(PanelGL::defaultFormat())
{
    _scene = scene;
    _camera = _scene->camera("persp");
    _sunshine = sunshine;

    init();
}

void PanelGL::init()
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    _validShaders = false;
    _validSelectionBuffer = FALSE;
    //_fbo = 0, _beautyTexture = 0, _indexTexture = 0, _depthTexture = 0,
    _pointLightTexture = 0, _spotLightTexture = 0;

    //_basicSelect = BasicSelect*(new BasicSelect());

    if (mainGrid == 0) {
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

    _hoverMesh = 0;
    _hoverFace.invalidate();
    _hoverVert.invalidate();
    _workTool = 0;
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
        else {
            //std::cout << "Glew initialized" << std::endl;

        }
    }
}

void PanelGL::paintGL()
{   
    QPainter painter;
    painter.begin(this);
    painter.beginNativePainting();

    if (!_validShaders) {
        _flatShader = ShaderFactory::buildFlatShader(this);
        _meshShader = ShaderFactory::buildMeshShader(this);
        _vertexShader = ShaderFactory::buildVertexShader(this);
        _textureShader = ShaderFactory::buildShader(this, ":/glsl/texture_shader.vert", ":/glsl/texture_shader.frag");
        _validShaders = true;
    }

    //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);

    _fbo->bind();

    glViewport( 0, 0, width(), height());
    //glClearColor(1,0,0,1);
    //glClear(GL_COLOR_BUFFER_BIT);

    renderAssets();
    _fbo->release();


    //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,1,0,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, _beautyTexture);
    glBindTexture(GL_TEXTURE_2D, _fbo->texture());
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
    /*
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
    */
    //std::cout << "(" << (int)r << "," << (int)g << "," << (int)b << "," << (int)a << ")" << std::endl;

    //renderHUD();

    painter.endNativePainting();
    renderHUD(painter);
    painter.end();
}

void PanelGL::renderAssets()
{
    GLuint selectionCounter = 1;
    _selectionAssets.clear();
    CursorTool* cursorTool = SunshineUi::cursorTool();

    GLenum bufs[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
    glDrawBuffers(2, bufs);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


#if 1
    glDrawBuffers(1, bufs);

    paintBackground();

    glEnable(GL_DEPTH_TEST);

    cursorTool->preDrawOverlay(this);

    // render the grid
    mainGrid->render(this);

    glDrawBuffers(2, bufs);

    // render all the meshes
    foreach(QString meshName, _scene->meshes()) {
        Mesh* mesh = _scene->mesh(meshName);

        if (!_meshRenderers.contains(meshName)) // create the mesh renderer if it doesn't exist for this mesh
            _meshRenderers[meshName] = MeshRendererP(new MeshRenderer(meshName));

        _meshRenderers[meshName]->render(this, selectionCounter++);
    }

    glDisable(GL_DEPTH_TEST);

    glDrawBuffers(1, bufs);

    if (_workTool == 0)
        cursorTool->postDrawOverlay(this);
    else
        _workTool->postDrawOverlay(this);

    renderLights(selectionCounter);
#endif


glDrawBuffer(GL_BACK);
}

void PanelGL::renderLights(GLuint &selectionCounter)
{
    if (_pointLightTexture == 0)
        _pointLightTexture = bindTexture(QImage(":/icons/point_light_icon.png"));
    if (_spotLightTexture == 0)
        _spotLightTexture = bindTexture(QImage(":/icons/spot_light_icon.png"));



    const float ICON_OFFSET = 12;

    glEnable(GL_DEPTH_TEST);

    QGLShaderProgram* textureShader = _textureShader;
    //QMatrix4x4 ortho;
    //ortho.ortho(0, width(), 0, height(), -1, 0);
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(_camera, width(), height());
    QMatrix4x4 cameraViewM = Camera::getViewMatrix(_camera, width(), height());
    QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
    textureShader->bind();
    textureShader->setUniformValue("objToWorld", QMatrix4x4());
    textureShader->setUniformValue("cameraPV", cameraProjViewM);
    textureShader->setUniformValue("colorMap", 0);

    GLfloat fSizes[2];
    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, fSizes);
    //std::cout << fSizes[0] << " " << fSizes[1] << std::endl;

    foreach(QString lightName, _scene->lights()) {
        Light* light = _scene->light(lightName);
        if (light->lightType() == LightType::POINT_LIGHT) {
            /*
            glBindTexture(GL_TEXTURE_2D, _pointLightTexture);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_TEXTURE_2D);
            glColor4f(1,1,1,1);

            //Point3 screen = project(light->center());
            Vector3 left = _camera->leftDir();
            Vector3 up = _camera->upDir();

            Vector3 a = left - up + light->center();
            Vector3 b = -left - up + light->center();
            Vector3 c = -left + up + light->center();
            Vector3 d = left + up + light->center();

            glBegin(GL_QUADS);
            {
                glTexCoord2f(0, 0);
                glVertex3f(a.x(), a.y(), a.z());
                glTexCoord2f(1, 0);
                glVertex3f(b.x(), b.y(), b.z());
                glTexCoord2f(1, 1);
                glVertex3f(c.x(), c.y(), c.z());
                glTexCoord2f(0, 1);
                glVertex3f(d.x(), d.y(), d.z());
            }
            glEnd();


            glDisable(GL_TEXTURE_2D);

            glDisable(GL_BLEND);
            */
        }
        else if (light->lightType() == LightType::SPOT_LIGHT) {
            glBindTexture(GL_TEXTURE_2D, _spotLightTexture);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_TEXTURE_2D);
            glColor4f(1,1,1,1);

            //Point3 screen = project(light->center());
            Vector3 left = 0.5f * _camera->leftDir();
            Vector3 up = 0.5f * _camera->upDir();

            Vector3 a = left - up + light->center();
            Vector3 b = -left - up + light->center();
            Vector3 c = -left + up + light->center();
            Vector3 d = left + up + light->center();

            glBegin(GL_QUADS);
            {
                glTexCoord2f(0, 0);
                glVertex3f(a.x(), a.y(), a.z());
                glTexCoord2f(1, 0);
                glVertex3f(b.x(), b.y(), b.z());
                glTexCoord2f(1, 1);
                glVertex3f(c.x(), c.y(), c.z());
                glTexCoord2f(0, 1);
                glVertex3f(d.x(), d.y(), d.z());
            }
            glEnd();


            glDisable(GL_TEXTURE_2D);

            glDisable(GL_BLEND);


            textureShader->release();

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glLoadMatrixd(cameraProjM.data());
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glLoadMatrixd(cameraViewM.data());

            QVector3D lookDir = light->lookDir().normalized();
            glColor4f(.6,.6,.8,1);
            glLineWidth(2.0f);
            float coneAngle = light->attributeByName("Cone Angle")->property("value").value<float>();
            glBegin(GL_LINES);
            {
                Point3 lightStart = light->center() + lookDir;
                Point3 lightEnd = light->center() + lookDir + lookDir*10.0f;

                Vector3 n = (lightEnd - lightStart).normalized();
                Vector3 u = Vector3::crossProduct(light->upDir(), n);
                u.normalize();
                Vector3 v = Vector3::crossProduct(u, n);
                v.normalize();

                // draw cone angles
                float radii[] = { tan(coneAngle * PI / 180.0f),
                                  tan(coneAngle * PI / 180.0f) * ((lightEnd-lightStart).length()+1) };
                Point3 circleCenters[] = { lightStart, lightEnd };

                const int CONNECTORS = 3;
                for (int i = 0; i < CONNECTORS; i++) {
                    float theta = 2 * PI * i / CONNECTORS;
                    Point3 l1 = (radii[0] * cos(theta) * u) + (radii[0] * sin(theta) * v) + circleCenters[0];
                    Point3 l2 = (radii[1] * cos(theta) * u) + (radii[1] * sin(theta) * v) + circleCenters[1];

                    glVertex3f(l1.x(), l1.y(), l1.z());
                    glVertex3f(l2.x(), l2.y(), l2.z());

                }



                //glVertex3f(lightStart.x(), lightStart.y(), lightStart.z());
                //glVertex3f(lightEnd.x(), lightEnd.y(), lightEnd.z());


                for (int i = 0; i < 2; i++)
                {
                    float radius = radii[i];
                    const int SEGMENTS = 16;

                    Point3 circleCenter = circleCenters[i];

                    for (int j = 0; j < SEGMENTS; j++)
                    {
                        float t1 = 2 * PI * ((float)j / SEGMENTS);
                        float t2 = 2 * PI * ((float)(j+1) / SEGMENTS);
                        Point3 p1 = (radius * cos(t1) * u) + (radius * sin(t1) * v) + circleCenter;
                        Point3 p2 = radius * cos(t2) * u + radius * sin(t2) * v + circleCenter;

                        glVertex3f(p1.x(), p1.y(), p1.z());
                        glVertex3f(p2.x(), p2.y(), p2.z());
                    }
                }
            }
            glEnd();




        }
    }

    textureShader->release();

    glDisable(GL_DEPTH_TEST);
}

void PanelGL::renderHUD(QPainter &painter)
{
    painter.setPen(QColor(135,37,255));

    // camera name
    {
        QString assetName = _scene->assetName(_camera);

        int nameWidth = painter.fontMetrics().width(assetName) * 0.5;
        int nameHeight = painter.fontMetrics().height();
        int descent = painter.fontMetrics().descent();

        painter.drawText(width()*.5-nameWidth, height()-descent-4, assetName);
    }

    // work tool or mode
    {
        QString toolName;
        if (_workTool != 0)
            toolName = _workTool->displayName();
        if (toolName != "") {
            int nameWidth = painter.fontMetrics().width(toolName);
            int nameHeight = painter.fontMetrics().height();
            int descent = painter.fontMetrics().descent();

            painter.drawText(width()-nameWidth - 4, height()-descent-4, toolName);
        }
    }

}

void PanelGL::paintBackground()
{
    Transformable* camera = _camera;
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

//    std::cout << width << " " << height << std::endl;

    if (width && height)
        initFBO(width, height);

    if (_fbo == 0 || _fbo->width() != width || _fbo->height() != height)
        _fbo = QSharedPointer<QGLFramebufferObject>(new QGLFramebufferObject(width, height, QGLFramebufferObject::CombinedDepthStencil));
}

void PanelGL::initFBO(int width, int height)
{
#if 0
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
#endif
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

    Transformable* camera = panel->camera();
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

void MeshRenderer::render(PanelGL* panel, GLuint selectionIndex)
{
    if (!_validVBOs) {
        glGenBuffers(2, _vboIds);

        _validVBOs = TRUE;
    }
    Mesh* mesh = panel->scene()->mesh(_meshName);
    CursorTool* cursorTool = SunshineUi::cursorTool();
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

    Mesh* mesh = panel->scene()->mesh(_meshName);
    mesh->validateNormals();
    const int numTriangles = mesh->numTriangles();

    Transformable* camera = panel->camera();
    QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
    QMatrix4x4 objToWorld = mesh->objectToWorld();
    QMatrix4x4 normalToWorld = mesh->normalToWorld();

    QGLShaderProgram* meshShader = panel->getMeshShader();

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

    CursorTool* cursorTool = SunshineUi::cursorTool();
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
        QVector4D singleColor = UNSELECTED_COLOR;
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
    Mesh* mesh = panel->scene()->mesh(_meshName);
    const int numVertices = mesh->numVertices();

    glBindBuffer(GL_ARRAY_BUFFER, _vboIds[1]);

    Transformable* camera = panel->camera();
    QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
    QMatrix4x4 objToWorld = mesh->objectToWorld();

    QGLShaderProgram* vertShader = panel->getVertexShader();

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

void MeshRenderer::loadVBOs(PanelGL* panel, Mesh* mesh)
{
    CursorTool* cursorTool = SunshineUi::cursorTool();
    const int drawSettings = cursorTool->drawSettings(panel, mesh);

    // load the faces
    {
        const int numTriangles = mesh->numTriangles();
        const int numVertices = mesh->numVertices();

        int triangleCount = 0;
        QVector<GLushort> indices(numTriangles*3);
        //GLushort indices[numTriangles*3];
        QVector<MeshVertexData> vertices(numTriangles*3);// vertices.resize();
        //MeshVertexData vertices[numTriangles*3];
        for (SunshineMesh::FaceIter f_it = mesh->_mesh->faces_begin(); f_it != mesh->_mesh->faces_end(); ++f_it) {
            OpenMesh::FaceHandle face = f_it.handle();
            QVector4D color;
            if (drawSettings & DrawSettings::USE_OBJECT_COLOR) {
                color = UNSELECTED_COLOR;
            } else {
                if (mesh->isSelected(face) && face != panel->_hoverFace)
                    color = SELECTED_COLOR;
                else if (mesh->isSelected(face) && face == panel->_hoverFace && mesh == panel->_hoverMesh && drawSettings & DrawSettings::HIGHLIGHT_FACES)
                    color = SELECTED_HOVER_COLOR;
                else if (face == panel->_hoverFace && mesh == panel->_hoverMesh && drawSettings & DrawSettings::HIGHLIGHT_FACES)
                    color = UNSELECTED_HOVER_COLOR;
                else
                    color = UNSELECTED_COLOR;
            }

            QListIterator<Triangle> j = buildTriangles(mesh, face);
            while (j.hasNext()) {
                Triangle triangle = j.next();
                OpenMesh::Vec3f p0 = mesh->_mesh->point(mesh->_mesh->from_vertex_handle(triangle.a));
                OpenMesh::Vec3f p1 = mesh->_mesh->point(mesh->_mesh->from_vertex_handle(triangle.b));
                OpenMesh::Vec3f p2 = mesh->_mesh->point(mesh->_mesh->from_vertex_handle(triangle.c));
                vertices[triangleCount*3+0] = MeshVertexData(Vector3(p0[0], p0[1], p0[2]),
                                                             color, mesh->normal(triangle.a),
                                                             mesh->_mesh->next_halfedge_handle(triangle.b) != triangle.c);
                vertices[triangleCount*3+1] = MeshVertexData(Vector3(p1[0], p1[1], p1[2]),
                                                             color, mesh->normal(triangle.b),
                                                             mesh->_mesh->next_halfedge_handle(triangle.c) != triangle.a);
                vertices[triangleCount*3+2] = MeshVertexData(Vector3(p2[0], p2[1], p2[2]),
                                                             color, mesh->normal(triangle.c),
                                                             mesh->_mesh->next_halfedge_handle(triangle.a) != triangle.b);
                triangleCount++;
            }
        }

        // Transfer vertex data to VBO 0
        glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, numTriangles*3*sizeof(MeshVertexData), vertices.data(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // load the vertices
    {
        const int numVertices = mesh->numVertices();
        VertexData vertices[numVertices];
        int vertexCount = 0;
        for (SunshineMesh::VertexIter v_it = mesh->_mesh->vertices_begin(); v_it != mesh->_mesh->vertices_end(); ++v_it) {
            OpenMesh::VertexHandle vertex = v_it.handle();
            float colorIndex;
            if (mesh->isSelected(vertex) && vertex == panel->_hoverVert) colorIndex = 3.5;
            else if (mesh->isSelected(vertex) && vertex != panel->_hoverVert) colorIndex = 2.5;
            else if (vertex == panel->_hoverVert) colorIndex = 1.5;
            else colorIndex = 0.5;
            float visible = 1.0;
            if (drawSettings & DrawSettings::CULL_BORING_VERTICES && !(mesh->isSelected(vertex)) && vertex != panel->_hoverVert)
                visible = 0.0;
            OpenMesh::Vec3f p = mesh->_mesh->point(vertex);
            vertices[vertexCount] = VertexData(Point3(p[0], p[1], p[2]), colorIndex, visible);
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
    CursorTool* cursorTool = SunshineUi::cursorTool();
    //setCursor(Qt::ArrowCursor);
    setCursor(cursorTool->cursor());
}

void PanelGL::mousePressEvent(QMouseEvent* event)
{
    bool altDown = event->modifiers() & Qt::AltModifier;

    if (mouseMode == MouseMode::FREE && altDown) {
        mouseMode = MouseMode::CAMERA;
        activeMouseButton = event->button();
        Camera::mousePressed(_camera, _cameraScratch, event);
    }
    else if (mouseMode == MouseMode::FREE && event->button() & Qt::LeftButton) {
        if (_workTool != 0) {
            mouseMode = MouseMode::TOOL;
            activeMouseButton = event->button();
            _workTool->mousePressed(this, event);
        }
        else {
            mouseMode = MouseMode::TOOL;
            activeMouseButton = event->button();
            CursorTool* cursorTool = SunshineUi::cursorTool();
            cursorTool->mousePressed(this, event);
        }

    }
}

void PanelGL::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (mouseMode == MouseMode::FREE && event->button() & Qt::LeftButton) {
        CursorTool* cursorTool = SunshineUi::cursorTool();
        cursorTool->mouseDoubleClicked(this, event);
        /*
        mouseMode = MouseMode::TOOL;
        activeMouseButton = event->button();
        //basicSelect = BasicSelect.instance
        //basic_select.mousePressed(self,event)
        //_basicSelect->mousePressed(this, event);
        CursorTool* cursorTool = SunshineUi::cursorTool();
        cursorTool->mousePressed(this, event);
        */
    }
}

void PanelGL::mouseReleaseEvent(QMouseEvent* event)
{
    CursorTool* cursorTool = SunshineUi::cursorTool();

    if (mouseMode == MouseMode::CAMERA && event->button() == activeMouseButton) {
        mouseMode = MouseMode::FREE;
        activeMouseButton = -1;
        Camera::mouseReleased(_camera, _cameraScratch, event);
    }
    else if (mouseMode == MouseMode::TOOL && _workTool == 0 && event->button() == activeMouseButton) {
        mouseMode = MouseMode::FREE;
        activeMouseButton = -1;

        cursorTool->mouseReleased(this, event);
    }
    else if (_workTool != 0) {
        if (_workTool->ravageMouse()) {
            if (_workTool != 0 && event->button() & Qt::LeftButton) {
                _workTool->finish(event);
                _workTool = 0;
            }
            else if (_workTool != 0 && event->button() & Qt::RightButton) {
                _workTool->cancel(event);
                _workTool = 0;
            }
        } else {
            _workTool->mouseReleased(this, event);
        }
        mouseMode = MouseMode::FREE;
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

    if (_workTool != 0 || mouseMode == MouseMode::TOOL) {
        mouseDragEvent(event);

        //_workTool->mouseMoved(event);


        update();
    }
    else if (mouseMode == MouseMode::FREE) {
        // calculate preselection
        Point3 rayOrig = camera()->eye();
        Vector3 rayDir = computeRayDirection(event->pos());
        //FaceUtil::FaceHit faceHit = FaceUtil::closestFace(_scene, rayOrig, rayDir, false);

        QList<Triangle> triangles = _meshGrid.trianglesByPoint(QPoint(event->pos().x(), height()-event->pos().y()));
        FaceUtil::FaceHit faceHit = FaceUtil::closestFace(triangles, rayOrig, rayDir, false);
        VertexUtil::VertexHit vertexHit = VertexUtil::closestVertex(this, event, false);

        _hoverMesh = faceHit.nearMesh ? faceHit.nearMesh : 0;
        if (faceHit.nearFace.is_valid()) _hoverFace = faceHit.nearFace;
        else _hoverFace.invalidate();

        if (vertexHit.vertex.is_valid()) _hoverVert = vertexHit.vertex;
        else _hoverVert.invalidate();

        CursorTool* cursorTool = SunshineUi::cursorTool();
        cursorTool->mouseMoved(this, event);

        update();
    }
    else {
        mouseDragEvent(event);
    }
}

void PanelGL::mouseDragEvent(QMouseEvent* event)
{
    if (mouseMode == MouseMode::CAMERA) {
        Camera::mouseDragged(_camera, _cameraScratch, event);

    }
    else if (mouseMode == MouseMode::TOOL || _workTool != 0) {
        if (_workTool != 0) {
            // move mouse back to center
                QPoint mouseDiff = QCursor::pos() - centerMouse(TRUE);
                if (mouseDiff != QPoint(0,0)) {
                    _workTool->mouseMoved(this, event, mouseDiff.x(), mouseDiff.y());
                    if (_workTool->ravageMouse())
                        centerMouse(FALSE);
                }

         //   _workTool->mouseMoved(event, 2, 0);
        }
        else {
            CursorTool* cursorTool = SunshineUi::cursorTool();
            cursorTool->mouseDragged(this, event);
        }
    }

    update();

}

void PanelGL::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F)
        frameSelection();
    else if (event->key() == Qt::Key_Escape) {
        if (_workTool != 0) {
            _workTool->cancel(0);
            _workTool = 0;
            if (mouseMode == MouseMode::FREE)
                std::cout << "free" << std::endl;
        }
        else if (SunshineUi::workMode() == WorkMode::OBJECT) {
            foreach(QString meshName, scene()->meshes()) {
                this->scene()->mesh(meshName)->setSelected(false);
            }
        }
        else if (SunshineUi::workMode() == WorkMode::VERTEX) {
            foreach(QString meshName, scene()->meshes()) {
                Mesh* mesh = scene()->mesh(meshName);
                for (SunshineMesh::VertexIter v_it = mesh->_mesh->vertices_begin(); v_it != mesh->_mesh->vertices_end(); ++v_it) {
                    OpenMesh::VertexHandle vertex = v_it.handle();
                    mesh->setSelected(vertex, false);
                }
            }
        }
        else if (SunshineUi::workMode() == WorkMode::FACE) {
            foreach(QString meshName, scene()->meshes()) {
                Mesh* mesh = scene()->mesh(meshName);
                for (SunshineMesh::FaceIter f_it = mesh->_mesh->faces_begin(); f_it != mesh->_mesh->faces_end(); ++f_it) {
                    OpenMesh::FaceHandle face = f_it.handle();
                    mesh->setSelected(face, false);
                }
            }
        }
    }
    else
        QGLWidget::keyReleaseEvent(event);

    SunshineUi::updatePanels();
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

    bool hasMeshSelected = false;
    foreach(QString meshName, _scene->meshes()) {
        hasMeshSelected = hasMeshSelected || _scene->mesh(meshName)->isSelected();
    }

    bool hasLightSelected = false;
    foreach(QString lightName, _scene->lights()) {
        hasLightSelected = hasLightSelected || _scene->light(lightName)->isSelected();
    }

    // add menu options specific to the selected cursor tool
    //
    SunshineUi::cursorTool()->updateMenu(&popup);



    // add menu tools specific to the current environment
    //
    QList<ContextAction*> actions;
    foreach (WorkTool* tool, _scene->_tools) {
        if (tool->isViewable(this)) {
            foreach(ContextAction* action, tool->actions()) {
                actions << action;
            }
        }
    }

    popup.addSeparator();

    foreach(ContextAction* action, actions) {
        action->setParent(&popup);
        connect(action, SIGNAL(triggered()), action, SLOT(itemTriggered()));
        connect(action, SIGNAL(workToolChange(WorkTool*, QString, int)),
                this, SLOT(initWorkTool(WorkTool*, QString, int)));

        popup.addAction(action);
    }

    popup.addSeparator();


    QMenu* lookThrough = popup.addMenu("Look through");
    if (hasLightSelected) {
        lookThrough->addAction("Selected");
        lookThrough->addSeparator();
    }
    foreach(QString cameraName, _scene->cameras()) {
        lookThrough->addAction(cameraName);
    }


    // add general UI actions
    //
    if (hasMeshSelected) {
        popup.addSeparator();
        popup.addAction("Material Attributes");
        popup.addSeparator();
        QMenu* newMat = popup.addMenu("Assign New Material");
        /*
        foreach(QString materialType, Material::materialTypes()) {
            QAction* action = newMat->addAction(materialType);
            action->setToolTip("Assign New Material: ");
        }
        */

        QMenu* existing = popup.addMenu("Assign Existing Material");
        foreach(QString materialName, _scene->materials()) {
            QAction* action = existing->addAction(materialName);
            action->setToolTip("Assign Existing Material: ");
        }
    }

    // process any actions not handled by their own trigger event
    //
    QAction* action = popup.exec(event->globalPos());
    if (action != 0) {
        if (action->parent() == lookThrough) {
            if (action->text() == "Selected") {
                foreach(QString lightName, _scene->lights()) {
                    Light* light = _scene->light(lightName);
                    if (light->isSelected())
                        _camera = light;
                }
            } else {
                _camera = _scene->camera(action->text());
            }
            update();
        }
        else if (action->text() == "Material Attributes") {
            foreach(QString meshName, _scene->meshes()) {
                Mesh* mesh = _scene->mesh(meshName);
                if (mesh->isSelected()) {
                    SunshineUi::showBindableAttributes(mesh->material());
                    break;
                }
            }
        }
        else if (action->toolTip().startsWith("Assign New Material:")) {
            Material* material = Material::buildByType(action->text());
            _scene->addAsset(action->text().toLower(), material);
            foreach(QString meshName, _scene->meshes()) {
                if (_scene->mesh(meshName)->isSelected())
                    _scene->mesh(meshName)->setMaterial(material);
            }
            SunshineUi::showBindableAttributes(material);
        }
        else if (action->toolTip().startsWith("Assign Existing Material:")) {
            Material* material = _scene->material(action->text());
            foreach(QString meshName, _scene->meshes()) {
                if (_scene->mesh(meshName)->isSelected())
                    _scene->mesh(meshName)->setMaterial(material);
            }
        }
    }
}

void PanelGL::initWorkTool(WorkTool* tool, QString command, int button)
{
    if (tool->init(this, command, button)) {
        if (tool->wantsMouse()) {
            //mouseMode = MouseMode::TOOL;
            if (tool->ravageMouse())
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
        Mesh* mesh = _scene->mesh(meshName);

        QMatrix4x4 objToWorld = mesh->objectToWorld();
        for (SunshineMesh::FaceIter f_it = mesh->_mesh->faces_begin(); f_it != mesh->_mesh->faces_end(); ++f_it) {
            OpenMesh::FaceHandle face = f_it.handle();
            QListIterator<Triangle> j = buildTriangles(mesh, face);
            while (j.hasNext()) {
                Triangle triangle = j.next();
                OpenMesh::Vec3f p0 = triangle.mesh->_mesh->point(triangle.mesh->_mesh->from_vertex_handle(triangle.a));
                OpenMesh::Vec3f p1 = triangle.mesh->_mesh->point(triangle.mesh->_mesh->from_vertex_handle(triangle.b));
                OpenMesh::Vec3f p2 = triangle.mesh->_mesh->point(triangle.mesh->_mesh->from_vertex_handle(triangle.c));
                triangle.screenP[0] = project(objToWorld.map(Vector3(p0[0], p0[1], p0[2])));
                triangle.screenP[1] = project(objToWorld.map(Vector3(p1[0], p1[1], p1[2])));
                triangle.screenP[2] = project(objToWorld.map(Vector3(p2[0], p2[1], p2[2])));

                triangles << triangle;
            }
        }
    }

    _meshGrid = MeshGrid(this->width(), this->height(), triangles);
}

void PanelGL::frameSelection()
{
    Box3D globalBounds;
    Box3D selectedBounds;

    foreach(QString meshName, _scene->meshes()) { // see if anything is selected
        Mesh* mesh = _scene->mesh(meshName);
        Box3D meshBounds = mesh->worldBounds();
        globalBounds.includeBox(meshBounds);
        if (mesh->isSelected())
            selectedBounds.includeBox(meshBounds);
    }

    Box3D dstBounds = globalBounds;
    if (selectedBounds.isValid())
        dstBounds = selectedBounds;

    if (dstBounds.isValid()) {
        // calculate new distance
        float radius = dstBounds.maxRadius();
        float dstDistance = radius / tan(_camera->fov() / 180.0f);
        float startDistance = _camera->distance();
        float distanceDiff = dstDistance - startDistance;

        const int ANIM_LENGTH = 100; // in milliseconds
        Point3 startEye = _camera->eye();

        Point3 dstEye = dstBounds.midpoint() + -1*_camera->lookDir().normalized()*dstDistance;
        Vector3 offset = dstEye - startEye;
        QTime timer;
        timer.start();
        while (timer.elapsed() < ANIM_LENGTH) {
            float t = timer.elapsed() / (float)ANIM_LENGTH;
            _camera->setCenter(startEye + offset*t);
            _camera->setDistance(startDistance + distanceDiff*t);
            //std::cout << t << std::endl;
            repaint();
        }
        _camera->setCenter(dstEye);
        _camera->setDistance(dstDistance);
    }
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
