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
#include "PositionConverter.h"
#include "RandomNumGenerator.h"
#include "InputManager.h"
#include <stdlib.h>
#include "BinaryFileManager.h"

Level11::Level11()
{
	render = new Shader(shader2DVertex.c_str(), shader2DFragment.c_str());
	renderOnePoint = new Shader(shaderOnePointVertex.c_str(), shader2DFragment.c_str());
	
	sandParticles = new ParticleSand[NUMPARTICLES];
	sandGrids = new ParticleGrid[NUMGRIDS];
	lands = new Land[NUMLANDS];
	spawnerPos = new SpawnerPos[NUMSPAWNERS];
	
	std::vector<int> landIndices;

	for (int i = 0; i < 10; ++i)
	{
		int randomNum = RandomNumber::instance->RandomInt(312000);
		landIndices.push_back(randomNum);
	}

	memset(sandParticles, 0, particleMemSize);
	memset(sandGrids, 0, gridMemSize);
	memset(lands, 0, landMemSize);
	memset(spawnerPos, 0, spawnerMemSize);

	cudaMalloc(&dSandParticles, particleMemSize);
	cudaMalloc(&dSandGrids, gridMemSize);
	cudaMalloc(&dLands, landMemSize);
	cudaMalloc(&dSpawnerPos, spawnerMemSize);
	
	CopyFromHostToDevice();
	Init(NUMPARTICLES, NUMGRIDS, NUMSPAWNERS, dSandParticles, dSandGrids, dLands, dSpawnerPos);
	//SetLands(landIndices, dSandParticles, dSandGrids, dLands);
	CopyFromDeviceToHost();
	
	sandPosBuffer = new Buffer(GL_ARRAY_BUFFER, particleMemSize, GL_STATIC_DRAW, sandParticles);
	gridPosBuffer = new Buffer(GL_ARRAY_BUFFER, gridMemSize, GL_STATIC_DRAW, sandGrids);
	landPosBuffer = new Buffer(GL_ARRAY_BUFFER, landMemSize, GL_STATIC_DRAW, nullptr);
	spawnerPosBuffer = new Buffer(GL_ARRAY_BUFFER, spawnerMemSize, GL_STATIC_DRAW, spawnerPos);
}

Level11::~Level11()
{
	delete render;
	delete sandPosBuffer;
	delete gridPosBuffer;
	delete landPosBuffer;
	delete[] sandParticles;
	delete[] sandGrids;
	delete[] lands;
	cudaFree(dSandParticles);
	cudaFree(dSandGrids);
	cudaFree(dLands);
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

	if (InputManager::instance->IsPressed('z'))
	{
		//spawn
		cudaMemcpy(sandParticles, dSandParticles, particleMemSize, cudaMemcpyDeviceToHost);
		cudaFree(dSandParticles);

		int lastIndex = NUMPARTICLES;

		NUMPARTICLES += NUMSPAWNERS;
		particleMemSize = NUMPARTICLES * sizeof(ParticleSand);

		sandParticles = (ParticleSand*)realloc(sandParticles, particleMemSize);

		cudaMalloc(&dSandParticles, particleMemSize);
		cudaMemcpy(dSandParticles, sandParticles, particleMemSize, cudaMemcpyHostToDevice);

		AddSandsInSpawnerPos(dSandParticles, dSandGrids, dSpawnerPos, lastIndex);

		cudaMemcpy(sandParticles, dSandParticles, particleMemSize, cudaMemcpyDeviceToHost);

		delete sandPosBuffer;
		sandPosBuffer = new Buffer(GL_ARRAY_BUFFER, particleMemSize, GL_STATIC_DRAW, sandParticles);
	}
	if (InputManager::instance->IsPressed('x'))
	{
		//spawn
		cudaMemcpy(lands, dLands, landMemSize, cudaMemcpyDeviceToHost);
		cudaFree(dLands);

		int lastIndex = NUMLANDS;

		NUMLANDS += NUMSPAWNERS;
		landMemSize = NUMLANDS * sizeof(Land);

		lands = (Land*)realloc(lands, landMemSize);

		cudaMalloc(&dLands, landMemSize);
		cudaMemcpy(dLands, lands, landMemSize, cudaMemcpyHostToDevice);

		//AddSandsInSpawnerPos(dSandParticles, dSandGrids, dSpawnerPos, lastIndex);
		AddLandsInSpawnerPos(dLands, dSandGrids, dSpawnerPos, lastIndex);
		cudaMemcpy(lands, dLands, landMemSize, cudaMemcpyDeviceToHost);

		delete landPosBuffer;
		landPosBuffer = new Buffer(GL_ARRAY_BUFFER, landMemSize, GL_STATIC_DRAW, lands);
	}
	if (InputManager::instance->IsPressed('c'))
	{
		cudaMemcpy(lands, dLands, landMemSize, cudaMemcpyDeviceToHost);
		std::vector<Vertex> vertexDatas;
		for (int i = 0; i < NUMLANDS; ++i)
		{
			Vertex vertex;
			vertex.position.x = static_cast<float>(lands[i].currGridIndex);
			vertexDatas.push_back(vertex);
		}
		BinaryFileManager::SaveBinFile("sand", vertexDatas, false, false);

	}
	if (InputManager::instance->IsPressed('v'))
	{
		std::vector<Vertex> vertexDatas;
		int check = 0;
		BinaryFileManager::LoadDataFile("sand", vertexDatas, check);

		cudaFree(dLands);
		delete[] lands;
		delete landPosBuffer;

		NUMLANDS = static_cast<int>(vertexDatas.size());

		int size = NUMLANDS * sizeof(int);
		loadedLands = new int[NUMLANDS];
		cudaMalloc(&dLoadedLands, size);

		for (int i = 0; i < NUMLANDS; ++i)
		{
			loadedLands[i] = static_cast<int>(vertexDatas[i].position.x);
		}
		cudaMemcpy(dLoadedLands, loadedLands, size, cudaMemcpyHostToDevice);

		landMemSize = NUMLANDS * sizeof(Land);
		lands = (Land*)malloc(landMemSize);
		memset(lands, 0, landMemSize);
		cudaMalloc(&dLands, landMemSize);

		cudaMemcpy(dLands, lands, landMemSize, cudaMemcpyHostToDevice);

		LoadLands(dLoadedLands, dSandGrids, dLands, NUMLANDS);
		cudaMemcpy(lands, dLands, landMemSize, cudaMemcpyDeviceToHost);
		landPosBuffer = new Buffer(GL_ARRAY_BUFFER, landMemSize, GL_STATIC_DRAW, lands);
		landPosBuffer->Check<Land>();
	}

	int speed = 16;
	if (InputManager::instance->IsPressed('j'))
	{
		//left
		MoveSpawner(dSandGrids, dSpawnerPos, -1 * speed, NUMSPAWNERS);
	}
	if (InputManager::instance->IsPressed('l'))
	{
		//right
		MoveSpawner(dSandGrids, dSpawnerPos, 1 * speed, NUMSPAWNERS);
	}
	if (InputManager::instance->IsPressed('k'))
	{
		//up
		MoveSpawner(dSandGrids, dSpawnerPos, -800 * speed, NUMSPAWNERS);
	}
	if (InputManager::instance->IsPressed('i'))
	{
		//down
		MoveSpawner(dSandGrids, dSpawnerPos, 800 * speed, NUMSPAWNERS);
	}

	SandUpdate(NUMPARTICLES, NUMGRIDS, dSandParticles, dSandGrids);
	CopyFromDeviceToHost();
	sandPosBuffer->WriteData<ParticleSand>(sandParticles);
	spawnerPosBuffer->WriteData<SpawnerPos>(spawnerPos);

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
	render->SendUniformFloat("pointSize", 1.0f);
	render->SendUniform3fv("color_val", &colorYellow, 1);
	render->SendUniformMat("VP", &vp);

	sandPosBuffer->Bind();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleSand), (GLvoid*)offsetof(ParticleSand, pos));
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, NUMPARTICLES);

	spawnerPosBuffer->Bind();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpawnerPos), (GLvoid*)offsetof(SpawnerPos, pos));
	glEnableVertexAttribArray(0);

	render->SendUniform3fv("color_val", &colorRed, 1);
	
	glDrawArrays(GL_POINTS, 0, NUMSPAWNERS);

	landPosBuffer->Bind();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Land), (GLvoid*)offsetof(Land, landPos));
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
	cudaMemcpy(spawnerPos, dSpawnerPos, spawnerMemSize, cudaMemcpyDeviceToHost);
}

void Level11::CopyFromHostToDevice()
{
	cudaMemcpy(dSandParticles, sandParticles, particleMemSize, cudaMemcpyHostToDevice);
	cudaMemcpy(dSandGrids, sandGrids, gridMemSize, cudaMemcpyHostToDevice);
	cudaMemcpy(dLands, lands, landMemSize, cudaMemcpyHostToDevice);
	cudaMemcpy(dSpawnerPos, spawnerPos, spawnerMemSize, cudaMemcpyHostToDevice);
}
