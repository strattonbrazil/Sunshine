#include "panelgl.h"

// possible reference
// http://code.google.com/p/opencamlib/source/browse/trunk/cpp_examples/qt_opengl_vbo/glwidget.h?spec=svn688&r=688

#include <QVarLengthArray>
#include <QMouseEvent>
#include "select.h"
#include "project_util.h"

//#include <iostream>
//using namespace std;

LineRenderer* mainGrid = NULL;
int workMode = WorkMode::FREE;
int workButton = -1;

struct VertexColorData
{
    Vector3 position;
    Vector4 color;
};

PanelGL::PanelGL() : QGLWidget(PanelGL::defaultFormat())
{
    setMouseTracking(true);
    _validShaders = false;

    _camera = Register::createCamera("dummy");

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
            std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        }
    }
}

void PanelGL::paintGL()
{   
    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    if (!_validShaders) {
        _flatShader = ShaderFactory::buildFlatShader(this);
        _meshShader = ShaderFactory::buildMeshShader(this);
        _validShaders = true;
    }

    // render the grid
    mainGrid->render(this);

    // render all the meshes
    QHashIterator<int,MeshP> meshes = Register::meshes();
    while (meshes.hasNext()) {
        meshes.next();
        int meshKey = meshes.key();
        MeshP mesh = meshes.value();

        if (!_meshRenderers.contains(meshKey)) // create the mesh renderer if it doesn't exist for this mesh
            _meshRenderers[meshKey] = MeshRendererP(new MeshRenderer(meshKey));

        _meshRenderers[meshKey]->render(this);
    }

    glDisable(GL_DEPTH_TEST);

    // render the selection box
    if (workMode == WorkMode::SELECT) {
        if (BasicSelect::selectMode() == SelectMode::BOX) {

            CameraP camera = _camera;
            QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera, width(), height());
            QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera, width(), height());
            QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
            QMatrix4x4 objToWorld;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, width(), 0, height(), -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glColor4f(1,.2f,1,.2f);
            glBegin(GL_LINE_LOOP);
            {
                glVertex2f(BasicSelect::minX,BasicSelect::minY);
                glVertex2f(BasicSelect::minX,BasicSelect::maxY);
                glVertex2f(BasicSelect::maxX,BasicSelect::maxY);
                glVertex2f(BasicSelect::maxX,BasicSelect::minY);
            }
            glEnd();
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_QUADS);
            {
                glVertex2f(BasicSelect::minX,BasicSelect::minY);
                glVertex2f(BasicSelect::minX,BasicSelect::maxY);
                glVertex2f(BasicSelect::maxX,BasicSelect::maxY);
                glVertex2f(BasicSelect::maxX,BasicSelect::minY);
            }
            glEnd();
            glDisable(GL_BLEND);
        }
    }
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

void LineRenderer::render(PanelGL* panel)
{
    if (!_validVBOs) {
        glGenBuffers(2, _vboIds);

        _validVBOs = TRUE;
    }

    loadVBOs(panel);

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
    glBufferData(GL_ARRAY_BUFFER, _segments.size()*2*sizeof(VertexColorData), vertices, GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _segments.size()*2*sizeof(GLushort), indices, GL_STATIC_DRAW);
}

class MeshVertexData
{
public:
                 MeshVertexData() {}
                 MeshVertexData(Vector3 p, Vector4 c, Vector4 n) : position(p), color(c), normal(n) {}
    Vector3      position;
    Vector4      color;
    Vector3      normal;
};

MeshRenderer::MeshRenderer(int meshKey)
{
    _validVBOs = FALSE;
    _meshKey = meshKey;
}

void MeshRenderer::render(PanelGL* panel)
{
    if (!_validVBOs) {
        glGenBuffers(2, _vboIds);

        _validVBOs = TRUE;
    }

    MeshP mesh = Register::mesh(_meshKey);
    mesh->validateNormals();
    const int numTriangles = mesh->numTriangles();

    loadVBOs(panel, mesh);

    CameraP camera = panel->camera();
    QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
    QMatrix4x4 objToWorld;

    QGLShaderProgramP meshShader = panel->getMeshShader();

    meshShader->bind();
    meshShader->setUniformValue("objToWorld", objToWorld);
    meshShader->setUniformValue("cameraPV", cameraProjViewM);
    meshShader->setUniformValue("cameraPos", camera->eye());
    meshShader->setUniformValue("lightDir", -camera->lookDir().normalized());
    meshShader->setUniformValue("singleColor", Vector4(1,1,0,1));
    meshShader->setUniformValue("isSingleColor", 0.0f);

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

    // tell OpenGL programmable pipeline how to locate vertex color data
    int normalLocation = meshShader->attributeLocation("normal");
    meshShader->enableAttributeArray(normalLocation);
    glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertexData), (const void *)offset);

    glDrawElements(GL_TRIANGLES, numTriangles*3, GL_UNSIGNED_SHORT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    meshShader->release();
}

void MeshRenderer::loadVBOs(PanelGL* panel, MeshP mesh)
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
        QListIterator<Triangle> j = face->buildTriangles();
        while (j.hasNext()) {
            Triangle triangle = j.next();
            vertices[triangleCount*3+0] = MeshVertexData(triangle.a->vert()->pos(), QVector4D(.8,.8,.8,1), triangle.a->normal());
            vertices[triangleCount*3+1] = MeshVertexData(triangle.b->vert()->pos(), QVector4D(.8,.8,.8,1), triangle.b->normal());
            vertices[triangleCount*3+2] = MeshVertexData(triangle.c->vert()->pos(), QVector4D(.8,.8,.8,1), triangle.c->normal());

            indices[triangleCount*3+0] = triangleCount*3+0;
            indices[triangleCount*3+1] = triangleCount*3+1;
            indices[triangleCount*3+2] = triangleCount*3+2;
            triangleCount++;
        }
    }

    // Transfer vertex data to VBO 0
    glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, numTriangles*3*sizeof(MeshVertexData), vertices, GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numTriangles*3*sizeof(GLushort), indices, GL_STATIC_DRAW);
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
        _camera->mousePressed(event);
    }
    else if (workMode == WorkMode::FREE && event->button() & Qt::LeftButton) {
        workMode = WorkMode::SELECT;
        workButton = event->button();
        //basicSelect = BasicSelect.instance
        //basic_select.mousePressed(self,event)
        BasicSelect::mousePressed(this, event);
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
        _camera->mouseReleased(event);
    }
    else if (workMode == WorkMode::SELECT && event->button() == workButton) {
        workMode = WorkMode::FREE;
        workButton = -1;
        BasicSelect::mouseReleased(this, event);

    }
    else if (workMode == WorkMode::FREE && event->button() == Qt::RightButton) { // popup menu
        std::cout << "Popup menu" << std::endl;
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
        _camera->mouseDragged(event);

    }
    else if (workMode == WorkMode::SELECT) {
        //basic_select = BasicSelect.instance
        //basic_select.mouseDrag(self, event)
        BasicSelect::mouseDragged(this, event);
    }

    update();

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
