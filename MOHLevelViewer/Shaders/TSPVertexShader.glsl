#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;

uniform mat4 MVPMatrix;
uniform bool EnableLighting;
out vec3 ourColor;
out vec2 TexCoord;
out float LightingEnabled;
void main()
{
    mat4 View = mat4(1.0);

    gl_Position =  MVPMatrix * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
    LightingEnabled = EnableLighting ? 1.0 : 0.0;
}
