#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec2 spriteID;

void main()
{
	//FragColor = texture(texture1, TexCoord, 1.0);
    vec2 coord = (TexCoord - floor(TexCoord) + spriteID)/vec2(27.0f, -27.0f);
    FragColor = texture(texture1, coord);
}