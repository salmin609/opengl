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
	std::vector<Vector3> check = fluid->ParticlePos();
	for(int i = 0; i < pTotalNum; ++i)
	{
		colors.push_back(Vector3(1.0, 1.0, 1.0));
		radii.push_back(0.002f);
	}
	particleBuffer = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(WaterParticle) * pTotalNum, GL_DYNAMIC_DRAW, particles);
	particleBuffer->BindStorage(0);
	
	vertexBuffer = new Buffer(GL_ARRAY_BUFFER, sizeof(Vector3) * pTotalNum, GL_STATIC_DRAW, check.data());
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
		particleBuffer->Bind();
		particleBuffer->BindStorage(0);
		//particleBuffer->BindStorageBuffer();
		WaterParticle* ptrChekc = reinterpret_cast<WaterParticle*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, pTotalNum * sizeof(WaterParticle), GL_MAP_READ_BIT));
		std::vector<WaterParticle> checkVal;
		for (int i = 0; i < pTotalNum; i++)
		{
			if(ptrChekc[i].id == 0 && i != 0)
			{
				int a = 0;
				(a);
			}
			checkVal.push_back(ptrChekc[i]);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
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
	particleBuffer->Bind();
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WaterParticle), (GLvoid*)offsetof(WaterParticle, pos));
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
