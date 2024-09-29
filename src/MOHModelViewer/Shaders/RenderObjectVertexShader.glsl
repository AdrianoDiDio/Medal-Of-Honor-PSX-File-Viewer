#version 330 core
layout (location = 0) in ivec3 aPos;
layout (location = 1) in ivec2 aTexCoord;
layout (location = 2) in ivec3 aColor;
layout (location = 3) in ivec2 aCLUTCoord;
layout (location = 4) in int   aColorMode;

uniform mat4 MVPMatrix;
uniform mat4 MVMatrix;
uniform bool EnableLighting;
uniform bool EnableFog;
uniform int ColorMode;
uniform vec3 FogColor;
uniform float FogNear;
out vec3 ourColor;
out vec2 TexCoord;
out float LightingEnabled;
out float FogEnabled;
out float FogFactor;
out vec3 ourFogColor;
out float ourFogNear;
out vec2 CLUTCoord;
flat out int ourColorMode;

void main()
{
    vec4 EyeSpacePosition;
    gl_Position =  MVPMatrix * vec4(aPos, 1.0);
    EyeSpacePosition = MVMatrix * vec4(aPos, 1.0);
    FogFactor = abs(EyeSpacePosition.z / EyeSpacePosition.w);

    ourColor = vec3(aColor) / 255.f;
    TexCoord = vec2(aTexCoord) + vec2(0.001, 0.001);
    LightingEnabled = EnableLighting ? 1.0 : 0.0;
    FogEnabled = EnableFog ? 1.0 : 0.0;
    CLUTCoord = vec2(aCLUTCoord) + vec2(0.001, 0.001);
    ourColorMode = aColorMode;
    ourFogColor = FogColor;
    ourFogNear = FogNear;
}
