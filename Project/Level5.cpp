#include "Level5.h"
#include "Graphic.h"
#include "Projection.h"
#include "VAO.h"
#include "Object.h"
#include "RandomNumGenerator.h"
#include "LoadedObj.h"
Level5::Level5()
{
	render = new Shader(shaderHdrBloomSceneVertex.c_str(), shaderHdrBloomSceneFragment.c_str());
	filter = new Shader(shaderHdrBloomFilterVertex.c_str(), shaderHdrBloomFilterFragment.c_str());
	resolve = new Shader(shaderHdrBloomResolveVertex.c_str(), shaderHdrBloomResolveFragment.c_str());

	sphere = new LoadedObj("Sphere");

	static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	static const GLfloat exposureLUT[20] = { 11.0f, 6.0f, 3.2f, 2.8f, 2.2f, 1.90f, 1.80f, 1.80f, 1.70f, 1.70f,  1.60f, 1.60f, 1.50f, 1.50f, 1.40f, 1.40f, 1.30f, 1.20f, 1.10f, 1.00f };
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenFramebuffers(1, &renderFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);
	
	glGenTextures(1, &texScene);
	glBindTexture(GL_TEXTURE_2D, texScene);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texScene, 0);

	glGenTextures(1, &texBrightPass);
	glBindTexture(GL_TEXTURE_2D, texBrightPass);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texBrightPass, 0);

	glGenTextures(1, &texDepth);
	glBindTexture(GL_TEXTURE_2D, texDepth);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texDepth, 0);

	glDrawBuffers(2, buffers);
	
	glGenFramebuffers(2, &filterFbo[0]);
	glGenTextures(2, &texFilter[0]);
	for(int i = 0 ; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, filterFbo[i]);
		glBindTexture(GL_TEXTURE_2D, texFilter[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, i ? MAX_SCENE_WIDTH : MAX_SCENE_HEIGHT, i ? MAX_SCENE_HEIGHT : MAX_SCENE_WIDTH);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texFilter[i], 0);
		glDrawBuffers(1, buffers);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &texLut);
	glBindTexture(GL_TEXTURE_1D, texLut);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 20);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 20, GL_RED, GL_FLOAT, exposureLUT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	//objLoad

	glGenBuffers(1, &uboTransform);
	glBindBuffer(GL_UNIFORM_BUFFER, uboTransform);
	glBufferData(GL_UNIFORM_BUFFER, (2 + SPHERE_COUNT) * sizeof(Matrix), NULL, GL_DYNAMIC_DRAW);

	struct material
	{
		Vector3 diffuse_color;
		unsigned int : 32;
		Vector3 specular_color;
		float specular_power;
		Vector3 ambient_color;
		unsigned int : 32;
	};

	glGenBuffers(1, &uboMaterial);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMaterial);
	glBufferData(GL_UNIFORM_BUFFER, SPHERE_COUNT * sizeof(material), NULL, GL_STATIC_DRAW);

	/*material* m = (material*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, SPHERE_COUNT * sizeof(material), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	*/
	float ambient = 0.002f;
	render->Use();
	
	for (int i = 0; i < SPHERE_COUNT; i++)
	{
		float fi = 3.14159267f * (float)i / 8.0f;
		std::string matStr = "mat[" + std::to_string(i) + "]";
		Vector3 diffuseColor = Vector3(sinf(fi) * 0.5f + 0.5f, sinf(fi + 1.345f) * 0.5f + 0.5f, sinf(fi + 2.567f) * 0.5f + 0.5f);
		Vector3 specularColor = Vector3(2.8f, 2.8f, 2.9f);
		float specularPower = 30.f;
		Vector3 ambientColor = Vector3(ambient * 0.025f);
		render->SendUniformVec3(matStr + ".diffuseColor", &diffuseColor);
		render->SendUniformVec3(matStr + ".specularColor", &specularColor);
		render->SendUniformVec3(matStr + ".ambientColor", &ambientColor);
		render->SendUniformFloat(matStr + ".specularPower", specularPower);
		
		ambient *= 1.1f;
	}
	//glUnmapBuffer(GL_UNIFORM_BUFFER);

	object = new Object(&snubMesh, Point{ 0.f, 0.f, 0.f }, nullptr, nullptr);
	object->Set_Scale(5.f);
	render->Use();
	

	for (int i = 0; i < SPHERE_COUNT; ++i)
	{
		std::string modelMatName = "matModel[" + std::to_string(i) + "]";
		const Vector3 randomVec = RandomNumber::RandomVector3(-5.f, 5.f);
		Matrix model = object->Get_Model_To_World();
		object->SetPosition(Point{ randomVec.x, randomVec.y, randomVec.z });
		render->SendUniformMat(modelMatName, &model);
	}

	//const std::vector<Vertex> vertices = snubMesh.GetTemp();
	const std::vector<Vertex> vertices = sphere->GetVertexDatas();
	objectVao = new VAO(render);
	objectVao->Init(vertices);
	/*glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboTransform);
	struct transforms_t
	{
		Matrix mat_proj;
		Matrix mat_view;
		Matrix mat_model[SPHERE_COUNT];
	} *transforms = (transforms_t*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(transforms_t), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	transforms->mat_proj = CameraToNDC(*CameraManager::instance->GetCamera());
	transforms->mat_proj = transpose(transforms->mat_proj);
	transforms->mat_view = CameraToWorld(*CameraManager::instance->GetCamera());
	transforms->mat_view = transpose(transforms->mat_view);
	for (int i = 0; i < SPHERE_COUNT; ++i)
	{
		//float fi = 3.141592f * (float)i / 16.0f;
		//// float r = cosf(fi * 0.25f) * 0.4f + 1.0f;
		//float r = (i & 2) ? 0.6f : 1.5f;
		Vector3 randomVec = RandomNumber::RandomVector3(-10.f, 10.f);
		//object->SetPosition(Point{ cosf(total_time + fi) * 5.0f * r, sinf(total_time + fi * 4.0f) * 4.0f, sinf(total_time + fi) * 5.f * r });
		object->SetPosition(Point{ randomVec.x, randomVec.y, randomVec.z });
		transforms->mat_model[i] = object->Get_Model_To_World();
		transforms->mat_model[i] = transpose(transforms->mat_model[i]);
	}
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboMaterial);*/

}

Level5::~Level5()
{

}

void Level5::Load()
{
	Graphic::objects.clear();
	Graphic::water = nullptr;
	CameraManager::instance->SetCameraPos(Vector3{ 0.942832f, 1.50537f, -1.57533f }, Vector3{ 0.f, -0.1f, 2.f });
}

void Level5::Update(float dt)
{	
	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const GLfloat one = 1.0f;
	static float total_time = 0.0f;

	total_time += dt;
	glViewport(0, 0, 1280, 768);

	glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_COLOR, 1, black);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	objectVao->Bind();
	//render->Use();
	Matrix ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	//ndcMat = transpose(ndcMat);
	Matrix camMat = CameraToWorld(*CameraManager::instance->GetCamera());
	//camMat = transpose(camMat);

	render->SendUniformMat("matProj", &ndcMat);
	render->SendUniformMat("matView", &camMat);
	
	render->SendUniformFloat("bloomThreshMin", bloomThreshMin);
	render->SendUniformFloat("bloomThreshMax", bloomThreshMax);
	
	//snubMesh.Bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, sphere->FaceCount() * 3,
		SPHERE_COUNT);

	glDisable(GL_DEPTH_TEST);

	filter->Use();
	glBindVertexArray(vao);
	
	glBindFramebuffer(GL_FRAMEBUFFER, filterFbo[0]);
	glBindTexture(GL_TEXTURE_2D, texBrightPass);
	glViewport(0, 0, 1280, 768);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, filterFbo[1]);
	glBindTexture(GL_TEXTURE_2D, texFilter[0]);
	glViewport(0, 0, 1280, 768);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resolve->Use();
	resolve->SendUniformFloat("exposure", 1.f);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texFilter[1]);
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texBrightPass);
	glBindTexture(GL_TEXTURE_2D, texScene);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Level5::UnLoad()
{
}

void Level5::ProgramInit()
{
	
}
