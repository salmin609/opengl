#include "Graphic.h"
#include "Level9.h"
#include "Shader_Table.hpp"
#include "Affine.h"
#include "Shader.h"
#include "Buffer.h"
#include "Client.h"
#include "Projection.h"
#include "VAO.h"
#include "LoadedObj.h"
#include "RandomNumGenerator.h"
#include "Model.h"
Level9::Level9()
{
	fpsAll = 0.f;
	gfsCount = 0;
	particleCountX = 1000;
	particleCountY = 1000;
	totalParticleNum = particleCountX * particleCountY;
	//particleCount = 500000;
	attractorCount = 16;
	render = new Shader(shaderParticleVertex.c_str(), shaderParticleFragment.c_str());
	compute = new Shader(shaderParticleCompute.c_str());

	for(int i = 0 ; i < totalParticleNum; ++i)
	{
		//Vector3 randomPos = RandomNumber::instance->RandomVector3(0.f, 200.f);
		//float randomLife = RandomNumber::instance->RandomFloat(1.f, 10.f);
		//Vector3 randomVel = RandomNumber::instance->RandomVector3(-1.f, 1.f);
		float randLife = static_cast<float>(rand()) / RAND_MAX;
		Vector3 randomPos;
		Vector3 randomVel;
		randomPos.x = static_cast<float>(rand() % 2000) / (500.0f);
		randomPos.y = static_cast<float>(rand() % 2000) / (500.0f);
		randomPos.z = static_cast<float>(rand() % 2000) / (500.0f);

		randomVel.x = static_cast<float>(rand() % 100) / 500.0f - static_cast<float>(rand() % 100) / 500.0f;
		randomVel.y = static_cast<float>(rand() % 100) / 500.0f - static_cast<float>(rand() % 100) / 500.0f;
		randomVel.z = static_cast<float>(rand() % 100) / 500.0f - static_cast<float>(rand() % 100) / 500.0f;
		
		positionDatas.push_back(randomPos);
		velocityDatas.push_back(Vector4{ randomVel.x, randomVel.y, randomVel.z, 0.0f });
		lifeDatas.push_back(randLife);
	}
	
	for(int i = 0 ; i < attractorCount; ++i)
	{
		//Vector3 randomAttr = RandomNumber::instance->RandomVector3(-10.f, 10.f);
		Vector3 vec;
		vec.x = static_cast<float>(rand() % 500) / 30.0f - static_cast<float>(rand() % 500) / 30.0f;
		vec.y = static_cast<float>(rand() % 500) / 30.0f - static_cast<float>(rand() % 500) / 30.0f;
		vec.z = static_cast<float>(rand() % 500) / 30.0f - static_cast<float>(rand() % 500) / 30.0f;
		attratorDatas.push_back(Vector4{ vec.x, vec.y, vec.z, 0.0f });
	}
	
	positionBuffer = new Buffer(GL_ARRAY_BUFFER, totalParticleNum * sizeof(Vector3), GL_DYNAMIC_COPY, positionDatas.data());
	velocityBuffer = new Buffer(GL_ARRAY_BUFFER, totalParticleNum * sizeof(Vector4), GL_DYNAMIC_COPY, velocityDatas.data());
	attratorBuffer = new Buffer(GL_ARRAY_BUFFER, attractorCount * sizeof(Vector4), GL_DYNAMIC_COPY, attratorDatas.data());
	lifeBuffer = new Buffer(GL_ARRAY_BUFFER, totalParticleNum * sizeof(float), GL_DYNAMIC_COPY, lifeDatas.data());
}

Level9::~Level9()
{
	delete render;
	delete compute;
	delete positionBuffer;
	delete velocityBuffer;
	delete attratorBuffer;
	delete lifeBuffer;
}

void Level9::Load()
{
	Graphic::objects.clear();
	Graphic::water = nullptr;
	Graphic::groundId = nullptr;
}

void Level9::Update(float dt)
{
	gfsCount++;

	if(gfsCount >= 1000)
	{
		fpsAll = 0.f;
		gfsCount = 0;
	}
	Matrix ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	Affine camMat = WorldToCamera(*CameraManager::instance->GetCamera());
	
	fpsAll += dt;
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glViewport(0, 0, Client::windowWidth, Client::windowHeight);

	attratorBuffer->Bind();
	
	Vector4* attractor = (Vector4*)glMapBufferRange(GL_ARRAY_BUFFER, 0, attractorCount * sizeof(Vector4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for(int i = 0 ; i < attractorCount; ++i)
	{
		attractor[i].x = sinf(fpsAll) * (float)(rand() % 500) / 10.0f;
		attractor[i].y = cosf(fpsAll) * (float)(rand() % 500) / 10.0f;
		attractor[i].z = tanf(fpsAll);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	compute->Use();
	compute->SendUniformFloat("dt", dt);
	positionBuffer->BindStorage(0);
	velocityBuffer->BindStorage(1);
	attratorBuffer->BindStorage(2);
	lifeBuffer->BindStorage(3);

	glDispatchCompute(particleCountX / 32, particleCountY / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glUseProgram(0);

	render->Use();
	render->SendUniformMat("viewMatrix", &camMat);
	render->SendUniformMat("projMatrix", &ndcMat);

	positionBuffer->Bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	lifeBuffer->Bind();
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINTS, 0, totalParticleNum);
	glUseProgram(0);
}

void Level9::UnLoad()
{
}
