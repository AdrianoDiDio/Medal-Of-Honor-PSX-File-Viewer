#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in float LightingEnabled;
in float FogEnabled;
in float FogFactor;
in float ourFogNear;
in vec3  ourFogColor;
in vec2 CLUTCoord;
flat in int ourColorMode;
flat in int STPMode;

uniform usampler2D ourIndexTexture;
uniform sampler2D ourPaletteTexture;
        
const float FogMax = 6000.f;

uint InternalToPsxColor(vec4 c) {
    uint a = uint(floor(c.a + 0.5));
    uint r = uint(floor(c.r * 31.0 + 0.5));
    uint g = uint(floor(c.g * 31.0 + 0.5));
    uint b = uint(floor(c.b * 31.0 + 0.5));
    return (a << 15) | (b << 10) | (g << 5) | r;
}

void main()
{
    uvec4 TexColor;
    uint CLUTIndex;
    vec4 CLUTTexel;
    uint CLUTX;
    uint CLUTY;
    float FogMin;
    float InterpolationFactor;

    //NOTE(Adriano):16-bpp mode textures are encoded directly into CLUT.
    if( ourColorMode == 2 ) {
        CLUTTexel = texelFetch(ourPaletteTexture, ivec2(TexCoord), 0);
    } else {
        TexColor = texelFetch(ourIndexTexture, ivec2(TexCoord), 0);
        CLUTIndex = TexColor.r;
        CLUTX = uint(CLUTCoord.x) + CLUTIndex;
        CLUTY = uint(CLUTCoord.y);
        CLUTTexel = texelFetch(ourPaletteTexture, ivec2(CLUTX,CLUTY), 0);
    }
    if( InternalToPsxColor(CLUTTexel) == 0x0000u) {
        discard;
    }

    if( LightingEnabled > 0.5 ) {
        CLUTTexel.r = clamp(CLUTTexel.r * ourColor.r * 2.f, 0.f, 1.f);
        CLUTTexel.g = clamp(CLUTTexel.g * ourColor.g * 2.f, 0.f, 1.f);
        CLUTTexel.b = clamp(CLUTTexel.b * ourColor.b * 2.f, 0.f, 1.f);
    }
    
    if( FogEnabled > 0.5 ) {
        FogMin = (ourFogNear / 4096.f) * 20.f;
        InterpolationFactor = 1.0 - clamp((FogMax - FogFactor) / (FogMax - FogMin), 0.0, 1.0);
        CLUTTexel = mix(CLUTTexel, vec4(ourFogColor,1.0), InterpolationFactor);
    }
    
    FragColor = CLUTTexel;
}
