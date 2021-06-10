#version 430 core

layout(location = 0) in vec2 position;

uniform mat4 VP;

void main()
{
	gl_PointSize = 1.5f;
	gl_Position = VP * vec4(position, 0.0f, 1);
	//gl_Position = vec4(position, 0.0f, 1.f);
};