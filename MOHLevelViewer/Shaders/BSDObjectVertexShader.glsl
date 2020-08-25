#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;
out vec2 TexCoord;

void main()
{
    mat4 View = mat4(1.0);

    gl_Position =  MVPMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
