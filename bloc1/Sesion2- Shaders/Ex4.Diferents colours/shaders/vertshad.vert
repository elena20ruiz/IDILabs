#version 330 core

 in vec3 v;
 in vec3 c;
 out vec4 colorx;

void main {
  
   colorx = vec4(c, 1.0);

  gl_Position = vec4(v*0.5, 1.0);
  
}