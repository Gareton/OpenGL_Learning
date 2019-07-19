#version 430 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D text;
uniform vec4 color;

void main()
{
	vec4 tColor = texture(text, TexCoords);
	
	if(tColor.x <= 0.1)
		FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	else
		FragColor = color;
		
}
