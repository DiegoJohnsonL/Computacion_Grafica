#version 330 core
out vec4 colorFrag;

in vec4 colorVertice;
in vec2 coordTextura;

uniform float mixValue;
uniform int texturaInterna;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main(){
	colorFrag = mix(texture(texture1, coordTextura), texture(texture2, coordTextura), mixValue)*colorVertice*texturaInterna + colorVertice*(1-texturaInterna);
}