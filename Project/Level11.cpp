#include "Level11.h"
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


Level11::Level11()
{
	render = new Shader(shaderFluidVertex2.c_str(), shaderFluidFragment2.c_str());
	compute = new Shader(shaderFluidCompute2.c_str());
	boxRender = new Shader(shader_simple_vertex.c_str(), shader_simple_fragment.c_str());

	std::vector positions = {
		// YZ left
		Vector4(-halfx, -halfy, -halfz, 1.0f), // 0
		Vector4(-halfx, halfy, -halfz, 1.0f),  // 2
		Vector4(-halfx, -halfy, halfz, 1.0f),  // 1

		Vector4(-halfx, -halfy, halfz, 1.0f),  // 1
		Vector4(-halfx, halfy, -halfz, 1.0f),  // 2
		Vector4(-halfx, halfy, halfz, 1.0f),   // 3

		// YZ right
		Vector4(halfx, -halfy, -halfz, 1.0f), // 0
		Vector4(halfx, -halfy, halfz, 1.0f),  // 1
		Vector4(halfx, halfy, -halfz, 1.0f),  // 2

		Vector4(halfx, -halfy, halfz, 1.0f),  // 1
		Vector4(halfx, halfy, halfz, 1.0f),   // 3
		Vector4(halfx, halfy, -halfz, 1.0f),  // 2

		// XZ bottom
		Vector4(-halfx, -halfy, -halfz, 1.0f), // 0
		Vector4(-halfx, -halfy, halfz, 1.0f),  // 1
		Vector4(halfx, -halfy, -halfz, 1.0f),  // 2

		Vector4(-halfx, -halfy, halfz, 1.0f),  // 1
		Vector4(halfx, -halfy, halfz, 1.0f),   // 3
		Vector4(halfx, -halfy, -halfz, 1.0f),  // 2

		// XZ top
		Vector4(-halfx, halfy, -halfz, 1.0f), // 0
		Vector4(halfx, halfy, -halfz, 1.0f),  // 2
		Vector4(-halfx, halfy, halfz, 1.0f),  // 1

		Vector4(-halfx, halfy, halfz, 1.0f),  // 1
		Vector4(halfx, halfy, -halfz, 1.0f),  // 2
		Vector4(halfx, halfy, halfz, 1.0f),   // 3

		// XY near
		Vector4(-halfx, -halfy, -halfz, 1.0f), // 0
		Vector4(halfx, -halfy, -halfz, 1.0f),  // 2
		Vector4(-halfx, halfy, -halfz, 1.0f),  // 1

		Vector4(-halfx, halfy, -halfz, 1.0f),  // 1
		Vector4(halfx, -halfy, -halfz, 1.0f),  // 2
		Vector4(halfx, halfy, -halfz, 1.0f),   // 3

		// XZ far
		Vector4(-halfx, -halfy, halfz, 1.0f), // 0
		Vector4(-halfx, halfy, halfz, 1.0f),  // 1
		Vector4(halfx, -halfy, halfz, 1.0f),  // 2

		Vector4(-halfx, halfy, halfz, 1.0f),  // 1
		Vector4(halfx, halfy, halfz, 1.0f),   // 3
		Vector4(halfx, -halfy, halfz, 1.0f),  // 2
	};

	std::vector normals = {
		// YZ left
		Vector4(-1.0f, 0.0f, 0.0f, 0.0f),
		Vector4(-1.0f, 0.0f, 0.0f, 0.0f),
		Vector4(-1.0f, 0.0f, 0.0f, 0.0f),
		Vector4(-1.0f, 0.0f, 0.0f, 0.0f),
		Vector4(-1.0f, 0.0f, 0.0f, 0.0f),
		Vector4(-1.0f, 0.0f, 0.0f, 0.0f),

		// YZ right
		Vector4(1.0f, 0.0f, 0.0f, 0.0f),
		Vector4(1.0f, 0.0f, 0.0f, 0.0f),
		Vector4(1.0f, 0.0f, 0.0f, 0.0f),
		Vector4(1.0f, 0.0f, 0.0f, 0.0f),
		Vector4(1.0f, 0.0f, 0.0f, 0.0f),
		Vector4(1.0f, 0.0f, 0.0f, 0.0f),

		// XZ bottom
		Vector4(0.0f, -1.0f, 0.0f, 0.0f),
		Vector4(0.0f, -1.0f, 0.0f, 0.0f),
		Vector4(0.0f, -1.0f, 0.0f, 0.0f),
		Vector4(0.0f, -1.0f, 0.0f, 0.0f),
		Vector4(0.0f, -1.0f, 0.0f, 0.0f),
		Vector4(0.0f, -1.0f, 0.0f, 0.0f),

		// XZ top
		Vector4(0.0f, 1.0f, 0.0f, 0.0f),
		Vector4(0.0f, 1.0f, 0.0f, 0.0f),
		Vector4(0.0f, 1.0f, 0.0f, 0.0f),
		Vector4(0.0f, 1.0f, 0.0f, 0.0f),
		Vector4(0.0f, 1.0f, 0.0f, 0.0f),
		Vector4(0.0f, 1.0f, 0.0f, 0.0f),

		// XY near
		Vector4(0.0f, 0.0f, -1.0f, 0.0f),
		Vector4(0.0f, 0.0f, -1.0f, 0.0f),
		Vector4(0.0f, 0.0f, -1.0f, 0.0f),
		Vector4(0.0f, 0.0f, -1.0f, 0.0f),
		Vector4(0.0f, 0.0f, -1.0f, 0.0f),
		Vector4(0.0f, 0.0f, -1.0f, 0.0f),

		// XY far
		Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4(0.0f, 0.0f, 1.0f, 0.0f),
	};
	
	CheckIndices indices{0,0,0,0};
	Vector4 vel = Vector4{};
	Vector4 force = Vector4(0.f, -9.8f, 0.f, 0.f);
	Vector4 prePos = Vector4{};
	int id = 0;
	
	Neighbors neighbor;
	for(int i = 0; i < neighborCount; ++i)
	{
		neighbor.neighbor[i] = -1;
	}

	int halfX = pxNum / 2;
	int halfY = pyNum / 2;
	int halfZ = pzNum / 2;

	for(int i = 0; i < binCount; ++i)
	{
		particleNeighbors.push_back(neighbor);
	}
	
	for(int i = -halfX; i < halfX; ++i)
	{
		for(int j = -halfY; j < halfY; ++j)
		{
			for(int k = -halfZ; k < halfZ; ++k)
			{
				Vector4 pos = Vector4(float(i) * pDist, float(j) * pDist, float(k) * pDist, 1.f);
				
				ParticleVec42 val;
				val.pos = pos;
				val.velocity = vel;
				val.force = force;
				val.predictedPos = prePos;

				ParticleVal valVal;
				valVal.lambda = 0.f;
				valVal.density = 0.f;
				valVal.id = id;
				id++;
				
				particleVec4s.push_back(val);
				particleVals.push_back(valVal);
				particlePoses.push_back(val.pos);
				particleDensities.push_back(valVal.density);
				particleIndices.push_back(indices);
			}
		}
	}
	
	compute->Use();
	particleVec4Buffer = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleVec42) * pTotalNum, GL_DYNAMIC_DRAW, particleVec4s.data());
	particleVec4Buffer->BindStorage(0);
	particleValBuffer = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleVal) * pTotalNum, GL_DYNAMIC_DRAW, particleVals.data());
	particleValBuffer->BindStorage(1);
	particleNeighborBuffer = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Neighbors) * binCount, GL_DYNAMIC_DRAW, particleNeighbors.data());
	particleNeighborBuffer->BindStorage(2);

	particlePosBuffer = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * pTotalNum, GL_DYNAMIC_DRAW, particlePoses.data());
	particlePosBuffer->BindStorage(3);

	particleIndicesBuffer = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(CheckIndices) * pTotalNum, GL_DYNAMIC_DRAW, particleIndices.data());
	particleIndicesBuffer->BindStorage(4);

	particleDensityBuffer = new Buffer(GL_ARRAY_BUFFER, sizeof(float) * pTotalNum, GL_DYNAMIC_DRAW, particleDensities.data());

	boxPositionBuffer = new Buffer(GL_ARRAY_BUFFER, sizeof(Vector4) * (unsigned)positions.size(), GL_STATIC_DRAW, positions.data());
	boxNormalBuffer = new Buffer(GL_ARRAY_BUFFER, sizeof(Vector4) * (unsigned)normals.size(), GL_STATIC_DRAW, normals.data());
	
}

Level11::~Level11()
{
	delete particleVec4Buffer;
	delete particleValBuffer;
	delete particleNeighborBuffer;
	delete compute;
	delete render;
}

void Level11::Load()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glClearColor(1.f, 1.f, 1.f, 1.f);

}

void Level11::Update(float dt)
{
	(dt);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BACK);
	SetCamVariables();

	compute->Use();
	particleVec4Buffer->BindStorage(0);
	particleValBuffer->BindStorage(1);
	particleNeighborBuffer->BindStorage(2);
	particlePosBuffer->BindStorage(3);
	particleIndicesBuffer->BindStorage(4);

	//particleValBuffer->Check<ParticleVal>();
	glDispatchCompute((pTotalNum / 128) + 1, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	//particleValBuffer->Check<ParticleVal>();
	
	//particleNeighborBuffer->Check<Neighbors>();
	//particleIndicesBuffer->Check<CheckIndices>();
	
	render->Use();
	render->SendUniformMat("MVP", &mvp);
	render->SendUniformMat("MV", &camMat);
	render->SendUniformFloat("pointRadius", 0.5f);
	render->SendUniformFloat("pointScale" , 1.f);

	particlePosBuffer->Bind();
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	particleDensityBuffer->Bind();
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	
	//glPointSize(0.5f);
	glDrawArrays(GL_POINTS, 0, pTotalNum);

	Vector3 white = Vector3(0.2f, 0.2f, 0.2f);
	boxRender->Use();
	boxRender->SendUniformVec3("color_val", &white);
	boxRender->SendUniformMat("to_ndc", &ndcMat);
	boxRender->SendUniformMat("cam", &camMat);

	boxPositionBuffer->Bind();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector4), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Level11::UnLoad()
{
}

void Level11::SetCamVariables()
{
	const Camera cam = *CameraManager::instance->GetCamera();
	ndcMat = CameraToNDC(cam);
	camMat = WorldToCamera(cam);
	mvp = ndcMat * camMat;
	camEye = cam.Eye();
	Vector4 camEyeVec4 = Vector4{ camEye.x, camEye.y, camEye.z, 1.f };
	static Vector3  lightDir = Vector3{ 1.f, 0.f, 0.f };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, Client::windowWidth, Client::windowHeight);
}
