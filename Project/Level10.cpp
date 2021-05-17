#include "Level10.h"

#include <iostream>
#include <GL/glew.h>
#include "Buffer.h"
#include "Camera.h"
#include "CameraManager.h"
#include "Client.h"
#include "FluidCompute.h"
#include "InputManager.h"
#include "Projection.h"
#include "RandomNumGenerator.h"
#include "Shader.h"
#include "Shader_Table.hpp"
Level10::Level10()
{
	fluid = new FluidCompute();
	render = new Shader(shaderFluidVertex.c_str(), shaderFluidFragment.c_str());
	compute = new Shader(shaderFluidCompute.c_str());
	computeNeighbor = new Shader(shaderFluidComputeNeighbor.c_str());
	pxNum = fluid->PxNum();
	pyNum = fluid->PyNum();
	pzNum = fluid->PzNum();
	pTotalNum = pxNum * pyNum * pzNum;

	WaterParticle* particles = fluid->Particles();

	std::vector<Vector3> particlePosVec = fluid->ParticlePos();
	Neighbors neighbor;
	for(int i = 0; i < neighborCount; ++i)
	{
		neighbor.neighbor[i] = -1;
	}
	for(int i = 0; i < pTotalNum; ++i)
	{
		colors.push_back(Vector3(0.831f, 0.945f, 0.976f));
		radii.push_back(0.008f);
		Vector3 posVal = particles[i].pos;
		Vector3 velVal = particles[i].vel;
		Vector3 forceVal = particles[i].force;
		int idVal = particles[i].id;
		float densityVal = particles[i].density;
		float lambdaVal = particles[i].lambda;
		
		pos.emplace_back(posVal.x, posVal.y, posVal.z, 1.f);
		vel.emplace_back(velVal.x, velVal.y, velVal.z, 1.f);
		force.emplace_back(forceVal.x, forceVal.y, forceVal.z, 1.f);
		ids.push_back(idVal);
		density.push_back(densityVal);
		lambda.push_back(lambdaVal);
		predictedPos.emplace_back(particlePosVec[i].x, particlePosVec[i].y, particlePosVec[i].z, 1.f);
		neighbors.push_back(neighbor);
		neighborsCheckCount.push_back(0);
	}
	
	compute->Use();
	particlePos = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * pTotalNum, GL_DYNAMIC_DRAW, pos.data());
	particlePos->BindStorage(0);
	
	particleId = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(int) * pTotalNum, GL_DYNAMIC_DRAW, ids.data());
	particleId->BindStorage(1);
	
	particleDensity = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(float) * pTotalNum, GL_DYNAMIC_DRAW, density.data());
	particleDensity->BindStorage(2);

	particleLambda = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(float) * pTotalNum, GL_DYNAMIC_DRAW, lambda.data());
	particleLambda->BindStorage(3);

	particleVel = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * pTotalNum, GL_DYNAMIC_DRAW, vel.data());
	particleVel->BindStorage(4);

	particleForce = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * pTotalNum, GL_DYNAMIC_DRAW, force.data());
	particleForce->BindStorage(5);

	particlePredictedPos = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * pTotalNum, GL_DYNAMIC_DRAW, predictedPos.data());
	particlePredictedPos->BindStorage(6);

	particleNeighbors = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Neighbors) * pTotalNum, GL_DYNAMIC_DRAW, neighbors.data());
	particleNeighbors->BindStorage(7);

	particleNeighborsCheckCount = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(int) * pTotalNum, GL_DYNAMIC_DRAW, neighborsCheckCount.data());
	
	computeNeighbor->Use();
	particlePos->BindStorage(0);
	particleNeighbors->BindStorage(1);
	particleNeighborsCheckCount->BindStorage(2);
	
	vertexBuffer = new Buffer(GL_ARRAY_BUFFER, sizeof(Vector3) * pTotalNum, GL_STATIC_DRAW, pos.data());
	colorBuffer = new Buffer(GL_ARRAY_BUFFER, sizeof(Vector3) * pTotalNum, GL_STATIC_DRAW, colors.data());
	radiiBuffer = new Buffer(GL_ARRAY_BUFFER, sizeof(float) * pTotalNum, GL_STATIC_DRAW, radii.data());
}

Level10::~Level10()
{
	delete fluid;
	delete vertexBuffer;
	delete colorBuffer;
	delete radiiBuffer;

	delete render;
	delete compute;
	delete computeNeighbor;

	delete particlePos;
	delete particleForce;
	delete particleVel;
	delete particleId;
	delete particleDensity;
	delete particleLambda;
	delete particlePredictedPos;
	delete particleNeighbors;
}

void Level10::Load()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	CameraManager::instance->SetCameraPos(Vector3{ 0.992733f, 0.126221f, 1.1071f }, Vector3{ -1.f, -0.1f, -1.f });
	//glPointSize(10.f);
}

void Level10::Update(float dt)
{
	(dt);
	//fluid->Update();
	glEnable(GL_PROGRAM_POINT_SIZE);

	computeNeighbor->Use();
	glDispatchCompute((pTotalNum / 128), 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	if(InputManager::instance->IsPressed('q'))
	{
		particleNeighborsCheckCount->BindStorage(2);
		int* neighborCheckCount = reinterpret_cast<int*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(int),
			GL_MAP_READ_BIT));
		std::vector<int> checkCount;
		for (int i = 0; i < pTotalNum; ++i)
		{
			checkCount.push_back(neighborCheckCount[i]);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	compute->Use();
	glDispatchCompute((pTotalNum / 128), 1 , 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	const Camera cam = *CameraManager::instance->GetCamera();
	Matrix ndcMat = CameraToNDC(cam);
	Affine camMat = WorldToCamera(cam);
	Matrix mvp = ndcMat * camMat;
	
	
	static Vector3  lightDir = Vector3{ 1.f, 0.f, 0.f };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, Client::windowWidth, Client::windowHeight);
	
	render->Use();
	render->SendUniformMat("MVP", &mvp);
	render->SendUniform3fv("lightDir", &lightDir, 1);

	particlePos->Bind();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector4), (void*)0);
	glEnableVertexAttribArray(0);

	colorBuffer->Bind();
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	
	radiiBuffer->Bind();
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_POINTS, 0, pTotalNum);

	particleNeighbors->BindStorage(1);
	Neighbors* neighbor = reinterpret_cast<Neighbors*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(Neighbors),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	for (int i = 0; i < pTotalNum; ++i)
	{
		for(int j = 0 ; j < neighborCount; ++j)
		{
			neighbor[i].neighbor[j] = -1;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	particleNeighborsCheckCount->BindStorage(2);
	int* neighborCheckCount = reinterpret_cast<int*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(int),
		GL_MAP_READ_BIT));
	std::vector<int> checkCount1;
	for (int i = 0; i < pTotalNum; ++i)
	{
		checkCount1.push_back(neighborCheckCount[i]);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	

	particleNeighborsCheckCount->BindStorage(2);
	int* neighborCountCheck = reinterpret_cast<int*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(int),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	for (int i = 0; i < pTotalNum; ++i)
	{
		neighborCountCheck[i] = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

}

void Level10::UnLoad()
{
}
