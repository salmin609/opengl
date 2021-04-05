#version 330 core

layout(location = 0) in vec3 position;

layout(std140) uniform Matrices
{
	mat4 to_ndc;
	mat4 cam;
};
uniform mat4 model;
out vec4 clipSpace;

void main(void)
{
	clipSpace = to_ndc * cam * model * vec4(position.x, 0.0, position.y, 1.0);
	gl_Position = clipSpace;
}