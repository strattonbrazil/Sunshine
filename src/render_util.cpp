#include "render_util.h"

namespace RenderUtil {
    void renderGL(PanelGL* panel)
    {
        int xres = 1000;
        int yres = 600;
        /*
        QGLWidget widget()
        QGLContext context(panel->format());
        panel->context()->create(&context);
        */
        panel->makeCurrent();

        QList<GLuint*> textures;
        GLuint diffuseSpecTexture;
        GLuint positionEmitTexture;
        GLuint normalTexture;
        GLuint finalTexture;
        GLuint depthTexture;
        textures << &diffuseSpecTexture << &positionEmitTexture << &normalTexture
                 << &finalTexture << &depthTexture;

        foreach(GLuint* addr, textures)
            glGenTextures(1, addr);

        // create VBOs and FBOs for drawing out geometry
        GLuint fbo;
        glGenFramebuffersEXT(1, &fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);


        // setup deferred textures
        QList<GLuint> deferredTextures = QList<GLuint>() << diffuseSpecTexture << positionEmitTexture << normalTexture;
        for (int i = 0; i < deferredTextures.size(); i++) {
            // setup and attach texture
            GLuint texture = deferredTextures[i];
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_UNSIGNED_SHORT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, texture, 0);
        }


        // setup final texture
        /*
        glBindTexture(GL_TEXTURE_2D, finalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_UNSIGNED_SHORT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, finalTexture, 0);
        */

        // setup depth texture
        glBindTexture( GL_TEXTURE_2D, depthTexture);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_EXT, xres, yres, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);

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

        // draw properties to FBO textures (position, diffuse, spec, position, emissive, normal, etc.)
        //QGLShaderProgramP meshShader = ShaderFactory::buildMeshShader(panel);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
        glViewport(0, 0, xres, yres);

        foreach(QString meshName, SunshineUi::activeScene()->meshes()) {
            MeshP mesh = SunshineUi::activeScene()->mesh(meshName);
            MaterialP material = mesh->material();

            // build the material shader
            QGLShaderProgramP program = ShaderFactory::buildMaterialShader(material, panel);

            // build the mesh VBO based on the required mesh parameters



            // render to the framebuffer

        }

        //panel->renderBeautyPass();

        // copy each attachment from OpenGL, save to disk
        for (int i = 0; i < deferredTextures.size(); i++) {
            GLubyte data[xres*yres*4];
            glReadBuffer(GL_COLOR_ATTACHMENT0_EXT + i);
            glReadPixels(0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, data);
            QImage image(data, xres, yres, QImage::Format_ARGB32);
            QString outImage = QString("/tmp/test%1.png").arg(i);
            std::cout << "Writing image: " << outImage.toStdString() << std::endl;
            image.mirrored().save(outImage);
        }

        // clean up OpenGL resources
        foreach(GLuint* addr, textures)
            glDeleteTextures(1, addr);
        glDeleteFramebuffersEXT(1, &fbo);

        glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
    }
}
