#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

uniform vec3 colFocus;
uniform vec3 posFocus;
uniform vec3 llumAmbient;
// Valors per als components que necessitem dels focus de llum
//CANVI: ARA NO ELS VOLEM PREDEFINITS
//vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
//vec3 colFocus = vec3(0.8, 0.8, 0.8);
//vec3 posFocus = vec3(1, 1, 1);  // en SCA

out vec3 colFocus2;
out vec3 posFocus2;
out vec3 llumAmbient2;

out vec3 normalSCO;
out vec3 focusSCO;
out vec3 vertSCO;

void main()
{	

    colFocus2 = colFocus;
    posFocus2 = posFocus;
    llumAmbient2 = llumAmbient;

    //1. Calcul normalSCO i L per a Lambert
    vertSCO = (view * TG * vec4 (vertex, 1.0));
    mat3 normalMatrix = inverse (transpose (mat3 (view * TG)));
    normalSCO =  normalize(normalMatrix * normal);
    focusSCO = view * vec4(posFocus,1.0);

    gl_Position = proj * vertSCO;
}
