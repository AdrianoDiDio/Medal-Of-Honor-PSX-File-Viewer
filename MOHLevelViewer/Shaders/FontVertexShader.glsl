#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 MVPMatrix;
uniform vec4 Color;

out vec4 ourColor;
out vec2 TexCoord;

void main(void) {
    gl_Position = MVPMatrix * vec4(aPos,0,1);
    ourColor = Color;
    TexCoord = aTexCoord;
}
