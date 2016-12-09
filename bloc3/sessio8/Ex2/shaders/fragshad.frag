#version 330 core

in vec3 normalSCO;
in vec3 focusSCO;
in vec3 vertSCO;
out vec4 FragColor;

void main()
{	
        vec3 L = normalize(focusSCO.xyz - vertSCO.xyz);

        //fcolor = Lambert(normalSCO,L);
        vec3 fcolor = Phong(normalSCO,L,vertSCO);

	FragColor = vec4(fcolor,1);	
}
