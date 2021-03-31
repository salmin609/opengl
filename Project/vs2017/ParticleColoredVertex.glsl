#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 dir; 

out vec3 colorVal;

uniform mat4 to_ndc;
uniform mat4 model;
uniform mat4 cam;

void main()
{
	gl_Position = to_ndc * cam * model * vec4(position + dir, 1);
	
	colorVal = color;
};