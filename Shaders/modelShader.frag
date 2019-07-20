#version 430 core

in vec2 TexCoords;
in vec3 normal;
in vec3 fragPos;

out vec4 fragColor;

uniform vec3 camPos;
uniform sampler2D texture_diffuse1;
uniform samplerCube cubemap;

void main()
{
	float ratio = 1.00 / 2.42;
	vec3 I = normalize(fragPos - camPos);
	vec3 R = refract(I, normalize(normal), ratio);

	fragColor = vec4(vec3(texture(cubemap, R)), 1.0);
}