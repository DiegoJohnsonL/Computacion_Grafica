#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 colorVertice;

uniform vec3 colores;
uniform mat4 modelo;
uniform mat4 vista;
uniform mat4 proyeccion;

uniform int colorInterno;

void main()
{
    FragPos = vec3(modelo * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(modelo))) * aNormal;  
    TexCoords = aTexCoords;
    
    gl_Position = proyeccion * vista * vec4(FragPos, 1.0);

    colorVertice = vec4(vec3(1.0,1.0,1.0)*colorInterno + colores*(1-colorInterno), 1.0);
}