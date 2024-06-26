#version 330 core
layout (location = 0) in ivec3 aPos;
layout (location = 1) in ivec2 aTexCoord;
layout (location = 2) in ivec3 aColor;
layout (location = 3) in ivec2 aCLUTCoord;
layout (location = 4) in int   aColorMode;

uniform mat4 MVPMatrix;
uniform int ColorMode;
uniform bool EnableLighting;
out vec3 ourColor;
out vec2 TexCoord;
out vec2 CLUTCoord;
out float LightingEnabled;
flat out int ourColorMode;

void main()
{
    gl_Position =  MVPMatrix * vec4(aPos, 1.0);
    ourColor = vec3(aColor) / 128.f;
    TexCoord = vec2(aTexCoord) + vec2(0.001, 0.001);
    CLUTCoord = vec2(aCLUTCoord) + vec2(0.001, 0.001);
    LightingEnabled = EnableLighting ? 1.0 : 0.0;
    ourColorMode = aColorMode;
}
