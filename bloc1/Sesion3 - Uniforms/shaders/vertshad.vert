#version 330 core

in vec3 vertex;
uniform mat4 TG;
uniform float val;

void main()  {
    gl_Position = TG*vec4(vertex*val,1.0);
}
