in vec3 vertex;
uniform mat4 objToWorld;
uniform mat4 cameraPV;

void main() {
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_Position = cameraPV * objToWorld * vec4(vertex,1.0);
  gl_FrontColor = vec4(1,1,1,1);
}
