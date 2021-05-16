#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

struct WaterParticle
{
	int id;
	float density;
	float lambda;
	vec3 pos;
	vec3 vel;
	vec3 force;
};

struct ParticleNeighbors
{
	int neighborIndices[50];
};

layout(std430, binding = 0) buffer poses
{
	vec4 pos[];
};

layout(std430, binding = 1) buffer ids
{
	int id[];
};

layout(std430, binding = 2) buffer densities
{
	float density[];
};

layout(std430, binding = 3) buffer lambdas
{
	float lambda[];
};

layout(std430, binding = 4) buffer velocities
{
	vec4 velocity[];
};

layout(std430, binding = 5) buffer forces
{
	vec4 force[];
};

layout(std430, binding = 6) buffer predictedPoses
{
	vec4 predictedPos[];
};

layout(std430, binding = 7) buffer neighbors
{
	ParticleNeighbors neighbor[];
};


const float tStep = 1.0f / 24;
const float gravity = 9.8f;
const float h = 0.14f;

const float d = 0.2f;
const float wxMin = -d;
const float wxMax = 2.f * d;
const float wyMin = -2.f * d;
const float wyMax = d;
const float wzMin = -d;
const float wzMax = d;

//neighbor
const float gap = h;
const float xMin = wxMin;
const float xMax = wxMax;
const float yMin = wyMin;
const float yMax = wyMax;
const float zMin = wzMin;
const float zMax = wzMax;
const int xNum = int(ceil((xMax - xMin) / gap));
const int yNum = int(ceil((yMax - yMin) / gap));
const int zNum = int(ceil((zMax - zMin) / gap));

ivec3 BoundaryCheck(int x, int y, int z)
{
	ivec3 result;
	result.x = x;
	result.y = y;
	result.z = z;

	if(result.x < 0)
	{
		result.x = 0;
	}
	if(result.x > xNum - 1)
	{
		result.x = xNum - 1;
	}
	if(result.y < 0)
	{
		result.y = 0;
	}
	if(result.y > yNum - 1)
	{
		result.y = yNum - 1;
	}
	if(result.z < 0)
	{
		result.z = 0;
	}
	if(result.z > zNum - 1)
	{
		result.z = zNum - 1;
	}

	return result;
}

void AdjustNeighbors(uint index)
{
	vec4 posVal = pos[index];

	int ix = int((posVal.x - xMin) / gap);
	int iy = int((posVal.y - yMin) / gap);
	int iz = int((posVal.z - zMin) / gap);
	ivec3 afterCheck = BoundaryCheck(ix, iy, iz);
	int thisIndex = ix + iy + iz;

	int id = 0;
	while(neighbor[index].neighborIndices[id] == -1)
	{
		if(neighbor[index].neighborIndices[id] == -1)
		{
			neighbor[index].neighborIndices[id] = thisIndex;
			break;
		}
		
		if(id > 49)
		{
			break;
		}
		id++;
	}
}

void PredictPosition(uint index)
{
	vec4 posVal = pos[index];
	vec4 forceVal = force[index];
	vec4 velVal = velocity[index];
	vec4 predictedPosVal = predictedPos[index];

	velVal.x += forceVal.x * tStep;
	velVal.y += forceVal.y * tStep;
	velVal.z += forceVal.z * tStep;

	forceVal.x = 0.f;
	forceVal.y = -gravity;
	forceVal.z = 0.f;

	posVal.x = predictedPosVal.x + velVal.x * tStep;
	posVal.y = predictedPosVal.y + velVal.y * tStep;
	posVal.z = predictedPosVal.z + velVal.z * tStep;

	pos[index] = posVal;
}

void main()
{
	uint gid = gl_GlobalInvocationID.x;
	
	PredictPosition(gid);
	AdjustNeighbors(gid);
}