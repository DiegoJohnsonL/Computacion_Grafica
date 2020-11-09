#version 330 core
out vec4 FragColor;

in vec4 colorVertice;
in vec2 coordTextura;

uniform float mixValue;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main(){
    FragColor = mix(texture(texture1, coordTextura), texture(texture2, coordTextura), mixValue) * colorVertice;
}