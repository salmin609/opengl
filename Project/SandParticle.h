#pragma once
#include "Affine.h"


enum GridStatus
{
	Empty,
	FilledWithSand,
	FilledWithLand,
};

struct ParticleGrid
{
	Vector2 gridPos;
	int index;
	GridStatus status;
	int particleIndex;
	int landIndex;
};

struct SpawnerPos 
{
	Vector2 pos;
	int currGridIndex;
};

struct Land 
{
	Vector2 landPos;
	int currGridIndex;
};

struct ParticleSand
{
	Vector2 pos;
	ParticleGrid* currGrid;
};