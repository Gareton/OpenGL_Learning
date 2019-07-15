#version 430 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D spriteTexture;
uniform vec3 color;

void main()
{
	fragColor = texture(spriteTexture, texCoords) * vec4(color, 1.0);
}