#version 330 core
out vec4 Color;
in  vec4 color_vertice;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main(){
	Color = texture(ourTexture, TexCoord);
}