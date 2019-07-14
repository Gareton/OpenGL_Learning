#version 430 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;  

void main()
{ 
	vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), 
        vec2( 0.0f,    offset), 
        vec2( offset,  offset), 
        vec2(-offset,  0.0f),   
        vec2( 0.0f,    0.0f),   
        vec2( offset,  0.0f),   
        vec2(-offset, -offset), 
        vec2( 0.0f,   -offset), 
        vec2( offset, -offset)   
    );

    float kernel[9] = float[]( 
	0, 0, 0,
        0,  1, 0,
        0, 0, 0);
	
	vec3 resColor = vec3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < 9; ++i)
	{
		resColor += vec3(texture(screenTexture, TexCoords.st + offsets[i])) * kernel[i];
	}

    FragColor = vec4(resColor, 1.0);
}