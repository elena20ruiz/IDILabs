#version 330 core

uniform int franges;
in vec3 fcolor;
out vec4 FragColor;

void main()
{	
        if (franges == 1) {
            if(int(gl_FragCoord.y)%20>=0 && int(gl_FragCoord.y)%20<10)FragColor = vec4(1,1,1,1);
            else FragColor = vec4(0,0,0,1);
        }
        else FragColor = vec4(fcolor,1);
}
