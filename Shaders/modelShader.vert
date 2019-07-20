#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 tCoords;

out vec2 TexCoords;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	TexCoords = tCoords;
	normal = Normal;
	fragPos = pos;
}