#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform vec3 colores;
uniform mat4 modelo;
uniform mat4 vista;
uniform mat4 proyeccion;

uniform int colorInterno;

void main(){
	gl_Position = proyeccion * vista * modelo * vec4(aPos, 1.0);
}