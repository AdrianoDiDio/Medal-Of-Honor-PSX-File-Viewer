#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 MVPMatrix;
out vec3 ourColor;

void main()
{
    gl_Position =  MVPMatrix * vec4(aPos, 1.0);
    ourColor = vec3(1.0,1.0,1.0);
}
 
