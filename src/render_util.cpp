#include "render_util.h"

#include <QLabel>

void setShaderUniforms(QGLShaderProgramP shader, BindableP obj, QList<Attribute> attributes);

typedef struct {
    float x;
    float y;
    float weight;
} Sample;

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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, xres, yres, 0, GL_RGBA, GL_BYTE, NULL);
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


        // draw properties to FBO textures (position, diffuse, spec, position, emissive, normal, etc.)
        //QGLShaderProgramP meshShader = ShaderFactory::buildMeshShader(panel);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

        GLenum bufs[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT };
        glDrawBuffers(3, bufs);

        QString dummyName = "sillyLightWithNameThatshouldnotExistinthewild";
        QList<QString> lights;
        lights << dummyName;
        lights.append(SunshineUi::activeScene()->lights());

        // simple box filtering
        QList<Sample> samples;
        const int SAMPLING_WIDTH = SunshineUi::renderSettings()->attributeByName("Sampling Width")->property("value").value<float>();
        const float SAMPLING_OFFSET = 1.0f / (2*SAMPLING_WIDTH);
        for (int ix = 0; ix < SAMPLING_WIDTH; ix++) {
            for (int iy = 0; iy < SAMPLING_WIDTH; iy++) {
                Sample sample;
                sample.x = SAMPLING_OFFSET + ix / (float)SAMPLING_WIDTH - 0.5;
                sample.y = SAMPLING_OFFSET + iy / (float)SAMPLING_WIDTH - 0.5;
                sample.weight = 1.0 / (SAMPLING_WIDTH * SAMPLING_WIDTH);
                samples << sample;
            }
        }

        QList<QImage> samplingImages;

        glViewport(0, 0, xres, yres);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);


        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        //glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);

        foreach(Sample sample, samples) {
            glClear(GL_DEPTH_BUFFER_BIT);

            foreach(QString lightName, lights) {
                bool zPrepass = false;
                LightP light = SunshineUi::activeScene()->light(lightName);
                if (light == 0) { // assume it to be the dummy z-pass light
                    light = LightP(new AmbientLight());
                    light->attributeByName("Intensity")->setProperty("value", 0.0f);
                    zPrepass = true;
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                }

                foreach(QString meshName, SunshineUi::activeScene()->meshes()) {
                    MeshP mesh = SunshineUi::activeScene()->mesh(meshName);
                    const int numTriangles = mesh->numTriangles();
                    MaterialP material = mesh->material();

                    QMatrix4x4 objToWorld = mesh->objectToWorld();
                    QMatrix4x4 normalToWorld = mesh->normalToWorld();

                    QMatrix4x4 samplingOffsetM;
                    samplingOffsetM.ortho(-1,1,-1,1,0,1);
                    //printMatrix(samplingOffsetM);
                    //QMatrix4x4 cameraPVOffset(cameraProjViewM);
                    //cameraPVOffset.translate(sample.x, sample.y);

                    // rebuild projection matrix using sampling offset
                    //
                    float dx = sample.x;
                    float dy = sample.y;
                    cameraProjViewM = Camera::getProjMatrix(camera,panel->width(),panel->height(),dx,dy) * cameraViewM;

                    // build the material shader
                    QGLShaderProgramP shader = ShaderFactory::buildMaterialShader(light, material, panel);
                    shader->bind();
                    shader->setUniformValue("objToWorld", objToWorld);
                    shader->setUniformValue("normalToWorld", normalToWorld);
                    //shader->setUniformValue("cameraPV", cameraProjViewM);
                    shader->setUniformValue("cameraPV", cameraProjViewM);
                    shader->setUniformValue("cameraPos", camera->eye());

                    //shader->setUniformValue("lightDir", -camera->lookDir().normalized());

                    // set uniform constants for light
                    setShaderUniforms(shader, light, light->glslFragmentConstants());

                    // pack mesh data (vertices, normals, etc.) into VBO
                    packVBO(mesh, numTriangles, vboId, shader);

                    // render to the framebuffer
                    glBindBuffer(GL_ARRAY_BUFFER, vboId);
                    //glBufferData(GL_ARRAY_BUFFER, numTriangles*3*blockSize, vertices, GL_STREAM_DRAW);
                    glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);

                    shader->release();
                }

                if (zPrepass)
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            }




            //panel->renderBeautyPass();

            // copy each attachment from OpenGL, save to disk
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            for (int i = 0; i < deferredTextures.size(); i++) {
                GLubyte data[xres*yres*4];
                glReadBuffer(GL_COLOR_ATTACHMENT0_EXT + i);
                glReadPixels(0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, data);

                QImage image = QImage(data, xres, yres, QImage::Format_ARGB32).rgbSwapped();
                QString outImage = QString("/tmp/test%1.png").arg(i);
                image.mirrored().save(outImage);

                if (i == 0)
                    samplingImages << image.mirrored();


            }

        //                glDrawBuffers(1, bufs);
        }

        glDrawBuffers(1, bufs);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // read back accumulated buffer and do color/gamma correction
        //
        GLfloat data[xres*yres*4];
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
        glReadPixels(0, 0, xres, yres, GL_RGBA, GL_FLOAT, data);

        // combine sampling images into one image
        //
        std::cout << "images rendered" << std::endl;
        QImage outImage(samplingImages[0].width(), samplingImages[0].height(), samplingImages[0].format());
        for (int x = 0; x < outImage.width(); x++) {
            for (int y = 0; y < outImage.height(); y++) {
                QColor empty(0,0,0,0);
                outImage.setPixel(x,y,empty.rgba());
            }
        }
        for (int i = 0; i < samplingImages.size(); i++) {
            QImage image = samplingImages[i];
            Sample sample = samples[i];

            for (int x = 0; x < outImage.width(); x++) {
                for (int y = 0; y < outImage.height(); y++) {
                    QColor color(image.pixel(x,y));
                    color.setAlpha(qAlpha(image.pixel(x,y)));
                    QColor accumColor(outImage.pixel(x,y));
                    accumColor.setAlpha(qAlpha(outImage.pixel(x,y)));

                    QColor combinedColor;
                    combinedColor.setRedF(std::min(color.redF()*sample.weight + accumColor.redF(), (qreal)1.0));
                    combinedColor.setGreenF(std::min(color.greenF()*sample.weight + accumColor.greenF(), (qreal)1.0));
                    combinedColor.setBlueF(std::min(color.blueF()*sample.weight + accumColor.blueF(), (qreal)1.0));
                    combinedColor.setAlphaF(std::min(color.alphaF()*sample.weight + accumColor.alphaF(), (qreal)1.0));
                    //std::cout << color.alphaF() * sample.weight << std::endl;
                    //std::cout << accumColor.alphaF() << std::endl;

                    outImage.setPixel(x, y, combinedColor.rgba());
                }
            }

            // save sampling image
            QString outPath = QString("/tmp/sampling%1.png").arg(i);
            image.save(outPath);
        }
        std::cout << "done combining" << std::endl;
        outImage.save("/tmp/aa.png");

        {
            // make a checkered background as a cue for alpha transparency
            //
            QImage checkeredImage(outImage.width(), outImage.height(), outImage.format());
            QBrush checkeredBrush(QPixmap(":/textures/checker_texture_soft.png"));
            QPainter p(&checkeredImage);
            //p.fillRect(QRect(0,0,outImage.width(),outImage.height()), checkeredBrush);
            p.fillRect(QRect(0,0,outImage.width(),outImage.height()), Qt::yellow);

            //p.setCompositionMode(QPainter::CompositionMode_SourceOver);
            p.drawImage(0, 0, outImage);
            p.end();


            //checkeredImage.save("/tmp/foo.png");

            QLabel* label = new QLabel();
            label->setStyleSheet("QLabel { background-color : #222222; }");
            label->setPixmap(QPixmap::fromImage(checkeredImage));
            label->show();
        }

        // release OpenGL resources
        //
        foreach(GLuint* addr, textures)
            glDeleteTextures(1, addr);
        glDeleteFramebuffersEXT(1, &fbo);
        glDeleteBuffers(1, &vboId);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } // end of RenderUtil::renderGL(...)

    // put stuff into VBO from mesh
    //
    void packVBO(MeshP mesh, const int numTriangles, GLint vboId, QGLShaderProgramP shader)
    {
        int triangleCount = 0;
        const uint blockSize = sizeof(QVector3D) * 2; // bytes of data per vertex
        GLbyte vertices[numTriangles*3*blockSize];
        //Point3 vertices[numTriangles*3];
        QHashIterator<int,FaceP> i = mesh->faces();
        while (i.hasNext()) {
            i.next();
            FaceP face = i.value();

            QListIterator<Triangle> j = face->buildTriangles();
            while (j.hasNext()) {
                Triangle triangle = j.next();
                *(Point3*)(vertices + triangleCount*3*blockSize + 0*blockSize) = triangle.a->vert()->pos();
                *(Point3*)(vertices + triangleCount*3*blockSize + 1*blockSize) = triangle.b->vert()->pos();
                *(Point3*)(vertices + triangleCount*3*blockSize + 2*blockSize) = triangle.c->vert()->pos();

                *(Vector3*)(vertices + triangleCount*3*blockSize + 0*blockSize+sizeof(Vector3)) = triangle.a->normal();
                *(Vector3*)(vertices + triangleCount*3*blockSize + 1*blockSize+sizeof(Vector3)) = triangle.b->normal();
                *(Vector3*)(vertices + triangleCount*3*blockSize + 2*blockSize+sizeof(Vector3)) = triangle.c->normal();

                /*
                Vector3 triangle.a->normal();
                Vector3 triangle.b->normal();
                Vector3 triangle.c->normal();
                */

                /*
                *(Point3*)(vertices+triangleCount*3*blockSize+3*sizeof(Point3)) = triangle.a->normal();
                *(Point3*)(vertices+triangleCount*3*blockSize+4*sizeof(Point3)) = triangle.b->normal();
                *(Point3*)(vertices+triangleCount*3*blockSize+5*sizeof(Point3)) = triangle.c->normal
                */
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

        offset += sizeof(Point3);

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int normalLocation = shader->attributeLocation("vertNormal");
        shader->enableAttributeArray(normalLocation);
        glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, blockSize, (const void *)offset);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

}

// goes through each attribute and attemps to set it as a uniform value in the shader
//
void setShaderUniforms(QGLShaderProgramP shader, BindableP obj, QList<Attribute> attributes)
{
    foreach(Attribute attribute, attributes) {
        QString varName = attribute->property("glslFragmentConstant").toString();
        if (varName == "" || varName == "true")
            varName = attribute->property("var").toString();

        if (attribute->type() == "color") {
            QColor c = attribute->property("value").value<QColor>();

            QVector3D color(c.redF(), c.greenF(), c.blueF());
            shader->setUniformValue(varName.toStdString().c_str(), color);
        }
        else if (attribute->type() == "point3") {
            QVector3D p;// = getBoundValue<QVector3D>(obj, attribute);

            if (attribute->property("getter").isValid())
                p = getBoundValue<QVector3D>(obj, attribute);
            else
                p = attribute->property("value").value<QVector3D>();
            shader->setUniformValue(varName.toStdString().c_str(), p);
        }
        else if (attribute->type() == "float") {
            float f;

            if (attribute->property("getter").isValid())
                f = getBoundValue<float>(obj, attribute);
            else
                f = attribute->property("value").value<float>();
            shader->setUniformValue(varName.toStdString().c_str(), f);
        }
        else {
            std::cerr << "unimplemented uniform type: " << attribute->type() << "(" << varName << ")" << std::endl;
        }
    }
}

