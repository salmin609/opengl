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
};

struct Land 
{
	Vector2 landPos;
};

struct ParticleSand
{
	Vector2 pos;
	ParticleGrid* currGrid;
};