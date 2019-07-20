#version 430 core

layout (location = 0) in vec3 pos;

out vec3 tPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 transformed =  projection * view * vec4(pos, 1.0);
	gl_Position = transformed.xyww;
	tPos = pos;
}