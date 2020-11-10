#version 330 core
out vec4 colorFrag;

uniform vec3 color;

void main(){
	colorFrag = vec4(color, 1.0);
	float gamma = 2.2;
    //colorFrag.rgb = pow(colorFrag.rgb, vec3(1.0/gamma));
}