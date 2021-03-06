#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 normal_vec;
out vec3 frag_pos;
out vec4 light;
out vec2 texture_coord;

uniform vec4 plane;

layout(std140) uniform Matrices
{
	mat4 to_ndc;
	mat4 cam;
};
//uniform mat4 to_ndc;
//uniform mat4 cam;
uniform mat4 model;
uniform vec4 light_pos;

void main()
{
	normal_vec = mat3(transpose(inverse(model))) * normal;
	light = light_pos;
	frag_pos = vec3(model * vec4(position, 1));

	gl_ClipDistance[0] = dot(vec4(frag_pos, 1), plane);

	gl_Position = to_ndc * cam * vec4(frag_pos, 1);
	texture_coord = texcoord;
};