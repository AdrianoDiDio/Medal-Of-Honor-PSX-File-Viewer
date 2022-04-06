#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D ourTexture;


void main(void) {
    vec4 TexColor;
    vec4 Color;
    
    TexColor = texture(ourTexture, TexCoord);
    //PS1 Uses black color as the transparent color.
//     if( TexColor.r <= 0.0 && TexColor.g <= 0.0 && TexColor.b <= 0.0 ) {
//         discard;
//     }
    FragColor = TexColor;
}
