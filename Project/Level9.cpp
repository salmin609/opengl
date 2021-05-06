#include "Graphic.h"
#include "Level9.h"
#include "Shader_Table.hpp"
#include "Affine.h"
#include "Shader.h"
#include "Buffer.h"
#include "Projection.h"
#include "VAO.h"
#include "LoadedObj.h"
#include "RandomNumGenerator.h"
#include "Model.h"
Level9::Level9()
{
	
	const int numX = 32;
	const int numY = 32;
	numworkGroups[0] = numX / 32;
	numworkGroups[1] = numY * 2 / 32;
	numworkGroups[2] = 1;
	particleCount = numX * numY * 2;

	positionBufferSize = sizeof(ParticlePos) * particleCount;
	velocityBufferSize = sizeof(ParticleVelocity) * particleCount;
	
	particlePositions = new ParticlePos[particleCount];
	particleVelocity = new ParticleVelocity[particleCount];
	loadedObj = new LoadedObj("Sphere");
	model = new Model(0.f, 2.5f);
	std::vector<Vertex> vertices = loadedObj->GetVertexDatas();
	
	
	for(int i = 0; i < particleCount; ++i)
	{
		Vector3 randomPos = RandomNumber::instance->RandomVector3(-10.f, 10.f);
		Vector3 randomVel = RandomNumber::instance->RandomVector3(-10.f, 10.f);
		particlePositions[i].x = randomPos.x;
		particlePositions[i].y = 100.f;
		particlePositions[i].z = randomPos.z;

		particleVelocity[i].vx = randomVel.x;
		particleVelocity[i].vy = randomVel.y;
		particleVelocity[i].vz = randomVel.z;
		particleVelocity[i].fTimeToLive = 10.f;
		
		particlePositions[i].w = 1.0f;
	}
	renderShader = new Shader(shader_simple_vertex.c_str(), shader_simple_fragment.c_str());
	computeShader = new Shader(shaderWaterCollisionCompute.c_str());

	for(int i = 0 ; i < 2; ++i)
	{
		positionBuffer[i] = new Buffer(GL_SHADER_STORAGE_BUFFER, positionBufferSize, GL_STATIC_DRAW, particlePositions);
		velocityBuffer[i] = new Buffer(GL_SHADER_STORAGE_BUFFER, velocityBufferSize, GL_STATIC_DRAW, particleVelocity);
		drawVao[i] = new VAO(renderShader);
		drawVao[i]->Init(vertices, true);
	}
	drawVao[0]->Bind();
	positionBuffer[0]->Bind();
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	drawVao[1]->Bind();
	positionBuffer[1]->Bind();
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	
	glVertexAttribDivisor(1, 1);
	glBindVertexArray(0);
}

Level9::~Level9()
{
	
}

void Level9::Load()
{
	Graphic::objects.clear();
	Graphic::water = nullptr;
	Graphic::groundId = nullptr;
}

void Level9::Update(float dt)
{
	(dt);
	/*computeShader->Use();
	computeShader->SendUniformFloat("dt", dt);
	computeShader->SendUniformInt("numParticles", particleCount);

	positionBuffer[index]->BindStorageBuffer(0, positionBufferSize);
	velocityBuffer[index]->BindStorageBuffer(1, velocityBufferSize);
	positionBuffer[!index]->BindStorageBuffer(2, positionBufferSize);
	velocityBuffer[!index]->BindStorageBuffer(3, velocityBufferSize);

	glDispatchCompute(numworkGroups[0], numworkGroups[1], numworkGroups[2]);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glUseProgram(0);*/

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderShader->Use();
	Matrix ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	Matrix camMat = WorldToCamera(*CameraManager::instance->GetCamera());

	renderShader->SendUniformMat("cam", &camMat);
	renderShader->SendUniformMat("to_ndc", &ndcMat);

	index = !index;
	drawVao[index]->Bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, loadedObj->FaceCount() * 3, particleCount);
	
	glBindVertexArray(0);
	glUseProgram(0);
}

void Level9::UnLoad()
{
}
