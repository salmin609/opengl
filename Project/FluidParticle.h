#pragma once
#include "Affine.h"

struct WaterParticle
{
	int id;
	float density;
	float lambda;
	Vector3 pos;
	Vector3 vel;
	Vector3 force;
};

struct Neighbors
{
	int neighbor[50];
};

typedef struct WaterParticle WaterParticle;
typedef struct WaterParticle* pWaterParticle;


struct Indices
{
	int ix, iy, iz;
};