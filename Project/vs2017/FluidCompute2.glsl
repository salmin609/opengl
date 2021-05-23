#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

const int pxNum = 20;
const int pyNum = 20;
const int pzNum = 20;

const int pTotalCount = pxNum * pyNum * pzNum;
const int neighborCount = 3 * 3 * 3;


struct ParticleNeighbors
{
	int neighborIndices[neighborCount];
};

struct ParticleVec4
{
	vec4 pos;
	vec4 velocity;
	vec4 force;
	vec4 predictedPos;
};
struct ParticleVal
{
	float density;
	float lambda;
	int id;
};
struct BubbleVec4
{
	vec4 bubblePos;
	vec4 bubbleVel;
};


layout(std430, binding = 0) buffer particleVec4
{
	ParticleVec4 particleInfoVec4[];
};

layout(std430, binding = 1) buffer particleFloat
{
	ParticleVal particleInfoValue[];
};

layout(std430, binding = 2) buffer neighbors
{
	ParticleNeighbors neighbor[];
};

layout(std430, binding = 3) buffer poses
{
	vec4 pos[];
};


const float PI = 3.1415926f;
const float tStep = 0.0083f;
const float gravity = -9.82f;

void TimeStep();


void TimeStep()
{
	int index = int(gl_GlobalInvocationID.x);
	ParticleVec4 vec4Val = particleInfoVec4[index];

	vec4 velVal = vec4Val.velocity;
	vec4 predictedVal = vec4Val.predictedPos;
	vec4 posVal = vec4Val.pos;

	velVal.y = velVal.y - (tStep * gravity);
	predictedVal = posVal + tStep * velVal;

	particleInfoVec4[index].predictedPos = predictedVal;
	particleInfoVec4[index].velocity = velVal;
	pos[index] = predictedVal;

}

void main()
{
	TimeStep();	
}