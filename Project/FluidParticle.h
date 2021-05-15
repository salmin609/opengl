#pragma once
#include "Affine.h"

struct WaterParticle
{
	int id;
	float density;
	float lambda;
	Vector3 pos;
	unsigned int : 32;
	Vector3 vel;
	unsigned int : 32;
	Vector3 force;
	unsigned int : 32;
};
typedef struct WaterParticle WaterParticle;
typedef struct WaterParticle* pWaterParticle;

struct Indices
{
	int ix, iy, iz;
};