#include "Level10.h"

#include <iostream>
#include <GL/glew.h>
#include "Buffer.hpp"
#include "Camera.h"
#include "CameraManager.h"
#include "Client.h"
#include "FluidCompute.h"
#include "InputManager.h"
#include "Projection.h"
#include "RandomNumGenerator.h"
#include "Shader.h"
#include "Shader_Table.hpp"
#include "TgaTexture.h"
Level10::Level10()
{
	fluid = new FluidCompute();
	//render = new Shader(shaderFluidVertex.c_str(), shaderFluidFragment.c_str(), shaderFluidGeometry.c_str());
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
		colors.push_back(Vector3(0.f, 0.3764705882f, 1.f));
		radii.push_back(pointSize);
		Vector3 posVal = particles[i].pos;
		Vector3 velVal = particles[i].vel;
		Vector3 forceVal = particles[i].force;
		Vector3 particlePosVal = particlePosVec[i];
		int idVal = particles[i].id;
		float densityVal = particles[i].density;
		float lambdaVal = particles[i].lambda;

		ParticleVec4 vec4Val;
		vec4Val.force = Vector4{ forceVal.x, forceVal.y, forceVal.z, 1.f };
		vec4Val.velocity = Vector4{ velVal.x, velVal.y, velVal.z, 1.f };
		vec4Val.predictedPos = Vector4{ particlePosVal.x, particlePosVal.y, particlePosVal.z, 1.f };

		ParticleVal particleValVal;
		particleValVal.id = idVal;
		particleValVal.lambda = lambdaVal;
		particleValVal.density = densityVal;

		BubbleVec4 bubbleVec4Val;
		bubbleVec4Val.bubblePos = Vector4{ 0.f, 0.f, 0.f, 1.f };
		bubbleVec4Val.bubbleVel  = Vector4{ 0.f, 0.f, 0.f, 0.f };

		particleVec4Vals.push_back(vec4Val);
		particleVals.push_back(particleValVal);
		
		pos.emplace_back(posVal.x, posVal.y, posVal.z, 1.f);
		vel.emplace_back(velVal.x, velVal.y, velVal.z, 1.f);
		force.emplace_back(forceVal.x, forceVal.y, forceVal.z, 1.f);
		ids.push_back(idVal);
		density.push_back(densityVal);
		lambda.push_back(lambdaVal);
		predictedPos.emplace_back(particlePosVec[i].x, particlePosVec[i].y, particlePosVec[i].z, 1.f);
		neighbors.push_back(neighbor);
		neighborsCheckCount.push_back(0);

		bubbleTypes.push_back(BubbleType::None);
		//bubblePoses.push_back(Vector4(0.f, 0.f, 0.f, 1.f));
		//bubbleVelocities.push_back(Vector4(0.f, 0.f, 0.f, 0.f));
		bubbleVec4Vals.push_back(bubbleVec4Val);
		bubbleRadiuses.push_back(0.f);
		bubbleLifetimes.push_back(0.f);
	}
	
	compute->Use();


	
	particleValVec4Buffer = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleVec4) * pTotalNum, GL_DYNAMIC_DRAW, particleVec4Vals.data());
	particleValVec4Buffer->BindStorage(0);

	particleValBuffer = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleVal) * pTotalNum, GL_DYNAMIC_DRAW, particleVals.data());
	particleValBuffer->BindStorage(1);

	/*particlePos = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * pTotalNum, GL_DYNAMIC_DRAW, pos.data());
	particlePos->BindStorage(0);
	
	particleId = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(int) * pTotalNum, GL_DYNAMIC_DRAW, ids.data());
	particleId->BindStorage(1);
	
	particleDensity = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(float) * pTotalNum, GL_DYNAMIC_DRAW, density.data());
	particleDensity->BindStorage(2);

	particleLambda = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(float) * pTotalNum, GL_DYNAMIC_DRAW, lambda.data());
	particleLambda->BindStorage(3);

	/*particleVel = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * pTotalNum, GL_DYNAMIC_DRAW, vel.data());
	particleVel->BindStorage(4);

	particleForce = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * pTotalNum, GL_DYNAMIC_DRAW, force.data());
	particleForce->BindStorage(5);

	particlePredictedPos = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * pTotalNum, GL_DYNAMIC_DRAW, predictedPos.data());
	particlePredictedPos->BindStorage(6);*/

	particleNeighbors = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Neighbors) * pTotalNum, GL_DYNAMIC_DRAW, neighbors.data());
	particleNeighbors->BindStorage(2);

	bubbleType = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(int) * pTotalNum, GL_DYNAMIC_DRAW, bubbleTypes.data());
	bubbleType->BindStorage(3);

	bubbleRadius = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(float) * pTotalNum, GL_DYNAMIC_DRAW, bubbleRadiuses.data());
	bubbleRadius->BindStorage(4);

	bubbleVec4 = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(BubbleVec4) * pTotalNum, GL_DYNAMIC_DRAW, bubbleVec4Vals.data());
	bubbleVec4->BindStorage(5);

	bubbleLifetime = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(float) * pTotalNum, GL_DYNAMIC_DRAW, bubbleLifetimes.data());
	bubbleLifetime->BindStorage(6);

	particlePos = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * pTotalNum, GL_DYNAMIC_DRAW, pos.data());
	particlePos->BindStorage(7);

	particleNeighborsCheckCount = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(int) * pTotalNum, GL_DYNAMIC_DRAW, neighborsCheckCount.data());
	
	computeNeighbor->Use();
	particlePos->BindStorage(0);
	particleNeighbors->BindStorage(1);
	particleNeighborsCheckCount->BindStorage(2);
	
	vertexBuffer = new Buffer(GL_ARRAY_BUFFER, sizeof(Vector3) * pTotalNum, GL_STATIC_DRAW, nullptr);
	colorBuffer = new Buffer(GL_ARRAY_BUFFER, sizeof(Vector3) * pTotalNum, GL_STATIC_DRAW, colors.data());
	radiiBuffer = new Buffer(GL_ARRAY_BUFFER, sizeof(float) * pTotalNum, GL_STATIC_DRAW, radii.data());
	colorBufferBubble = new Buffer(GL_ARRAY_BUFFER, sizeof(Vector3) * pTotalNum, GL_STATIC_DRAW, colors.data());
	radiiBufferBubble = new Buffer(GL_ARRAY_BUFFER, sizeof(float) * pTotalNum, GL_STATIC_DRAW, radii.data());

	tgaTexture = new TgaTexture("Particle.tga");
	tgaTexture->Use(render->GetShaderId());
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

	delete particleValVec4Buffer;
	delete particleValBuffer;
	/*delete particlePos;
	delete particleForce;
	delete particleVel;
	delete particleId;
	delete particleDensity;
	delete particleLambda;
	delete particlePredictedPos;
	delete particleNeighbors;*/
}

void Level10::Load()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	CameraManager::instance->SetCameraPos(Vector3{ 0.13937f, 0.0234417f, 1.64168f }, Vector3{ 0.f, -0.2f, -1.f });
	glPointSize(10.f);
}


void Level10::Update(float dt)
{
	//glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	
	fpsTimer += dt;
	gfsCount++;

	if (gfsCount > 10000)
	{
		gfsCount = 0;
		fpsTimer = 0.f;
	}
	//fluid->Update();
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	computeNeighbor->Use();
	particlePos->BindStorage(0);
	particleNeighbors->BindStorage(1);
	particleNeighborsCheckCount->BindStorage(2);

	glDispatchCompute((pTotalNum / 128) + 1, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	compute->Use();
	
	particleValVec4Buffer->BindStorage(0);
	particleValBuffer->BindStorage(1);
	particleNeighbors->BindStorage(2);
	bubbleType->BindStorage(3);
	bubbleRadius->BindStorage(4);
	bubbleVec4->BindStorage(5);
	bubbleLifetime->BindStorage(6);
	particlePos->BindStorage(7);

	glDispatchCompute((pTotalNum / 128) + 1, 1 , 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	const Camera cam = *CameraManager::instance->GetCamera();
	Matrix ndcMat = CameraToNDC(cam);
	Affine camMat = WorldToCamera(cam);
	Matrix mvp = ndcMat * camMat;
	Point camEye = cam.Eye();
	Vector4 camEyeVec4 = Vector4{ camEye.x, camEye.y, camEye.z, 1.f };
	static Vector3  lightDir = Vector3{ 1.f, 0.f, 0.f };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, Client::windowWidth, Client::windowHeight);

	render->Use();

	render->SendUniformMat("MVP", &mvp);
	render->SendUniform3fv("lightdir", &lightDir, 1);

	particlePos->Bind();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector4), (void*)0);
	glEnableVertexAttribArray(0);

	colorBuffer->Check<Vector3>();
	
	colorBuffer->Bind();
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	radiiBuffer->Bind();
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_POINTS, 0, pTotalNum);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(2);
	
	bubbleType->BindStorage();
	int* BubbleTypeCheck = reinterpret_cast<int*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(int),
		GL_MAP_READ_BIT));
	for (int i = 0; i < pTotalNum; ++i)
	{
		if(BubbleTypeCheck[i] != BubbleType::None)
		{
			if(BubbleTypeCheck[i] == BubbleType::Bubble)
			{
				bubbleGeneratedColor.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.f));
			}
			else if (BubbleTypeCheck[i] == BubbleType::Foam)
			{
				bubbleGeneratedColor.push_back(Vector4(1.f, 1.f, 1.f, 1.f));
			}
			else if (BubbleTypeCheck[i] == BubbleType::Spray)
			{
				bubbleGeneratedColor.push_back(Vector4(0.831f, 0.945f, 0.976f, 1.f));
			}
			bubbleTypeCheck.push_back(i);
			bubbleGeneratedRadius.push_back(bubbleSize);
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	size_t typeSize = bubbleTypeCheck.size();

	bubbleVec4->BindStorage();
	BubbleVec4* bubblePosCheck = reinterpret_cast<BubbleVec4*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(BubbleVec4),
		GL_MAP_READ_BIT));
	
	for(size_t i = 0 ; i < typeSize; ++i)
	{
		const int index = bubbleTypeCheck[i];
		bubbleGeneratedPos.push_back(bubblePosCheck[index].bubblePos);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	
	radiiBuffer->Check<float>();
	
	if(typeSize > 0)
	{
		vertexBuffer->Bind();
		Vector3* data = reinterpret_cast<Vector3*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, typeSize * sizeof(Vector3),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

		for(size_t i = 0 ; i < typeSize; ++i)
		{
			const Vector4 posVal = bubbleGeneratedPos[i];
			data[i] = Vector3(posVal.x, posVal.y, posVal.z);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		vertexBuffer->Check<Vector3>();

		colorBufferBubble->Bind();
		Vector3* dataColor = reinterpret_cast<Vector3*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, typeSize * sizeof(Vector3),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

		for (size_t i = 0; i < typeSize; ++i)
		{
			const Vector4 colorVal = bubbleGeneratedColor[i];
			dataColor[i] = Vector3(colorVal.x, colorVal.y, colorVal.z);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		colorBufferBubble->Check<Vector3>();

		radiiBufferBubble->Bind();
		float* dataRadii = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, typeSize * sizeof(float),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

		for (size_t i = 0; i < typeSize; ++i)
		{
			const float radiiVal = bubbleGeneratedRadius[i];
			dataRadii[i] = radiiVal;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		radiiBufferBubble->Check<float>();

		render->Use();

		render->SendUniformMat("MVP", &mvp);
		render->SendUniform3fv("lightdir", &lightDir, 1);
		
		glEnableVertexAttribArray(0);
		vertexBuffer->Bind();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		colorBufferBubble->Bind();
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(2);
		radiiBufferBubble->Bind();
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
		const int size = static_cast<int>(typeSize);
		glDrawArrays(GL_POINTS, 0, size);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		//glDisableVertexAttribArray(2);
		
	}

	

	
	particleNeighbors->BindStorage();
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
	
	particleNeighborsCheckCount->BindStorage();
	int* neighborCountCheck = reinterpret_cast<int*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pTotalNum * sizeof(int),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	for (int i = 0; i < pTotalNum; ++i)
	{
		neighborCountCheck[i] = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	bubbleTypeCheck.clear();
	bubbleGeneratedColor.clear();
	bubbleGeneratedPos.clear();
	bubbleGeneratedRadius.clear();
	//bubbleTypeCheck.reserve(pTotalNum);
}

void Level10::UnLoad()
{
}

