#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec2 TexOffset;

uniform sampler2D texture1;

void main()
{
	//FragColor = texture(texture1, TexCoord, 1.0);
    vec2 coord = (TexCoord - floor(TexCoord) + TexOffset)/vec2(27.0f, -27.0f);
    FragColor = texture(texture1, coord);
}