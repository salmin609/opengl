#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 c;
layout(location = 2) in float radius;

uniform mat4 MVP;

out vec3 color;

void main()
{
	gl_Position = MVP * vec4(vertexPos, 1);
	color = c;
	gl_PointSize = radius * 2000;
}