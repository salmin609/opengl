#include "Graphic.h"
#include "Level9.h"

#include <memory>

#include "Shader_Table.hpp"
#include "Affine.h"
#include "Shader.h"
#include "Buffer.h"
#include "Client.h"
#include "InputManager.h"
#include "PositionConverter.h"
#include "Projection.h"
#include "VAO.h"
#include "RandomNumGenerator.h"
Level9::Level9()
{
	attrator = Vector4{ 0.f, 0.f, 0.f, 1.f };
	gfsCount = 0;
	fpsTimer = 0.f;
	
	particleCountX = 1000000;
	particleCountY = 1;
	totalParticleNum = particleCountX * particleCountY;
	attractorCount = 16;
	
	render = new Shader(shaderParticleVertex.c_str(), shaderParticleFragment.c_str(), shaderParticleGeometry.c_str());
	compute = new Shader(shaderParticleCompute.c_str());

	const auto particles = std::make_unique<Particle[]>(totalParticleNum);

	for(int i = 0; i < totalParticleNum; ++i)
	{
		const Vector3 randomPos = RandomNumber::instance->RandomVector3(-10.f, 10.f);
		const Vector4 randomPosInVector4{ randomPos.x, randomPos.y, randomPos.z ,1.f};
		particles[i].currPos = randomPosInVector4;
		particles[i].prevPos = randomPosInVector4;
	}
	compute->Use();
	particleBuffer = new Buffer(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * totalParticleNum, GL_DYNAMIC_DRAW, particles.get());
	particleBuffer->BindStorage(0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer->GetId());
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer->GetId());
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)0);
	glBindVertexArray(0);
}

Level9::~Level9()
{
	delete render;
	delete compute;
	delete particleBuffer;
	glDeleteVertexArrays(1, &vao);
}

void Level9::Load()
{
	Graphic::objects.clear();
	Graphic::water = nullptr;
	Graphic::groundId = nullptr;
}

void Level9::Update(float dt)
{
	fpsTimer += dt;
	gfsCount++;

	if(gfsCount > 1000)
	{
		gfsCount = 0;
		fpsTimer = 0.f;
	}
	
	const Camera cam = *CameraManager::instance->GetCamera();
	Matrix ndcMat = CameraToNDC(cam);
	Affine camMat = WorldToCamera(cam);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glViewport(0, 0, Client::windowWidth, Client::windowHeight);

	if(InputManager::instance->IsPressed('q'))
	{
		isActive = !isActive;
	}
	
	//int newx = 0, newy = 0;
	//SDL_GetMouseState(&newx, &newy);
	//const Point curr = PositionConverter::GetMousePosInWorldCoord(static_cast<float>(newx), static_cast<float>(newy));
	//Vector3 goal{ curr.x, curr.y, -10.f};
	////Vector3 goal{ 0.f, 0.f, 0.f };
	//Vector4 goalVec4{ goal.x, goal.y, goal.z, 1.f };
	attrator.x = cosf(fpsTimer) * static_cast<float>(rand() % 100);
	attrator.y = sinf(fpsTimer) * static_cast<float>(rand() % 100);
	attrator.z = tanf(fpsTimer);
	
	compute->Use();
	//particleBuffer->BindStorage(0);
	compute->SendUniformInt("maxParticles", totalParticleNum);

	Vector4 attractor;

	isActive ? attractor = Vector4{ attrator.x, attrator.y, attrator.z, 1.f } : attractor = Vector4{ attrator.x, attrator.y, attrator.z, -1.f };
	
	compute->SendUniform4fv("attPos", &attractor, 1);
	compute->SendUniformFloat("dt", dt);

	glDispatchCompute((totalParticleNum / 256) + 1, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glUseProgram(0);
	Point camEye = cam.Eye();
	Vector4 camEyeVec4 = Vector4{ camEye.x, camEye.y, camEye.z, 1.f };
	render->Use();
	glBindVertexArray(vao);
	
	render->SendUniformMat("viewMatrix", &camMat);
	render->SendUniformMat("projMatrix", &ndcMat);
	render->SendUniform4fv("camPos", &camEyeVec4, 1);
	render->SendUniformFloat("quadLength", 0.01f);
	render->SendUniformFloat("time", fpsTimer);
	
	glDrawArrays(GL_POINTS, 0, totalParticleNum);
}

void Level9::UnLoad()
{
}
