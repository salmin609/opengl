#version 420 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

out VS_OUT
{
	vec3 N;
	vec3 L;
	vec3 V;
	flat int materialIndex;
} vs_out;

uniform vec3 lightPos = vec3(100.0, 100.0, 100.0);

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel[32];
//layout (binding = 0, std140) uniform TRANSFORM_BLOCK
//{
//	mat4 matProj;
//	mat4 matView;
//	mat4 matModel[32];
//}transforms;

void main(void)
{
	mat4 matMv = matView * matModel[gl_InstanceID];

	vec4 p = matMv * position;

	vs_out.N = mat3(matMv) * normal;

	vs_out.L = lightPos - p.xyz;

	vs_out.V = -p.xyz;

	vs_out.materialIndex = gl_InstanceID;

	gl_Position = matProj * p;
}