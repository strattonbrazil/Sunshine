#ifndef RENDER_UTIL_H
#define RENDER_UTIL_H

#include "panelgl.h"

namespace RenderUtil {
    void renderScene(Scene* scene, Camera* camera);
    void renderAqsis(Scene* scene, const int XRES, const int YRES);

    void renderGL(PanelGL* panel);
    void renderMeshes(QMatrix4x4 cameraProjViewM, Light* light, PanelGL* panel, Transformable* camera, GLuint vboId, QGLShaderProgram* shader, GLuint* depthMaps=0);
    void packVBO(Mesh* mesh, const int numTriangles, GLint vboId, QGLShaderProgram* shader);
}

#endif // RENDER_UTIL_H
