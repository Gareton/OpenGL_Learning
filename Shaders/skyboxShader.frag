#version 430 core

in vec3 tPos;

out vec4 fragColor;

uniform samplerCube cubeMap;

void main()
{
	fragColor = texture(cubeMap, tPos);
}