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
	pxNum = fluid->PxNum();
	pyNum = fluid->PyNum();
	pTotalNum = pxNum * pyNum;

	WaterParticle* particles = fluid->Particles();

	std::vector<Vector3> particlePosVec = fluid->ParticlePos();
	Neighbors neighbor;
	for(int i = 0; i < 50; ++i)
	{
		neighbor.neighbor[i] = -1;
	}
	for(int i = 0; i < pTotalNum; ++i)
	{
		colors.push_back(Vector3(1.0, 1.0, 1.0));
		radii.push_back(0.002f);
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
	}
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

	compute->Use();
	glDispatchCompute((pTotalNum / 32), 1 , 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	const Camera cam = *CameraManager::instance->GetCamera();
	Matrix ndcMat = CameraToNDC(cam);
	Affine camMat = WorldToCamera(cam);
	Matrix mvp = ndcMat * camMat;
	
	if (InputManager::instance->IsPressed('a'))
	{
		particlePos->BindStorage(0);
		Vector4* ptrChekc = reinterpret_cast<Vector4*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(Vector4), GL_MAP_READ_BIT));
		std::vector<Vector4> checkVal;
		for (int i = 0; i < pTotalNum; i++)
		{
			checkVal.push_back(ptrChekc[i]);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		particleId->BindStorage(1);
		int* idCheck = reinterpret_cast<int*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(int), GL_MAP_READ_BIT));
		std::vector<int> checkId;
		for (int i = 0; i < pTotalNum; i++)
		{
			checkId.push_back(idCheck[i]);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		particleDensity->BindStorage(2);
		float* denCheck = reinterpret_cast<float*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(float), GL_MAP_READ_BIT));
		std::vector<float> checkDen;
		for (int i = 0; i < pTotalNum; i++)
		{
			checkDen.push_back(denCheck[i]);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		particleLambda->BindStorage(3);
		float* lamCheck = reinterpret_cast<float*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(float), GL_MAP_READ_BIT));
		std::vector<float> checkLam;
		for (int i = 0; i < pTotalNum; i++)
		{
			checkLam.push_back(lamCheck[i]);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		particleVel->BindStorage(4);
		Vector4* velCheck = reinterpret_cast<Vector4*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(Vector4), GL_MAP_READ_BIT));
		std::vector<Vector4> checkVel;
		for (int i = 0; i < pTotalNum; i++)
		{
			checkVel.push_back(velCheck[i]);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		particleForce->BindStorage(5);
		Vector4* forceCheck = reinterpret_cast<Vector4*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(Vector4), GL_MAP_READ_BIT));
		std::vector<Vector4> checkForce;
		for (int i = 0; i < pTotalNum; i++)
		{
			checkForce.push_back(forceCheck[i]);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		particlePredictedPos->BindStorage(6);
		Vector4* predictedCheck = reinterpret_cast<Vector4*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(Vector4), GL_MAP_READ_BIT));
		std::vector<Vector4> checkPredicted;
		for (int i = 0; i < pTotalNum; i++)
		{
			checkPredicted.push_back(predictedCheck[i]);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		particleNeighbors->BindStorage(7);
		Neighbors* neighborsCheck = reinterpret_cast<Neighbors*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(Neighbors), GL_MAP_READ_BIT));
		std::vector<Neighbors> checkNeighbor;
		for (int i = 0; i < pTotalNum; i++)
		{
			checkNeighbor.push_back(neighborsCheck[i]);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	}
	


	static Vector3  lightDir = Vector3{ 1.f, 0.f, 0.f };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, Client::windowWidth, Client::windowHeight);
	
	render->Use();
	render->SendUniformMat("MVP", &mvp);
	render->SendUniform3fv("lightDir", &lightDir, 1);

	/*std::vector<Vector3> particlePos = fluid->ParticlePos();
	vertexBuffer->Bind();
	Vector3* ptr = reinterpret_cast<Vector3*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, pTotalNum * sizeof(Vector3), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	for (int i = 0; i < pTotalNum; i++)
	{
		const Vector3 vec = particlePos[i];
		ptr[i].x = vec.x;
		ptr[i].y = vec.y;
		ptr[i].z = vec.z;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);*/

	
	
	//vertexBuffer->Bind();
	particlePos->Bind();
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WaterParticle), (GLvoid*)offsetof(WaterParticle, pos));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector4), (void*)0);
	glEnableVertexAttribArray(0);

	colorBuffer->Bind();
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	
	radiiBuffer->Bind();
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_POINTS, 0, pTotalNum);
}

void Level10::UnLoad()
{
}
