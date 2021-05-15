#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

struct WaterParticle
{
	int id;
	float density;
	float lambda;
	vec3 pos;
	vec3 vel;
	vec3 force;
};

layout(std430, binding = 0) buffer particles
{
	WaterParticle particle[];
};

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;
void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	int lid = int(gl_LocalInvocationID.x);
	vec3 newPos = particle[gid].pos;
	newPos += vec3(0, 0.001, 0);
	particle[257].id = 0;
	
}