#include "render_util.h"

#include <QLabel>
#include <QDir>
#include <QElapsedTimer>

class Filter
{
public:
    virtual float getSize() = 0;
    virtual float getWeight(float x, float y) = 0;
};

class BoxFilter : public Filter
{
public:
    float getSize() { return 1.0; }
    float getWeight(float dx, float dy) { return 1.0f; }
};

class GaussianFilter : public Filter
{
    static const int SIZE = 2.0;
public:
    float getSize() { return SIZE; }
    float getWeight(float dx, float dy) {
        float es2 = -exp(-SIZE*SIZE);
        float gx = exp(-dx * dx) + es2;
        float gy = exp(-dy * dy) + es2;
        return gx * gy;
    }
};

class RenderContext
{
    RenderContext() {
        textureUnit = 0;
    }
private:
    GLuint textureUnit;
};

QString formattedTime(qint64 milliseconds)
{
    int seconds = milliseconds / 1000;
    int hours = seconds / 3600;
    seconds -= hours * 3600;
    int minutes = seconds / 60;
    seconds -= minutes * 60;

    QString hoursStr = QString("%1").arg(hours, 2, 10, QChar('0')).toUpper();
    QString minsStr = QString("%1").arg(minutes, 2, 10, QChar('0')).toUpper();
    QString secondsStr = QString("%1").arg(seconds, 2, 10, QChar('0')).toUpper();

    return QString("%1h:%2m:%3s").arg(hoursStr).arg(minsStr).arg(secondsStr);
}

void setShaderUniforms(QGLShaderProgram* shader, Bindable* obj, QList<Attribute> attributes);

typedef struct {
    float x;
    float y;
    float weight;
} Sample;

void exposureCorrection(float exposure, float maxLuminance, float *r, float *g, float *b)
{
    //float maxLuminance = std::max(*r, std::max(*g, *b));
    float zeta = exposure * (exposure / maxLuminance + 1.0) / (exposure + 1.0);
    //std::cout << zeta << std::endl;
    *r *= zeta;
    *g *= zeta;
    *b *= zeta;
}

void checkGL(QString s)
{
    GLenum errCode;
    std::string errString;
    errCode = glGetError();
    if (errCode != GL_NO_ERROR)
    {
        errString = std::string((const char*)gluErrorString(errCode));
        std::cerr << s << ":" << errString << std::endl;
    }
}

/*
static GLenum faceTarget[6] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

static QVector3D faceTargetDir[6] = {
    QVector3D(1,0,0),
    QVector3D(-1,0,0),
    QVector3D(0,1,0),
    QVector3D(0,-1,0),
    QVector3D(0,0,1),
    QVector3D(0,0,-1)
};

static QVector3D faceTargetUp[6] = {
    QVector3D(0,1,0),
    QVector3D(0,1,0),
    QVector3D(1,0,0),
    QVector3D(1,0,0),
    QVector3D(0,1,0),
    QVector3D(0,1,0)
};
*/

namespace RenderUtil {
    void renderScene(Scene* scene, Camera* camera)
    {
    }

    void renderAqsis(Scene* scene, const int XRES, const int YRES)
    {
    }

    void renderGL(PanelGL* panel)
    {
        checkGL("renderGL start");

        Transformable* camera = panel->camera();
        const int xres = SunshineUi::renderSettings()->attributeByName("Image Width")->property("value").value<int>();
        const int yres = SunshineUi::renderSettings()->attributeByName("Image Height")->property("value").value<int>();

        std::cout << xres << "x" << yres << std::endl;
        /*
        QGLWidget widget()
        QGLContext context(panel->format());
        panel->context()->create(&context);
        */
        panel->makeCurrent();

        QMatrix4x4 cameraViewM = Camera::getViewMatrix(camera,panel->width(),panel->height());
        QMatrix4x4 cameraProjM = Camera::getProjMatrix(camera,panel->width(),panel->height());
        //QMatrix4x4 cameraProjViewM = cameraProjM * cameraViewM;



        /*
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
            */

        // create VBOs and FBOs for drawing out geometry
        const int NUM_COLOR_ATTACHMENTS = 1;
        GLuint fbos[2];
        GLuint depthBufferIds[2];
        GLuint colorBufferIds[NUM_COLOR_ATTACHMENTS];
        glGenFramebuffersEXT(2, fbos);

        GLuint vboId;
        glGenBuffers(1, &vboId);

        GLuint depthMaps[2];


        // setup deferred textures
        /*
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
        glBindTexture( GL_TEXTURE_2D, 0);
*/

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

        // depth map setup
        const int shadowXres = 2048;
        const int shadowYres = 2048;

        glGenTextures(2, depthMaps);
        for (int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, depthMaps[i]);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        //const int shadowXres = 2048;
        //const int shadowYres = 2048;

        // Create depth renderbuffers
        //
        glGenRenderbuffers(2, depthBufferIds);
        for (int i = 0; i < 2; i++) {
            glBindFramebufferEXT(GL_FRAMEBUFFER, fbos[i]);
            glBindRenderbuffer(GL_RENDERBUFFER, depthBufferIds[i]);

            if (i == 0)
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, xres, yres);
            else
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, shadowXres, shadowYres);
            glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferIds[i]);
        }
        glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Create color renderbuffers (just for first FBO)
        //
        glBindFramebufferEXT(GL_FRAMEBUFFER, fbos[0]);
        glGenRenderbuffers(NUM_COLOR_ATTACHMENTS, colorBufferIds);
        for (int i = 0; i < NUM_COLOR_ATTACHMENTS; i++) {
            glBindRenderbuffer(GL_RENDERBUFFER, colorBufferIds[i]);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F_ARB, xres, yres); checkGL("valid storage format");
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_RENDERBUFFER, colorBufferIds[i]); checkGL("attaching render buffer to FBO");
        }
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

        checkGL("build general resources");



        /*
        glBindTexture( GL_TEXTURE_2D, depthTexture);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_EXT, xres, yres, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);

        glBindTexture( GL_TEXTURE_2D, 0);
        */

        // check status of both FBOs
        //
        for (int i = 0; i < 2; i++) {
            glBindFramebufferEXT(GL_FRAMEBUFFER, fbos[i]);
            switch (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)) {
            case GL_FRAMEBUFFER_COMPLETE_EXT:
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                std::cerr << "fbo " << i << ": GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                std::cerr << "fbo " << i << ": GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                std::cerr << "fbo " << i << ": GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                std::cerr << "fbo " << i << ": GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << std::endl;
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
                std::cerr << "fbo " << i << ": GL_FRAMEBUFFER_UNSUPPORTED_EXT" << std::endl;
                break;
            }
        }

        // prepare all the lights in the scene (render shadow maps, calculate bounding boxes, etc.)
        //


        // draw properties to FBO textures (position, diffuse, spec, position, emissive, normal, etc.)
        //QGLShaderProgram* meshShader = ShaderFactory::buildMeshShader(panel);
        glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);

        GLenum bufs[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT };
        //glDrawBuffers(3, bufs);

        QString dummyName = "sillyLightWithNameThatshouldnotExistinthewild";
        QList<QString> lights;
        lights << dummyName;
        lights.append(SunshineUi::activeScene()->lights());

        const uint NUM_LIGHT_PASSES = lights.size() - 1; // don't count dummy light

        // simple filtering
        GaussianFilter filter;

        QList<Sample> samples;
        const int SAMPLING_WIDTH = SunshineUi::renderSettings()->attributeByName("Sampling Width")->property("value").value<float>();
        const float SAMPLING_OFFSET = filter.getSize() / SAMPLING_WIDTH;
        const float SAMPLING_OFFSET_HALF = SAMPLING_OFFSET * 0.5f;
        for (int ix = 0; ix < SAMPLING_WIDTH; ix++) {
            for (int iy = 0; iy < SAMPLING_WIDTH; iy++) {
                Sample sample;
                sample.x = SAMPLING_OFFSET*ix - (float)filter.getSize()*0.5f + SAMPLING_OFFSET_HALF;
                sample.y = SAMPLING_OFFSET*iy - (float)filter.getSize()*0.5f + SAMPLING_OFFSET_HALF;
                std::cout << sample.x << "," << sample.y << std::endl;
                sample.weight = filter.getWeight(sample.x, sample.y) / (SAMPLING_WIDTH * SAMPLING_WIDTH);
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
                Light* light = SunshineUi::activeScene()->light(lightName);
                if (light == 0) { // assume it to be the dummy z-pass light
                    light = new AmbientLight();
                    light->attributeByName("Intensity")->setProperty("value", 0.0f);
                    zPrepass = true;
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                }

                // update shadows if necessary
                //
                bool casting = false;
                Attribute castShadows = light->attributeByName("Casts Shadows");
                if (castShadows && castShadows->property("value").isValid()) {
                    casting = castShadows->property("value").value<bool>();

                    if (casting) {
                        glBindFramebufferEXT(GL_FRAMEBUFFER, fbos[1]); checkGL("bound depth FBO");
                        glViewport(0, 0, shadowXres, shadowYres); checkGL("set viewport");
                        Camera* shadowCamera(new Camera());
                        //shadowCamera->setCenter(light->center());

                        // shadowCamera <- set direction and up vector
                        QImage depthImage(shadowXres, shadowYres, QImage::Format_ARGB32);
                        GLfloat* pixels = new GLfloat[shadowXres*shadowYres];
                        QGLShaderProgram* distanceShader = ShaderFactory::buildDistanceShader(panel); checkGL("created distance shader");
                        distanceShader->bind(); checkGL("bound distance shader");
                        distanceShader->setUniformValue("origin", light->center()); checkGL("set light center");



                        QVector4D d0[] = { QVector4D(0,0,1,0), QVector4D(0,0,-1,0) };
                        for (int i = 0; i < 2; i++) { // render scene in both directions
                            distanceShader->setUniformValue("d0", d0[i]);
                            glBindTexture(GL_TEXTURE_2D, depthMaps[i]); checkGL("bound depth map");
                            //shadowCamera->orient(light->center(), light->center()+faceTargetDir[i], faceTargetUp[i]);
                            glClear(GL_DEPTH_BUFFER_BIT); checkGL("cleared depth buffer");

                            QMatrix4x4 worldToLightM;
                            worldToLightM.translate(-light->center());
                            //QMatrix4x4 shadowCameraViewM = Camera::getViewMatrix(shadowCamera, shadowXres, shadowYres);
                            //QMatrix4x4 shadowCamera*rojViewM = Camera::getProjMatrix(shadowCamera,shadowXres,shadowYres,0,0) * shadowCameraViewM;
                            renderMeshes(worldToLightM, light, panel, shadowCamera, vboId, distanceShader);  checkGL("renderMeshes() for shadow map");
                            //renderMeshes(shadowCamera*rojViewM, light, panel, shadowCamera, vboId, QGLShaderProgram*(0));

                            // copy into correct face
                            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadowXres, shadowYres, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0); checkGL("glteximage2d to depth texture");
                            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowXres, shadowYres); checkGL("copytexsubimage2d to depth texture");

                            /*
                            glReadPixels(0, 0, shadowXres, shadowYres, GL_DEPTH_COMPONENT, GL_FLOAT, pixels); checkGL("read pixels from depth buffer");
                            for (int x = 0; x < shadowXres; x++) {
                                for (int y = 0; y < shadowYres; y++) {
                                    float depth = pixels[x+y*shadowXres];
                                    //if (depth < 0.5)
                                        //std::cout << depth << std::endl;
                                    QColor depthColor(depth*255,depth*255,depth*255,255);
                                    depthImage.setPixel(x,y,depthColor.rgba());
                                }
                            }
                            QString fileName = QString("/tmp/depth%1.png").arg(i);
                            depthImage.save(fileName);
                            */

                        }
                        delete[] pixels;
                        glBindFramebufferEXT(GL_FRAMEBUFFER, fbos[0]);  checkGL("bound beauty FBO");
                        glViewport(0, 0, xres, yres);  checkGL("reset viewport");
                        //glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                        distanceShader->release(); checkGL("released distance shader");
                    }



                    //glClear(GL_DEPTH_BUFFER_BIT);
                    checkGL("updated shadow map");
                }

                // rebuild projection matrix using sampling offset
                //
                float dx = sample.x;
                float dy = sample.y;
                QMatrix4x4 cameraProjViewM = Camera::getProjMatrix(camera,xres,yres,dx,dy) * cameraViewM;
                //glEnable(GL_TEXTURE_2D);
                if (casting)
                    renderMeshes(cameraProjViewM, light, panel, camera, vboId, 0, depthMaps);
                else
                    renderMeshes(cameraProjViewM, light, panel, camera, vboId, 0, 0);
                //glDisable(GL_TEXTURE_2D);

                if (zPrepass)
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            }



        }

        glDrawBuffers(1, bufs);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        checkGL("collected samples");

        // read back accumulated buffer and do color/gamma correction
        //
        //GLfloat data[xres*yres*4];
        const float exposure = SunshineUi::renderSettings()->attributeByName("Exposure")->property("value").value<float>();
        QImage outImage(xres, yres, QImage::Format_ARGB32);
        GLfloat* data = new GLfloat[xres*yres*4];
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
        glReadPixels(0, 0, xres, yres, GL_RGBA, GL_FLOAT, data);
        float maxLuminance = 0.0f;
        for (int x = 0; x < xres; x++) {
            for (int y = 0; y < yres; y++) {
                float aInv = 1;
                float aF = data[4*(x+y*xres)+3] / NUM_LIGHT_PASSES;
                if (aF > .05) aInv = 1.0f / aF;
                float rF = data[4*(x+y*xres)+0]*aInv;
                float gF = data[4*(x+y*xres)+1]*aInv;
                float bF = data[4*(x+y*xres)+2]*aInv;
                aF = std::min(aF / samples.size(), 1.0f);
                data[4*(x+y*xres)+0] = rF;
                data[4*(x+y*xres)+1] = gF;
                data[4*(x+y*xres)+2] = bF;
                data[4*(x+y*xres)+3] = aF;
                maxLuminance = std::max(maxLuminance, std::max(rF, std::max(gF, bF)));
            }
        }

        for (int i = 0; i < xres*yres; i++) {
            float rF = data[4*i+0];
            float gF = data[4*i+1];
            float bF = data[4*i+2];
            float aF = data[4*i+3];

            // do gamma/tone-mapping here
            if (maxLuminance > 0.000001)
                exposureCorrection(exposure, maxLuminance, &rF, &gF, &bF);

            unsigned int r = std::min((int)(255*rF), 255);
            unsigned int g = std::min((int)(255*gF), 255);
            unsigned int b = std::min((int)(255*bF), 255);
            unsigned int a = std::min((int)(255*aF), 255);

            outImage.bits()[4*i+0] = b;
            outImage.bits()[4*i+1] = g;
            outImage.bits()[4*i+2] = r;
            outImage.bits()[4*i+3] = a;
        }

        outImage = outImage.mirrored();
        delete[] data;

        {
            // make a checkered background as a cue for alpha transparency
            //
            QImage checkeredImage(outImage.width(), outImage.height(), outImage.format());
            QBrush checkeredBrush(QPixmap(":/textures/checker_texture_soft.png"));
            QPainter p(&checkeredImage);
            p.fillRect(QRect(0,0,outImage.width(),outImage.height()), checkeredBrush);
            //p.fillRect(QRect(0,0,outImage.width(),outImage.height()), Qt::yellow);

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
        glDeleteFramebuffersEXT(2, fbos); checkGL("removing framebuffers");
        glDeleteRenderbuffers(2, depthBufferIds);
        glDeleteRenderbuffers(NUM_COLOR_ATTACHMENTS, colorBufferIds);
        glDeleteBuffers(1, &vboId);

        glDeleteTextures(2, depthMaps);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        checkGL("renderGL end");
    } // end of RenderUtil::renderGL(...)

    // renders each mesh in the scene
    //
    void renderMeshes(QMatrix4x4 cameraProjViewM, Light* light, PanelGL* panel, Transformable* camera, GLuint vboId, QGLShaderProgram* forceShader, GLuint* depthMaps)    {
        foreach(QString meshName, SunshineUi::activeScene()->meshes()) {
            Mesh* mesh = SunshineUi::activeScene()->mesh(meshName);
            const int numTriangles = mesh->numTriangles();
            Material* material = mesh->material();

            QMatrix4x4 objToWorld = mesh->objectToWorld();
            QMatrix4x4 normalToWorld = mesh->normalToWorld();

            QMatrix4x4 samplingOffsetM;
            samplingOffsetM.ortho(-1,1,-1,1,0,1);
            //printMatrix(samplingOffsetM);
            //QMatrix4x4 cameraPVOffset(cameraProjViewM);
            //cameraPVOffset.translate(sample.x, sample.y);



            // build the material shader
            QGLShaderProgram* shader = forceShader;
            if (forceShader == 0) {
                //glActiveTexture(GL_TEXTURE0);
                shader = ShaderFactory::buildMaterialShader(light, material, panel);  checkGL("created material shader");
                shader->bind(); checkGL("bound material shader");
                shader->setUniformValue("normalToWorld", normalToWorld);
                shader->setUniformValue("cameraPos", camera->eye());
                shader->setUniformValue("cameraPV", cameraProjViewM);
                shader->setUniformValue("objToWorld", objToWorld);
                checkGL("set material shader camera uniforms");

                // set uniform constants for light
                setShaderUniforms(shader, light, light->glslFragmentConstants()); checkGL("set material shader light uniforms");

                // set uniform constants for material
                //setShaderUniforms(shader, material, material->glslFragmentConstants()); checkGL("set material shader light uniforms");

                if (depthMaps != 0) {
                    QString depthMapNames[] = { "depthMapP", "depthMapN" };
                    for (int i = 0; i < 2; i++) {
                        glActiveTexture(GL_TEXTURE0+i);
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, depthMaps[i]); checkGL("bind depth texture");
                        shader->setUniformValue(depthMapNames[i].toStdString().c_str(), i); checkGL("set dm texture uniform");
                    }
                    QMatrix4x4 worldToLightM;
                    worldToLightM.translate(-light->center());
                    shader->setUniformValue("worldToLight", worldToLightM);
                }
            }
            else
                shader->setUniformValue("objToLight", cameraProjViewM * objToWorld);

            //shader->setUniformValue("objToLight", objToWorld);

            // pack mesh data (vertices, normals, etc.) into VBO
            packVBO(mesh, numTriangles, vboId, shader); checkGL("packed VBO");

            // render to the framebuffer
            glBindBuffer(GL_ARRAY_BUFFER, vboId); checkGL("bound VBO");
            //glBufferData(GL_ARRAY_BUFFER, numTriangles*3*blockSize, vertices, GL_STREAM_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, numTriangles*3); checkGL("draw arrays");
            glBindBuffer(GL_ARRAY_BUFFER, 0); checkGL("disable VBO");

            if (forceShader == 0) {
                shader->release(); checkGL("released mesh shader");
                if (depthMaps != 0) { // turn off shadow maps
                    for (int i = 1; i >= 0; i--) {
                        glActiveTexture(GL_TEXTURE0+i);
                        glDisable(GL_TEXTURE_2D);
                    }
                }
            }
        }
    }

    // put stuff into VBO from mesh
    //
    void packVBO(Mesh* mesh, const int numTriangles, GLint vboId, QGLShaderProgram* shader)
    {
        int triangleCount = 0;
        const uint blockSize = sizeof(QVector3D) * 2; // bytes of data per vertex
        GLbyte vertices[numTriangles*3*blockSize];
        //Point3 vertices[numTriangles*3];
        QHashIterator<int,Face*> i = mesh->faces();
        while (i.hasNext()) {
            i.next();
            Face* face = i.value();

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
        glBindBuffer(GL_ARRAY_BUFFER, vboId); checkGL("packVBO: glBindBuffer");
        glBufferData(GL_ARRAY_BUFFER, numTriangles*3*blockSize, vertices, GL_STREAM_DRAW); checkGL("packVBO: glBufferData");

        int offset = 0;
        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = shader->attributeLocation("vertex");  checkGL("packVBO: vertex");
        shader->enableAttributeArray(vertexLocation);  checkGL("packVBO: enabled attributes");
        glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, blockSize, (const void *)offset);  checkGL("packVBO: pointed at vertex info");

        offset += sizeof(Point3);

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int normalLocation = shader->attributeLocation("vertNormal");  checkGL("packVBO: normal");
        if (normalLocation != -1) {
            shader->enableAttributeArray(normalLocation); checkGL("packVBO: enabled attributes again");
            glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, blockSize, (const void *)offset); checkGL("packVBO: pointed at normal info");
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);  checkGL("packVBO: released array buffer");

    }
}

// goes through each attribute and attemps to set it as a uniform value in the shader
//
void setShaderUniforms(QGLShaderProgram* shader, Bindable* obj, QList<Attribute> attributes)
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
        else if (attribute->type() == "vector3") {
            QVector3D v;// = getBoundValue<QVector3D>(obj, attribute);

            if (attribute->property("getter").isValid())
                v = getBoundValue<QVector3D>(obj, attribute);
            else
                v = attribute->property("value").value<QVector3D>();
            shader->setUniformValue(varName.toStdString().c_str(), v);
        }
        else if (attribute->type() == "float") {
            float f;

            if (attribute->property("getter").isValid())
                f = getBoundValue<float>(obj, attribute);
            else
                f = attribute->property("value").value<float>();
            shader->setUniformValue(varName.toStdString().c_str(), f);
        }
        else if (attribute->type() == "samplerCubeShadow") {
            // set later
        }
        else {
            std::cerr << "unimplemented uniform type: " << attribute->type() << "(" << varName << ")" << std::endl;
        }
    }
}

