#pragma once
#include "Affine.h"

struct WaterParticle
{
	int id;
	Vector3 pos;
	Vector3 vel;
	Vector3 force;
	float density;
	float lambda;
};
typedef struct WaterParticle WaterParticle;
typedef struct WaterParticle* pWaterParticle;

struct Indices
{
	int ix, iy, iz;
};