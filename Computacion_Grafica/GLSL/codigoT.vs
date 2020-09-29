#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 view;
out vec2 TexCoord;
uniform mat4 transformacion;
void main()
{
    gl_Position = view * transformacion *vec4(aPos, 1.0);
   
    TexCoord = aTexCoord;
}

