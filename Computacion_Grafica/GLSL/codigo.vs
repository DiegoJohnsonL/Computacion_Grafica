#version 330 core
layout (location = 0) in vec3 pPosicion;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
out vec4 color_vertice;
out vec2 TexCoord;
uniform vec3 colors;
uniform mat4 transformacion;  
void main(){
	gl_Position = transformacion * vec4(pPosicion, 1.0);
	color_vertice = vec4(colors, 1.0);
	TexCoord = aTexCoord;
}