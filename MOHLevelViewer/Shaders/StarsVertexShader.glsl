#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;
out vec3 ourColor;

void main()
{
    vec4 Position;
    Position = MVPMatrix * vec4(aPos,1.0);
    gl_Position = Position.xyww;
    ourColor = aColor;
}
