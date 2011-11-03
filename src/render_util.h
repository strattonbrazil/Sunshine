#ifndef RENDER_UTIL_H
#define RENDER_UTIL_H

#include "panelgl.h"

namespace RenderUtil {
    void renderGL(PanelGL* panel);

    void packVBO(MeshP mesh, const int numTriangles, GLint vboId, QGLShaderProgramP shader);
}

#endif // RENDER_UTIL_H
