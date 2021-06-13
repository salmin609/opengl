#include "SandParticle.h"
#include <cuda_runtime.h>
#include "device_launch_parameters.h"
#include <stdio.h>
#include "Affine.h"
#include <atomic>
#include <vector>
const int width = 100;
const int height = 10;
const int gridWidth = 800;
const int gridHeight = 800;
const dim3 threadsPerBlock(16, 16);
const int blockSize = 256;
__constant__ float gap = 0.01f;
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
	
	float xPos = -5 + ((index % gridWidth) * gap);
	float yPos = ((index / gridHeight) * gap);

	grid[index].gridPos.x = xPos;
	grid[index].gridPos.y = yPos;
	grid[index].index = index;
	grid[index].status = Empty;
}

__global__ void SetSpawner(ParticleGrid* grid, SpawnerPos* spawnerPos, int startIndex, int width, int i)
{
	int gridIndex = startIndex + threadIdx.x + (i * gridWidth);
	ParticleGrid& currGrid = grid[gridIndex];
	int iIndex = i * width;

	//currGrid.status = FilledWithLand;
	spawnerPos[iIndex + threadIdx.x].pos = currGrid.gridPos;
	spawnerPos[iIndex + threadIdx.x].currGridIndex = currGrid.index;
}

__global__ void SetLand(ParticleGrid* grid, Land* land, int landStartIndex, int landWidth, int i)
{

	int index = landStartIndex + threadIdx.x;
	int ogLandStartIndex = landStartIndex;
	ParticleGrid& currGrid = grid[index];
	int iIndex = i * landWidth;

	currGrid.status = FilledWithLand;
	land[iIndex + threadIdx.x].landPos = currGrid.gridPos;

}

__global__ void CheckGridPos(ParticleSand* particles, ParticleGrid* grids)
{
	const int index = threadIdx.x + (blockIdx.x * blockDim.x);
	const int startIndex = gridWidth * 500 + 400;

	const int widthVal = index % width;
	const int heightVal = index / width;
	const int heightInGrid = heightVal * gridWidth;
	

	particles[index].currGrid = &grids[startIndex + heightInGrid + widthVal];
	particles[index].pos = grids[startIndex + heightInGrid + widthVal].gridPos;

}

__global__ void SpawnerMove(ParticleGrid* grids, SpawnerPos* spawnerPos, int dir)
{
	const int index = threadIdx.x + (blockIdx.x * blockDim.x);
	const int currGridIndex = spawnerPos[index].currGridIndex;

	//printf("currGrid : %d", currGridIndex);

	const int destGridIndex = currGridIndex + dir;

	SpawnerPos& spawner = spawnerPos[index];

	spawner.currGridIndex = destGridIndex;
	spawner.pos = grids[destGridIndex].gridPos;
}

__global__ void Spawn(ParticleSand* particle, ParticleGrid* grid, SpawnerPos* spawners, int lastIndex)
{
	int index = lastIndex + threadIdx.x;

	ParticleSand& part = particle[index];
	SpawnerPos& spawner = spawners[threadIdx.x];
	ParticleGrid& gridInfo = grid[spawner.currGridIndex];

	part.pos = spawner.pos;
	part.currGrid = &grid[spawner.currGridIndex];
	gridInfo.status = FilledWithSand;
}

__global__ void SpawnLand(Land* lands, ParticleGrid* grid, SpawnerPos* spawners, int lastIndex)
{
	int index = lastIndex + threadIdx.x + (blockIdx.x * blockDim.x);

	Land& land = lands[index];
	SpawnerPos& spawner = spawners[threadIdx.x];
	ParticleGrid& gridInfo = grid[spawner.currGridIndex];

	land.landPos = spawner.pos;
	land.currGridIndex = spawner.currGridIndex;
	gridInfo.status = FilledWithLand;
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
	int gridDownIndex = gridIndex - gridWidth;
	int gridLeftDownIndex = gridIndex - (gridWidth - 1);
	int gridRightDownIndex = gridIndex - (gridWidth + 1);

	if (gridIndex < gridWidth)
	{
		gridDownIndex = gridIndex;
		gridLeftDownIndex = gridIndex;
		gridRightDownIndex = gridIndex;
	}
	if (gridIndex % gridWidth == 0)
	{
		gridLeftDownIndex = gridIndex;
	}
	if (gridIndex % gridWidth == gridWidth - 1)
	{
		gridRightDownIndex = gridIndex;
	}

	if (gridDownIndex <= 0 || gridDownIndex >= 256 * 2000 ||
		gridLeftDownIndex <= 0 || gridLeftDownIndex >= 256 * 2000 ||
		gridRightDownIndex <= 0 || gridRightDownIndex >= 256 * 2000)
	{
		return;
	}

	ParticleGrid& downGrid = grids[gridDownIndex];
	ParticleGrid& leftDownGrid = grids[gridLeftDownIndex];
	ParticleGrid& rightDownGrid = grids[gridRightDownIndex];

	if (downGrid.status == Empty)
	{
		particle.pos = downGrid.gridPos;
		particle.currGrid = &downGrid;

		currGrid.status = Empty;
		downGrid.status = FilledWithSand;
	}
	else if (leftDownGrid.status == Empty)
	{
		particle.pos = leftDownGrid.gridPos;
		particle.currGrid = &leftDownGrid;

		currGrid.status = Empty;
		leftDownGrid.status = FilledWithSand;
	}
	else if (rightDownGrid.status == Empty)
	{
		particle.pos = rightDownGrid.gridPos;
		particle.currGrid = &rightDownGrid;

		currGrid.status = Empty;
		rightDownGrid.status = FilledWithSand;
	}
}
void SandUpdate(int particleNum, int gridNum, ParticleSand* particle, ParticleGrid* grid)
{
	int gridCount = particleNum / blockSize;
	MoveDown << <gridCount, blockSize>> > (particle, grid, particleNum);
	gpuErrchk(cudaPeekAtLastError());
}

void AddSandsInSpawnerPos(ParticleSand* particle, ParticleGrid* grid, SpawnerPos* spawners, int lastIndex)
{
	Spawn << <1, blockSize >> > (particle, grid, spawners, lastIndex);
}
void AddLandsInSpawnerPos(Land* lands, ParticleGrid* grid, SpawnerPos* spawners, int lastIndex)
{
	SpawnLand << <1, blockSize >> > (lands, grid, spawners, lastIndex);
}

void MoveSpawner(ParticleGrid* grid, SpawnerPos* spawners, int dir, int spawnerCount)
{
	SpawnerMove << <1, spawnerCount >> > (grid, spawners, dir);
}



void SetLands(std::vector<int>& landStartRandomIndices, ParticleSand* particle, ParticleGrid* grid, Land* land)
{
	size_t vecSize = landStartRandomIndices.size();
	int landCount = 1;
	int landWidth = 50;
	int landStartIndex = gridWidth * 100 + 200;

	for (size_t i = 0; i < vecSize; ++i)
	{
		SetLand << <landCount, landWidth >> > (grid, land, landStartRandomIndices[i], landWidth, i);
	}

}
__global__ void LoadLand(Land* lands, ParticleGrid* grid, int* indices, int lastIndex)
{
	int index = threadIdx.x + (blockIdx.x * blockDim.x);

	if (index >= lastIndex)
	{
		printf("over");
		return;
	}
	int gridIndex = indices[index];
	lands[index].landPos = grid[gridIndex].gridPos;
	lands[index].currGridIndex = grid[gridIndex].index;
	grid[gridIndex].status = FilledWithLand;
}

void LoadLands(int* landGridIndices, ParticleGrid* grid, Land* land, int landsNum)
{
	int landBlockCount = landsNum / blockSize;

	if (landBlockCount < 0)
		landBlockCount = 1;

	LoadLand << <landBlockCount, blockSize>> > (land, grid, landGridIndices, landsNum);
	gpuErrchk(cudaPeekAtLastError());
}

void Init(int particleNum, int gridNum, int spawnerNum, ParticleSand* particle, ParticleGrid* grid, Land* land, SpawnerPos* spawners)
{
	int particleGridCount = particleNum / blockSize;
	int gridCount = gridNum / blockSize;
	
	int spawnerStartIndex = gridWidth * 300 + 500;

	SetGrid << <gridCount, blockSize >> > (grid);
	CheckGridPos << <particleGridCount, blockSize >> > (particle, grid);

	int spawnerWidth = sqrt(spawnerNum);
	int spwanerCount = 1;

	for (int i = 0; i < spawnerWidth; ++i)
	{
		SetSpawner << <spwanerCount, spawnerWidth >> > (grid, spawners, spawnerStartIndex, spawnerWidth, i);
	}
	gpuErrchk(cudaPeekAtLastError());
}