#include "panelgl.h"

#include <QVarLengthArray>
#include <QMouseEvent>
#include <QMenu>
#include "select.h"
#include "project_util.h"
#include "contextmenu.h"
#include "face_util.h"
#include "vertex_util.h"

namespace MouseMode {
    enum { FREE, CAMERA, TOOL, SELECT };
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
    _ravagingMouse = FALSE;
    _basicSelect = BasicSelectP(new BasicSelect());

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

    paintBackground();

    glEnable(GL_DEPTH_TEST);

    if (!_validShaders) {
        _flatShader = ShaderFactory::buildFlatShader(this);
        _meshShader = ShaderFactory::buildMeshShader(this);
        _vertexShader = ShaderFactory::buildVertexShader(this);
        _validShaders = true;
    }

    // render the grid
    mainGrid->render(this);

    // render all the meshes
    QHashIterator<int,MeshP> meshes = _scene->meshes();
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
    if (mouseMode == MouseMode::SELECT) {
        if (_basicSelect->selectMode() == SelectMode::BOX) {

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
                glVertex2f(_basicSelect->minX,_basicSelect->minY);
                glVertex2f(_basicSelect->minX,_basicSelect->maxY);
                glVertex2f(_basicSelect->maxX,_basicSelect->maxY);
                glVertex2f(_basicSelect->maxX,_basicSelect->minY);
            }
            glEnd();
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_QUADS);
            {
                glVertex2f(_basicSelect->minX,_basicSelect->minY);
                glVertex2f(_basicSelect->minX,_basicSelect->maxY);
                glVertex2f(_basicSelect->maxX,_basicSelect->maxY);
                glVertex2f(_basicSelect->maxX,_basicSelect->minY);
            }
            glEnd();
            glDisable(GL_BLEND);
        }
    }
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
    glBufferData(GL_ARRAY_BUFFER, _segments.size()*2*sizeof(VertexColorData), vertices, GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _segments.size()*2*sizeof(GLushort), indices, GL_STATIC_DRAW);
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
    float        excludeEdge;
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
    MeshP mesh = panel->scene()->mesh(_meshKey);
    loadVBOs(panel, mesh);

    renderFaces(panel);
    renderVertices(panel);
}

void MeshRenderer::renderFaces(PanelGL *panel)
{
    glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);

    MeshP mesh = panel->scene()->mesh(_meshKey);
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

    if (SunshineUi::workMode() == WorkMode::LAYOUT) {
        QVector4D singleColor;
        if (mesh->isSelected() && mesh->key() != panel->_hoverMeshKey)
            singleColor = SELECTED_COLOR;
        else if (mesh->isSelected() && mesh->key() == panel->_hoverMeshKey)
            singleColor = SELECTED_HOVER_COLOR;
        else if (mesh->key() == panel->_hoverMeshKey)
            singleColor = UNSELECTED_HOVER_COLOR;
        else
            singleColor = UNSELECTED_COLOR;

        meshShader->setUniformValue("singleColor", singleColor.x(),
                                                   singleColor.y(),
                                                   singleColor.z(),
                                                   singleColor.w());
        meshShader->setUniformValue("isSingleColor", 1.0f);
    } else {
        meshShader->setUniformValue("singleColor", 0,0,0,0);
        meshShader->setUniformValue("isSingleColor", 0.0f);
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

    glDrawElements(GL_TRIANGLES, numTriangles*3, GL_UNSIGNED_SHORT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    meshShader->release();
}

void MeshRenderer::renderVertices(PanelGL *panel)
{
    MeshP mesh = panel->scene()->mesh(_meshKey);
    const int numTriangles = mesh->numTriangles();

    glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);

    CameraP camera = panel->camera();
    QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera,panel->width(),panel->height());
    QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;
    QMatrix4x4 objToWorld = mesh->objectToWorld();

    QGLShaderProgramP vertShader = panel->getVertexShader();

    vertShader->bind();
    vertShader->setUniformValue("objToWorld", objToWorld);
    vertShader->setUniformValue("cameraPV", cameraProjViewM);
    vertShader->setUniformValue("overrideStrength", 0.0f);

    int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = vertShader->attributeLocation("vertex");
    vertShader->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertexData), (const void *)offset);
    // Offset for texture coordinate

    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int colorLocation = vertShader->attributeLocation("color");
    vertShader->enableAttributeArray(colorLocation);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVertexData), (const void *)offset);

    glDrawElements(GL_POINTS, numTriangles, GL_UNSIGNED_SHORT, 0);
/*
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
*/
    vertShader->release();
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
        QVector4D color;
        if (SunshineUi::workMode() == WorkMode::MODEL) {
            if (face->isSelected() && face->key() != panel->_hoverFaceKey)
                color = SELECTED_COLOR;
            else if (face->isSelected() && face->key() == panel->_hoverFaceKey && mesh->key() == panel->_hoverMeshKey)
                color = SELECTED_HOVER_COLOR;
            else if (face->key() == panel->_hoverFaceKey and mesh->key() == panel->_hoverMeshKey)
                color = UNSELECTED_HOVER_COLOR;
            else
                color = UNSELECTED_COLOR;
        } else {
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

    if (mouseMode == MouseMode::TOOL) {
        //workTool.mousePressed(event);
    }
    else if (mouseMode == MouseMode::FREE && altDown) {
        mouseMode = MouseMode::CAMERA;
        activeMouseButton = event->button();
        _camera->mousePressed(event);
    }
    else if (mouseMode == MouseMode::FREE && event->button() & Qt::LeftButton) {
        mouseMode = MouseMode::SELECT;
        activeMouseButton = event->button();
        //basicSelect = BasicSelect.instance
        //basic_select.mousePressed(self,event)
        _basicSelect->mousePressed(this, event);
    }
}

void PanelGL::mouseReleaseEvent(QMouseEvent* event)
{
    if (mouseMode == MouseMode::TOOL) {
        if (event->button() == Qt::RightButton)
            _workTool->cancel(event);
        else
            _workTool->finish(event);
        mouseMode = MouseMode::FREE;
        _ravagingMouse = FALSE;
        setArrowCursor();
    }
    else if (mouseMode == MouseMode::CAMERA && event->button() == activeMouseButton) {
        mouseMode = MouseMode::FREE;
        activeMouseButton = -1;
        _camera->mouseReleased(event);
    }
    else if (mouseMode == MouseMode::SELECT && event->button() == activeMouseButton) {
        mouseMode = MouseMode::FREE;
        activeMouseButton = -1;
        _basicSelect->mouseReleased(this, event);

    }
    else if (mouseMode == MouseMode::FREE && event->button() == Qt::RightButton) { // popup menu
        showContextMenu(event);
    }

    update();
}

void PanelGL::mouseMoveEvent(QMouseEvent* event)
{
    if (mouseMode == MouseMode::FREE) {
        // calculate preselection
        Point3 rayOrig = camera()->eye();
        Vector3 rayDir = computeRayDirection(event->pos());
        FaceUtil::FaceHit faceHit = FaceUtil::closestFace(_scene, rayOrig, rayDir, false);

        VertexP closestVert = VertexUtil::closestVertex(this, event, false);

        _hoverMeshKey = faceHit.nearMesh ? faceHit.nearMesh->key() : -1;
        _hoverFaceKey = faceHit.nearFace ? faceHit.nearFace->key() : -1;
        _hoverVertKey = closestVert ? closestVert->key() : -1;

        // figure out near edge and vertex
        update();
    }
    else if (mouseMode == MouseMode::TOOL) {
        //_workTool->mouseMoved(event);
        if (_ravagingMouse) { // move mouse back to center
            QPoint mouseDiff = QCursor::pos() - centerMouse(TRUE);
            if (mouseDiff != QPoint(0,0)) {
                _workTool->mouseMoved(event, mouseDiff.x(), mouseDiff.y());
                centerMouse(FALSE);
            }
        }
        update();
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
    else if (mouseMode == MouseMode::SELECT) {
        //basic_select = BasicSelect.instance
        //basic_select.mouseDrag(self, event)
        _basicSelect->mouseDragged(this, event);
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

    QList<ContextAction*> actions = _scene->contextActions();

    foreach(ContextAction* action, actions) {
        action->setParent(&popup);
        connect(action, SIGNAL(triggered()), action, SLOT(itemTriggered()));
        connect(action, SIGNAL(workToolChange(WorkTool*, QString, int)),
                this, SLOT(initWorkTool(WorkTool*, QString, int)));

        popup.addAction(action);
    }

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
