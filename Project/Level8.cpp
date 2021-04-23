#include "Level8.h"
#include "Graphic.h"
#include "Buffer.h"
#include "Projection.h"

Level8::Level8()
{
	//particleCount = 1024 * 1024 * 2;
	m_NumWorkGroups[0] = 1024 / 32;
	m_NumWorkGroups[1] = 1024 * 2 / 32;
	m_NumWorkGroups[2] = 1;
	particleCount = 1024 * 1024 * 2;
	for (int i = 0; i < sphereCount; ++i)
	{
		spheres.radii[i] = 100.0f + fmod((float)rand(), 50.0f);
		spheres.centers[i].x = -700.0f + fmod((float)rand(), 1400.0f);
		spheres.centers[i].y = 0.0f;
		spheres.centers[i].z = -700.0f + fmod((float)rand(), 1400.0f);
	}

	particlePositions = new ParticlePos[particleCount];
	particleVelocities = new ParticleVelocity[particleCount];

	memset(particleVelocities, 0, particleCount * sizeof(ParticleVelocity));
	
	static const double cubeSize = 1400.0;
	
	for(int i = 0; i < particleCount; ++i)
	{
		particlePositions[i].x = static_cast<float>(-cubeSize / 2.0 + fmod((float)rand(), cubeSize));
		particlePositions[i].y = static_cast<float>(cubeSize / 3.0 + fmod((float)rand(), cubeSize));
		particlePositions[i].z = static_cast<float>(-cubeSize / 2.0 + fmod((float)rand(), cubeSize));

		particleVelocities[i].vx = -2.5f + rand() % 5 + (rand() % 10) / 10.0f;
		particleVelocities[i].vz = -2.5f + rand() % 5 + (rand() % 10) / 10.0f;
		particleVelocities[i].vy = -5.f + rand() % 10 + (rand() % 10) / 10.0f;
		particleVelocities[i].fTimeToLive = 20.f + fmod((float)rand(), 10.f);

		particlePositions[i].w = 1.0;
	}
	
	renderShader = new Shader(shaderParticleCollisionVertex.c_str(), 
		shaderParticleCollisionFragment.c_str(), 
		shaderParticleCollisionGeometry.c_str());

	computeShader = new Shader(shaderParticleCollisionCompute.c_str());

	for(int i = 0; i < 2; ++i)
	{
		positionBuffer[i] = new Buffer(GL_SHADER_STORAGE_BUFFER, particleCount * sizeof(ParticlePos), GL_STATIC_DRAW, particlePositions);
		velocityBuffer[i] = new Buffer(GL_SHADER_STORAGE_BUFFER, particleCount * sizeof(ParticleVelocity), GL_STATIC_DRAW, particleVelocities);
	}

	glGenVertexArrays(2, drawVao);

	glBindVertexArray(drawVao[0]);
	positionBuffer[0]->Bind();
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(drawVao[1]);
	positionBuffer[1]->Bind();
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

Level8::~Level8()
{
	delete renderShader;
	delete computeShader;
}

void Level8::Load()
{
	Graphic::objects.clear();
	Graphic::water = nullptr;
	Graphic::groundId = nullptr;
	CameraManager::instance->SetCameraPos(Vector3{ 0.942832f, 100.50537f, -1.57533f }, Vector3{ 0.f, -0.1f, 2.f });
}

void Level8::Update(float dt)
{
	computeShader->Use();
	computeShader->SendUniformFloat("dt", dt);
	computeShader->SendUniform3fv("sphereOffsets", &spheres.centers, sphereCount);
	computeShader->SendUniform1fv("sphereRadius", &spheres.radii, sphereCount);
	computeShader->SendUniformInt("gNumParticles", particleCount);

	positionBuffer[!index]->BindStorageBuffer(0, particleCount * sizeof(ParticlePos));
	velocityBuffer[!index]->BindStorageBuffer(1, particleCount * sizeof(ParticleVelocity));
	positionBuffer[index]->BindStorageBuffer(2, particleCount * sizeof(ParticlePos));
	velocityBuffer[index]->BindStorageBuffer(3, particleCount * sizeof(ParticleVelocity));

	glDispatchCompute(m_NumWorkGroups[0], m_NumWorkGroups[1], m_NumWorkGroups[2]);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glUseProgram(0);

	glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderShader->Use();
	Matrix ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	Matrix camMat = WorldToCamera(*CameraManager::instance->GetCamera());

	renderShader->SendUniformMat("viewMat", &camMat);
	renderShader->SendUniformMat("projMat", &ndcMat);

	index = !index;

	glBindVertexArray(drawVao[index]);
	glDrawArrays(GL_POINTS, 0, particleCount);
	
	glBindVertexArray(0);
	glUseProgram(0);
}

void Level8::UnLoad()
{
}
