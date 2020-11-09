#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec4 colorVertice;
out vec2 coordTextura;

uniform vec3 colores;
uniform mat4 modelo;
uniform mat4 vista;
uniform mat4 proyeccion;

void main(){
	gl_Position = proyeccion * vista * modelo * vec4(aPos, 1.0);
	colorVertice = vec4(colores, 1.0);
	coordTextura = vec2(aTexCoord.x, aTexCoord.y);
}