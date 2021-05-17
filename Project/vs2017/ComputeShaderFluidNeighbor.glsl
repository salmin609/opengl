#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;
const int particleNum = 32 * 32 * 32;

const int neighborCount = 100;
struct ParticleNeighbors
{
	int neighborIndices[neighborCount];
};

layout(std430, binding = 0) buffer poses
{
	vec4 pos[];
};

layout(std430, binding = 1) buffer neighbors
{
	ParticleNeighbors neighbor[];
};

layout(std430, binding = 2) buffer neighborsCheckCount
{
	int neighborCheckCount[];
};

const float h = 0.14f;

//neighbor
const float gap = h; //0.14f
int index = 0;

void AddNeighborIndex(int i)
{
	uint gid = gl_GlobalInvocationID.x;
	neighbor[gid].neighborIndices[index] = i;

	neighborCheckCount[gid]++;
	//neighborCheckCount[i]++;

	index++;
}

void CalculateNeighboringIndices()
{
	uint gid = gl_GlobalInvocationID.x;
	vec4 posVal = pos[gid];

	for(int i = 0; i < particleNum; ++i)
	{
		//if(neighborCheckCount[i] < 100)
		//{
			vec4 otherPosVal = pos[i];

		
			//check
			float xBoundaryMin = posVal.x - gap;
			float xBoundaryMax = posVal.x + gap;
			float yBoundaryMin = posVal.y - gap;
			float yBoundaryMax = posVal.y + gap;
			float zBoundaryMin = posVal.z - gap;
			float zBoundaryMax = posVal.z + gap;

			if(otherPosVal.x > xBoundaryMin && otherPosVal.x < xBoundaryMax && 
			otherPosVal.y > yBoundaryMin && otherPosVal.y < yBoundaryMax &&
			otherPosVal.z > zBoundaryMin && otherPosVal.z < zBoundaryMax)
			{
				AddNeighborIndex(i);
			}
		

			if(index > 99)
				break;
		//}
	}

}

void main()
{
	CalculateNeighboringIndices();
}
