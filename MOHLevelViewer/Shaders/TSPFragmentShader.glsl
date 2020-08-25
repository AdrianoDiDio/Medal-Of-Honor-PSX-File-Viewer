#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    vec4 TexColor;
    vec4 Color;
    TexColor = texture(ourTexture, TexCoord);
    
    //PS1 Uses black color as the transparent color.
    if( TexColor.rgb == vec3(0.0,0.0,0.0) ) {
        discard;
    }
    //Water not visible in 1_1/1_2
    FragColor = TexColor * vec4(ourColor.rgb,1);
}
