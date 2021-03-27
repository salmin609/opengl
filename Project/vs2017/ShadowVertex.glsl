#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 to_ndc;
uniform mat4 model;
uniform mat4 cam;
uniform mat4 shadow_mat;

out vec3 frag_pos;

void main()
{
	frag_pos = vec3(model * vec4(position, 1));
	gl_Position = to_ndc * cam * shadow_mat * vec4(frag_pos, 1);
};