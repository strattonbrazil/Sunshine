#include "render_util.h"

namespace RenderUtil {
    void renderGL(PanelGL* panel)
    {
        CameraP camera = panel->camera();
        const int xres = 1000;
        const int yres = 600;
        /*
        QGLWidget widget()
        QGLContext context(panel->format());
        panel->context()->create(&context);
        */
        panel->makeCurrent();

        QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera,panel->width(),panel->height());
        QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera,panel->width(),panel->height());
        QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;

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
        GLuint vboId;
        glGenBuffers(1, &vboId);


        // setup deferred textures
        QList<GLuint> deferredTextures = QList<GLuint>() << diffuseSpecTexture << positionEmitTexture << normalTexture;
        for (int i = 0; i < deferredTextures.size(); i++) {
            // setup and attach texture
            GLuint texture = deferredTextures[i];
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_BYTE, NULL);
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

        // prepare all the lights in the scene (render shadow maps, calculate bounding boxes, etc.)
        //
        LightP light;

        // draw properties to FBO textures (position, diffuse, spec, position, emissive, normal, etc.)
        //QGLShaderProgramP meshShader = ShaderFactory::buildMeshShader(panel);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

        glViewport(0, 0, xres, yres);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLenum bufs[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT };
        glDrawBuffers(3, bufs);

        QList<LightP> lights;
        lights << LightP(new PointLight());

        foreach(QString meshName, SunshineUi::activeScene()->meshes()) {
            MeshP mesh = SunshineUi::activeScene()->mesh(meshName);
            const int numTriangles = mesh->numTriangles();
            MaterialP material = mesh->material();

            QMatrix4x4 objToWorld = mesh->objectToWorld();
            QMatrix4x4 normalToWorld = mesh->normalToWorld();

            foreach(LightP light,lights) {
                // build the material shader
                QGLShaderProgramP shader = ShaderFactory::buildMaterialShader(light, material, panel);
                shader->bind();
                shader->setUniformValue("objToWorld", objToWorld);
                shader->setUniformValue("normalToWorld", normalToWorld);
                shader->setUniformValue("cameraPV", cameraProjViewM);
                shader->setUniformValue("cameraPos", camera->eye());
                shader->setUniformValue("lightDir", -camera->lookDir().normalized());

                int triangleCount = 0;
                const uint blockSize = sizeof(QVector3D); // bytes of data per vertex
                GLbyte vertices[numTriangles*3*blockSize];
                //Point3 vertices[numTriangles*3];
                QHashIterator<int,FaceP> i = mesh->faces();
                while (i.hasNext()) {
                    i.next();
                    FaceP face = i.value();

                    QListIterator<Triangle> j = face->buildTriangles();
                    while (j.hasNext()) {
                        Triangle triangle = j.next();
                        *(Point3*)(vertices+triangleCount*3*blockSize+0*blockSize) = triangle.a->vert()->pos();
                        *(Point3*)(vertices+triangleCount*3*blockSize+1*blockSize) = triangle.b->vert()->pos();
                        *(Point3*)(vertices+triangleCount*3*blockSize+2*blockSize) = triangle.c->vert()->pos();
                        /*
                    vertices[triangleCount*3+0] = triangle.a->vert()->pos();
                    vertices[triangleCount*3+1] = triangle.b->vert()->pos();
                    vertices[triangleCount*3+2] = triangle.c->vert()->pos();
                    */
                        triangleCount++;
                    }
                }

                // build the mesh VBO based on the required mesh parameters
                glBindBuffer(GL_ARRAY_BUFFER, vboId);
                glBufferData(GL_ARRAY_BUFFER, numTriangles*3*blockSize, vertices, GL_STREAM_DRAW);

                int offset = 0;
                // Tell OpenGL programmable pipeline how to locate vertex position data
                int vertexLocation = shader->attributeLocation("vertex");
                shader->enableAttributeArray(vertexLocation);
                glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, blockSize, (const void *)offset);

                // render to the framebuffer
                glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);

                // clean up
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                shader->release();
            }
        }

        glDrawBuffers(1, bufs);

        //panel->renderBeautyPass();

        // copy each attachment from OpenGL, save to disk
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        for (int i = 0; i < deferredTextures.size(); i++) {
            GLubyte data[xres*yres*4];
            glReadBuffer(GL_COLOR_ATTACHMENT0_EXT + i);
            glReadPixels(0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, data);

            QImage image = QImage(data, xres, yres, QImage::Format_ARGB32).rgbSwapped();
            QString outImage = QString("/tmp/test%1.png").arg(i);
            std::cout << "Writing image: " << outImage.toStdString() << std::endl;
            image.mirrored().save(outImage);
        }

        // clean up OpenGL resources
        foreach(GLuint* addr, textures)
            glDeleteTextures(1, addr);
        glDeleteFramebuffersEXT(1, &fbo);
        glDeleteBuffers(1, &vboId);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
}
