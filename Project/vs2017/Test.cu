#include "SandParticle.h"
#include <cuda_runtime.h>
#include "device_launch_parameters.h"
#include <stdio.h>
#include "Affine.h"
#include <atomic>
const int width = 50;
const int height = 10;
const int gridWidth = 100;
const int gridHeight = 100;
const dim3 threadsPerBlock(16, 16);
const int blockSize = 256;
__constant__ float gap = 0.05f;
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }

inline void gpuAssert(cudaError_t code, const char* file, int line, bool abort = true)
{
	if (code != cudaSuccess)
	{
		fprintf(stderr, "GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
		if (abort) exit(code);
	}
}

#define cudaCheck(x) { cudaError_t err = x; if (err != cudaSuccess) { printf("Cuda error: %d in %s at %s:%d\n", err, #x, __FILE__, __LINE__); assert(0); } }

__global__ void SetGrid(ParticleGrid* grid)
{
	const int index = threadIdx.x + (blockIdx.x * blockDim.x);
	
	float xPos = ((index % gridWidth) * gap);
	float yPos = ((index / gridHeight) * gap);

	grid[index].gridPos.x = xPos;
	grid[index].gridPos.y = yPos;
	grid[index].index = index;
	grid[index].status = Empty;
}

__global__ void SetLand(ParticleGrid* grid, Land* land)
{
	const int index = threadIdx.x + (blockIdx.x * blockDim.x);
	ParticleGrid& currGrid = grid[index];
	int landStartIndex = gridWidth * 5 + 10;
	int landWidth = 50;
	int ogLandStartIndex = landStartIndex;
	if (index > landStartIndex && index < landStartIndex + landWidth)
	{
		currGrid.status = FilledWithLand;
		land[index - ogLandStartIndex].landPos = currGrid.gridPos;
	}

	landStartIndex += gridWidth;
	ogLandStartIndex += landWidth;
	if (index > landStartIndex && index < landStartIndex + landWidth)
	{
		currGrid.status = FilledWithLand;
		land[index - ogLandStartIndex].landPos = currGrid.gridPos;
	}

	landStartIndex += gridWidth;
	ogLandStartIndex += landWidth;

	if (index > landStartIndex && index < landStartIndex + landWidth)
	{
		currGrid.status = FilledWithLand;
		land[index - ogLandStartIndex].landPos = currGrid.gridPos;
	}

	landStartIndex += gridWidth;
	ogLandStartIndex += landWidth;

	if (index > landStartIndex && index < landStartIndex + landWidth)
	{
		currGrid.status = FilledWithLand;
		land[index - ogLandStartIndex].landPos = currGrid.gridPos;
	}

	/*if (isInside)
	{
		
	}*/
}

__global__ void CheckGridPos(ParticleSand* particles, ParticleGrid* grids)
{
	const int index = threadIdx.x + (blockIdx.x * blockDim.x);
	
	printf("index : %d\n", index);

	const int startIndex = gridWidth * 10 + 5;

	const int widthVal = index % width;
	const int heightVal = index / width;
	const int heightInGrid = heightVal * gridWidth;
	

	particles[index].currGrid = &grids[startIndex + heightInGrid + widthVal];
	particles[index].pos = grids[startIndex + heightInGrid + widthVal].gridPos;

}

__global__ void MoveDown(ParticleSand* particles, ParticleGrid* grids, int particleNum)
{
	const int index = threadIdx.x + (blockIdx.x * blockDim.x);
	if (index >= particleNum)
	{
		return;
	}

	ParticleSand& particle = particles[index];
	ParticleGrid& currGrid = *particle.currGrid;

	const int gridIndex = currGrid.index;

	const int downGridIndex = gridIndex - gridWidth;
	const int downDoubleGridIndex = downGridIndex - gridWidth;
	ParticleGrid& destGrid = grids[downGridIndex];
	ParticleGrid& destDoubleGrid = grids[downDoubleGridIndex];
	
	
	if(destGrid.status == Empty)
	{
		particle.pos = destGrid.gridPos;
		particle.currGrid = &destGrid;

		currGrid.status = Empty;
		destGrid.status = FilledWithSand;
	}
	else if(destGrid.status == FilledWithSand && destDoubleGrid.status == FilledWithLand)
	{
		bool isIndexOdd = index % 2;
		int destGridIndex;
		
		if (isIndexOdd)
		{
			destGridIndex = gridIndex - 1;

			if (gridIndex % gridWidth == 0)
			{
				return;
			}
		}
		else
		{
			destGridIndex = gridIndex + 1;

			if (gridIndex % gridWidth == gridWidth - 1)
			{
				return;
			}
		}
		ParticleGrid& newDestGrid = grids[destGridIndex];
		particle.pos = newDestGrid.gridPos;
		particle.currGrid = &newDestGrid;
		currGrid.status = Empty;
		newDestGrid.status = FilledWithSand;
	}
}
void SandUpdate(int particleNum, int gridNum, ParticleSand* particle, ParticleGrid* grid)
{
	int gridCount = particleNum / blockSize;
	MoveDown << <gridCount, blockSize>> > (particle, grid, particleNum);
	gpuErrchk(cudaPeekAtLastError());
}

void Init(int particleNum, int gridNum, ParticleSand* particle, ParticleGrid* grid, Land* land)
{
	int particleGridCount = particleNum / blockSize;
	int gridCount = gridNum / blockSize;

	SetGrid << <gridCount, blockSize >> > (grid);
	SetLand << <gridCount, blockSize >> > (grid, land);
	CheckGridPos << <particleGridCount, blockSize >> > (particle, grid);
}