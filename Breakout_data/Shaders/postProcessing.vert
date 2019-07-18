#version 430 core

layout (location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform bool chaos;
uniform bool shake;
uniform bool confuse;
uniform float time;

void main()
{
	gl_Position = vec4(vertex.xy, 0.0, 1.0);
	vec2 resTexCoords = vertex.zw;
	
	if(chaos)
	{
		float strength = 0.3;
		resTexCoords = vec2(vertex.z + sin(time) * strength, vertex.w + cos(time) * strength);
	}
	else if(confuse)
	{
		resTexCoords = vec2(1.0 - vertex.z, 1.0 - vertex.w);
	}
	
	if(shake)
	{
		float strength = 0.01;
        gl_Position.x += cos(time * 10) * strength;        
        gl_Position.y += cos(time * 15) * strength;  
	}
	
	TexCoords = resTexCoords;
}