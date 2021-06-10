#include "Level11.h"
#include <iostream>
#include <GL/glew.h>
#include "Shader_Table.hpp"
#include "Shader.h"
#include "vs2017/Test.cuh"
#include <cuda_runtime.h>
#include "Buffer.hpp"
#include "Client.h"
#include "Graphic.h"
#include "Projection.h"

Level11::Level11()
{
	render = new Shader(shader2DVertex.c_str(), shader2DFragment.c_str());
	
	sandParticles = new ParticleSand[NUMPARTICLES];
	sandGrids = new ParticleGrid[NUMGRIDS];
	lands = new Land[NUMLANDS];
	
	memset(sandParticles, 0, particleMemSize);
	memset(sandGrids, 0, gridMemSize);
	memset(lands, 0, landMemSize);

	cudaMalloc(&dSandParticles, particleMemSize);
	cudaMalloc(&dSandGrids, gridMemSize);
	cudaMalloc(&dLands, landMemSize);
	
	CopyFromHostToDevice();
	Init(NUMPARTICLES, NUMGRIDS, dSandParticles, dSandGrids, dLands);
	CopyFromDeviceToHost();
	
	sandPosBuffer = new Buffer(GL_ARRAY_BUFFER, particleMemSize, GL_STATIC_DRAW, sandParticles);
	gridPosBuffer = new Buffer(GL_ARRAY_BUFFER, gridMemSize, GL_STATIC_DRAW, sandGrids);
	landPosBuffer = new Buffer(GL_ARRAY_BUFFER, landMemSize, GL_STATIC_DRAW, lands);
}

Level11::~Level11()
{
	delete render;
	delete sandPosBuffer;
	delete[] sandParticles;
	cudaFree(dSandParticles);
}

void Level11::Load()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	//glClearColor(1.f, 1.f, 1.f, 1.f);
	Graphic::objects.clear();
}

void Level11::Update(float dt)
{
	(dt);

	const Camera cam = *CameraManager::instance->GetCamera();
	Matrix ndcMat = CameraToNDC(cam);
	Affine camMat = WorldToCamera(cam);
	Matrix vp = ndcMat * camMat;
	Point camEye = cam.Eye();
	Vector4 camEyeVec4 = Vector4{ camEye.x, camEye.y, camEye.z, 1.f };

	timer -= dt;

	if (timer < 0.f)
	{
		SandUpdate(NUMPARTICLES, NUMGRIDS, dSandParticles, dSandGrids);
		CopyFromDeviceToHost();
		sandPosBuffer->WriteData<ParticleSand>(sandParticles);

		timer = 0.05f;
	}

	sandPosBuffer->Check<ParticleSand>();
	
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glViewport(0, 0, Client::windowWidth, Client::windowHeight);
	
	Vector3 colorWhite(1.f);
	Vector3 colorYellow(0.7686274510f, 0.5686274510f, 0.0078431373f);
	Vector3 colorGreen(0.f, 0.5f, 0.0f);
	Vector3 colorRed(0.5f, 0.0f, 0.0f);

	render->Use();
	render->SendUniform3fv("color_val", &colorYellow, 1);
	render->SendUniformMat("VP", &vp);

	sandPosBuffer->Bind();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleSand), (GLvoid*)offsetof(ParticleSand, pos));
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, NUMPARTICLES);

	gridPosBuffer->Bind();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleGrid), (GLvoid*)offsetof(ParticleGrid, gridPos));
	glEnableVertexAttribArray(0);

	render->SendUniform3fv("color_val", &colorRed, 1);
	
	glDrawArrays(GL_POINTS, 0, NUMGRIDS);

	landPosBuffer->Bind();
	landPosBuffer->Check<Land>();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offsetof(Land, landPos));
	glEnableVertexAttribArray(0);

	render->SendUniform3fv("color_val", &colorGreen, 1);

	glDrawArrays(GL_POINTS, 0, NUMLANDS);
}

void Level11::UnLoad()
{
}

void Level11::CopyFromDeviceToHost()
{
	cudaMemcpy(sandParticles, dSandParticles, particleMemSize, cudaMemcpyDeviceToHost);
	cudaMemcpy(sandGrids, dSandGrids, gridMemSize, cudaMemcpyDeviceToHost);
	cudaMemcpy(lands , dLands, landMemSize, cudaMemcpyDeviceToHost);
}

void Level11::CopyFromHostToDevice()
{
	cudaMemcpy(dSandParticles, sandParticles, particleMemSize, cudaMemcpyHostToDevice);
	cudaMemcpy(dSandGrids, sandGrids, gridMemSize, cudaMemcpyHostToDevice);
	cudaMemcpy(dLands, lands, landMemSize, cudaMemcpyHostToDevice);
}
