#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in float LightingEnabled;
uniform sampler2D ourTexture;

void main()
{
    vec4 TexColor;
    vec4 Color;
    TexColor = texture(ourTexture, TexCoord);
    
    if(TexColor.a < 0.5) {
        discard;
    }
    //PS1 Uses black color as the transparent color.
    if( TexColor.r <= 0.0 && TexColor.g <= 0.0 && TexColor.b <= 0.0 ) {
        discard;
    }
    //Water not visible in 1_1/1_2
    if( LightingEnabled > 0.5 ) {
        FragColor = TexColor * vec4(ourColor.rgb,1);
    } else {
        FragColor = TexColor;
    }
}
