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

struct ParticleVec4
{
	Vector4 velocity;
	Vector4 force;
	Vector4 predictedPos;
};

struct ParticleVal
{
	float density;
	float lambda;
	int id;
};

const int neighborCount = 100;

struct Neighbors
{
	int neighbor[neighborCount];
};

typedef struct WaterParticle WaterParticle;
typedef struct WaterParticle* pWaterParticle;

struct Indices
{
	int ix, iy, iz;
};

struct Bubble {
	int type;
	float radius;
	Vector4 pos;
	Vector4 vel;
	float lifetime;
};

enum BubbleType
{
	None = -1,
	Spray = 0,
	Bubble = 1,
	Foam = 2
};

struct BubbleVec4
{
	Vector4 bubblePos;
	Vector4 bubbleVel;
};