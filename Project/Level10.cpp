#include "Level10.h"
#include <GL/glew.h>
#include "Buffer.h"
#include "Camera.h"
#include "CameraManager.h"
#include "Client.h"
#include "FluidCompute.h"
#include "Projection.h"
#include "RandomNumGenerator.h"
#include "Shader.h"
#include "Shader_Table.hpp"
Level10::Level10()
{
	fluid = new FluidCompute();
	render = new Shader(shaderFluidVertex.c_str(), shaderFluidFragment.c_str());
	pTotalNum = fluid->ParticleTotalNum();

	std::vector<Vector3> check = fluid->ParticlePos();
	for(int i = 0; i < pTotalNum; ++i)
	{
		colors.push_back(Vector3(1.0, 1.0, 1.0));
		radii.push_back(0.002f);
	}
	
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
	//glPointSize(10.f);
}

void Level10::Update(float dt)
{
	fluid->Update();
	(dt);
	glEnable(GL_PROGRAM_POINT_SIZE);
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

	std::vector<Vector3> particlePos = fluid->ParticlePos();
	vertexBuffer->Bind();
	Vector3* ptr = reinterpret_cast<Vector3*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, pTotalNum * sizeof(Vector3), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	for (int i = 0; i < pTotalNum; i++)
	{
		const Vector3 vec = particlePos[i];
		ptr[i].x = vec.x;
		ptr[i].y = vec.y;
		ptr[i].z = vec.z;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	vertexBuffer->Bind();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
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
