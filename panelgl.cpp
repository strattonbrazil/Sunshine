#include "panelgl.h"
#include "camera.h"

// possible reference
// http://code.google.com/p/opencamlib/source/browse/trunk/cpp_examples/qt_opengl_vbo/glwidget.h?spec=svn688&r=688

#include <QVarLengthArray>

#include <iostream>
using namespace std;

LineRenderer* mainGrid = NULL;

PanelGL::PanelGL() : QGLWidget(PanelGL::defaultFormat())
{
    setMouseTracking(true);
    _validShaders = false;

    camera = new Camera();

    if (mainGrid == NULL) {
        int range[] = {-10,10};
        int numSegments = range[1]-range[0]+1;
        QVector<LineSegment> segments(numSegments);
        for (int i = 0; i < numSegments; i++) {
            segments[i].p1 = Point3(i, 0, 10);
            segments[i].p2 = Point3(i, 0, -10);
            segments[i].r = 0.4f;
            segments[i].g = 0.4f;
            segments[i].b = 0.4f;
        }
        mainGrid = new LineRenderer(segments, 2);
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
    //_segments = segments;
    _lineWidth = lineWidth;
}

struct VertexData
{
    Vector3 position;
    Vector2 texCoord;
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

    /*
    glLineWidth(_lineWidth);
    flatShader->bind();
    int objToWorldLoc = flatShader->attributeLocation("objToWorld");
    flatShader->setUniformValue(objToWorldLoc, objToWorld);
    int cameraPVLoc = flatShader->attributeLocation("cameraPV");
    flatShader->setUniformValue(cameraPVLoc, cameraProjViewM);
    int overrideStrengthLoc = flatShader->attributeLocation("overrideStrength");
    flatShader->setUniformValue(overrideStrengthLoc, 0.0f);

    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);
    // Offset for position
    int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = flatShader->attributeLocation("vertex");
    flatShader->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
    // Offset for texture coordinate
    offset += sizeof(QVector3D);
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = flatShader->attributeLocation("a_texcoord");
    flatShader->enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);

    //drawSegments.call
    flatShader->release();
    */


    glLineWidth(_lineWidth);
    flatShader->bind();
    flatShader->setUniformValue("objToWorld", objToWorld);
    flatShader->setUniformValue("cameraPV", cameraProjViewM);
    flatShader->setUniformValue("overrideStrength", 0.0f);

    int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = flatShader->attributeLocation("vertex");
    flatShader->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
    // Offset for texture coordinate
    offset += sizeof(QVector3D);
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = flatShader->attributeLocation("a_texcoord");
    flatShader->enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);


    /*
    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
    //glVertexPointer(3, GL_FLOAT, GL_UNSIGNED_BYTE, 0);

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    */

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /*
    glBegin(GL_QUADS);
    glVertex2f(0,0);
    glVertex2f(0,1);
    glVertex2f(1,1);
    glVertex2f(1,0);
    glEnd();
    */

    //drawSegments.call
    flatShader->release();
}

void LineRenderer::loadVBOs(PanelGL* panel)
{
    VertexData vertices[] = {
            // Vertex data for face 0
            {QVector3D(-1.0, -1.0,  1.0), QVector2D(0.0, 0.0)},  // v0
            {QVector3D( 1.0, -1.0,  1.0), QVector2D(0.33, 0.0)}, // v1
            {QVector3D(-1.0,  1.0,  1.0), QVector2D(0.0, 0.5)},  // v2
            {QVector3D( 1.0,  1.0,  1.0), QVector2D(0.33, 0.5)}, // v3
            // Vertex data for face 1
            {QVector3D( 1.0, -1.0,  1.0), QVector2D( 0.0, 0.5)}, // v4
            {QVector3D( 1.0, -1.0, -1.0), QVector2D(0.33, 0.5)}, // v5
            {QVector3D( 1.0,  1.0,  1.0), QVector2D(0.0, 1.0)},  // v6
            {QVector3D( 1.0,  1.0, -1.0), QVector2D(0.33, 1.0)}, // v7
            // Vertex data for face 2
            {QVector3D( 1.0, -1.0, -1.0), QVector2D(0.66, 0.5)}, // v8
            {QVector3D(-1.0, -1.0, -1.0), QVector2D(1.0, 0.5)},  // v9
            {QVector3D( 1.0,  1.0, -1.0), QVector2D(0.66, 1.0)}, // v10
            {QVector3D(-1.0,  1.0, -1.0), QVector2D(1.0, 1.0)},  // v11
            // Vertex data for face 3
            {QVector3D(-1.0, -1.0, -1.0), QVector2D(0.66, 0.0)}, // v12
            {QVector3D(-1.0, -1.0,  1.0), QVector2D(1.0, 0.0)},  // v13
            {QVector3D(-1.0,  1.0, -1.0), QVector2D(0.66, 0.5)}, // v14
            {QVector3D(-1.0,  1.0,  1.0), QVector2D(1.0, 0.5)},  // v15
            // Vertex data for face 4
            {QVector3D(-1.0, -1.0, -1.0), QVector2D(0.33, 0.0)}, // v16
            {QVector3D( 1.0, -1.0, -1.0), QVector2D(0.66, 0.0)}, // v17
            {QVector3D(-1.0, -1.0,  1.0), QVector2D(0.33, 0.5)}, // v18
            {QVector3D( 1.0, -1.0,  1.0), QVector2D(0.66, 0.5)}, // v19
            // Vertex data for face 5
            {QVector3D(-1.0,  1.0,  1.0), QVector2D(0.33, 0.5)}, // v20
            {QVector3D( 1.0,  1.0,  1.0), QVector2D(0.66, 0.5)}, // v21
            {QVector3D(-1.0,  1.0, -1.0), QVector2D(0.33, 1.0)}, // v22
            {QVector3D( 1.0,  1.0, -1.0), QVector2D(0.66, 1.0)}  // v23
        };
        // Indices for drawing cube faces using triangle strips.
        // Triangle strips can be connected by duplicating indices
        // between the strips. If connecting strips have opposite
        // vertex order then last index of the first strip and first
        // index of the second strip needs to be duplicated. If
        // connecting strips have same vertex order then only last
        // index of the first strip needs to be duplicated.
        GLushort indices[] = {
             0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
             4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
             8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
            12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
            16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
            20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
        };
        // Transfer vertex data to VBO 0
        glBindBuffer(GL_ARRAY_BUFFER, _vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(VertexData), vertices, GL_STATIC_DRAW);

        // Transfer index data to VBO 1
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIds[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 34 * sizeof(GLushort), indices, GL_STATIC_DRAW);
}
