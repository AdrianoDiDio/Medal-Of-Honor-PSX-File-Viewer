#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    vec4 TexColor;
    vec4 Color;

    TexColor = texture(ourTexture, TexCoord);
    if( TexColor.rgb == vec3(0.0,0.0,0.0) ) {
        discard;
    }
    if( TexColor.rgb == vec3(0.14453125,0.14453125,0.21875) ) {
    discard;
    }
    FragColor = TexColor;
}
