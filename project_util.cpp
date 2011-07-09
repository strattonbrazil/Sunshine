#include "project_util.h"

#include <GL/gl.h>
#include <GL/glu.h>

/*
def unproject(winX:Float,winY:Float,winZ:Float,modelView:Array[Float],proj:Array[Float],viewport:Array[Int]):Point3 = {
 val modelBuffer = BufferUtil.newFloatBuffer(modelView)
 val projBuffer = BufferUtil.newFloatBuffer(proj)
 val viewportBuffer = BufferUtil.newIntBuffer(viewport)
 val objBuffer = BufferUtil.newFloatBuffer(4)
 glu.gluUnProject(winX, winY, winZ, modelBuffer, projBuffer, viewportBuffer, objBuffer)

  new Point3(objBuffer.get(0), objBuffer.get(1), objBuffer.get(2))
}
def project(objX:Float,objY:Float,objZ:Float,modelView:Array[Float],proj:Array[Float],viewport:Array[Int]):Point3 = {
  val modelBuffer = BufferUtil.newFloatBuffer(modelView)
  val projBuffer = BufferUtil.newFloatBuffer(proj)
  val viewportBuffer = BufferUtil.newIntBuffer(viewport)
  val screenBuffer = BufferUtil.newFloatBuffer(4)
  glu.gluProject(objX, objY, objZ, modelBuffer, projBuffer, viewportBuffer, screenBuffer)

  new Point3(screenBuffer.get(0), screenBuffer.get(1), screenBuffer.get(2))
}
*/

namespace ProjectUtil {
    Point3 unproject(float winX, float winY, float winZ, QMatrix4x4 modelView, QMatrix4x4 proj, int* viewport)
    {
        GLdouble objX, objY, objZ;
        qreal m[16];
        qreal p[16];
        modelView.transposed().copyDataTo(m);
        proj.transposed().copyDataTo(p);
        gluUnProject(winX, winY, winZ,
                     m,
                     p,
                     viewport,
                     &objX, &objY, &objZ);
        return Point3(objX, objY, objZ);
    }
}
