#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 c;
layout(location = 2) in float radius;

uniform mat4 MVP;

//out VData{
//	vec3 color;
//}vColor;

out vec3 col;

void main()
{
	gl_Position = MVP * vec4(vertexPos, 1);
	col = c;
	gl_PointSize = radius * 1000;
}