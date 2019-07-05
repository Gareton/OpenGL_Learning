#version 430 core

in vec3 Color;
in vec2 tCords;

out vec4 fragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float attitude;

void main()
{
	fragColor = mix(texture(texture1, tCords), texture(texture2, tCords), attitude);
}