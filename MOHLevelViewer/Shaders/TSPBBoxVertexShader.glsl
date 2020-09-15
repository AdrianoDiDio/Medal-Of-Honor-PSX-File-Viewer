#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;
out vec3 ourColor;

void main()
{
    mat4 View = mat4(1.0);

    gl_Position =  MVPMatrix * vec4(aPos, 1.0);
    ourColor = vec3(1.0,0.0,0.0);
}
