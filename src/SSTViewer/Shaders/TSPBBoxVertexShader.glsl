#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 MVPMatrix;

out vec4 ourColor;

void main()
{
    gl_Position =  MVPMatrix * vec4(aPos, 1.0);
    ourColor = vec4(aColor,1.0);
}
