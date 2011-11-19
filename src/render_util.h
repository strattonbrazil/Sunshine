#ifndef RENDER_UTIL_H
#define RENDER_UTIL_H

#include "panelgl.h"

namespace RenderUtil {
    void renderGL(PanelGL* panel);
    void renderMeshes(QMatrix4x4 cameraProjViewM, LightP light, PanelGL* panel, CameraP camera, GLuint vboId, QGLShaderProgramP shader, GLuint* depthMaps=0);
    void packVBO(MeshP mesh, const int numTriangles, GLint vboId, QGLShaderProgramP shader);
}

#endif // RENDER_UTIL_H
