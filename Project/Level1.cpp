#include "Level1.h"
#include "Texture.h"
#include "Client.h"
#include "FrameBuffer.h"
#include "Graphic.h"
#include "Projection.h"
#include "KtxFileLoader.h"

const Vector    WHITE(1, 1, 1),
                PURPLE(1, 0, 1),
                BLACK(0, 0, 0),
                RED(1, 0, 0),
                GREEN(0, 1, 0),
                SKY(0.313725490f, 0.73725490196f, 0.8745098039f),
                HOT_PINK(1.f, 0.00784313725f, 0.55294117647f),
                YELLOW(1.f, 0.83137254901f, 0.f);

Level1::Level1()
{
	deerObj = new LoadedObj("Deer");
	quadObj = new LoadedObj("LowpolyTree");
	Initialize_Material();
	Init_Objects();

	groundShader = new Shader(shaderGroundVertex.c_str(), shaderGroundFragment.c_str(), shaderGroundTesselationControl.c_str(),
		shaderGroundTesselationEvaluation.c_str());
	groundShader->Use();
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	groundShader->SendUniformInt("tex_displacement", 0);
	check = new Texture("height2.png");
	//glEnable(GL_CULL_FACE);
	//terrianDisplacement = KtxFileLoader::load("terragen1.ktx");
	glActiveTexture(GL_TEXTURE1);
	terrianColor = KtxFileLoader::load("terragen_color.ktx");

	Level1::Load();
}

void Level1::Load()
{
	Graphic::objects.clear();
	//Graphic::objects.push_back(center_circle);
	//Graphic::objects.push_back(light);
	//Graphic::objects.push_back(right_circle);
	//Graphic::objects.push_back(deer);
	//Graphic::objects.push_back(tree);
	
	Graphic::light = light;
	//Graphic::groundId = groundShader;
	Graphic::water = nullptr;
}

void Level1::Update(float dt)
{
	(dt);
	glEnable(GL_CULL_FACE);
	static const GLfloat black[] = { 0.85f, 0.95f, 1.0f, 1.0f };
	static const GLfloat one = 1.0f;
	
	glViewport(0, 0, Client::windowWidth, Client::windowHeight);
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_DEPTH, 0, &one);
	
	groundShader->Use();
	Matrix ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	Matrix camMat = WorldToCamera(*CameraManager::instance->GetCamera());
	Matrix mvp = ndcMat * camMat;
	groundShader->SendUniformMat("mv_matrix", &camMat);
	groundShader->SendUniformMat("proj_matrix", &ndcMat);
	groundShader->SendUniformMat("mvpMat", &mvp);
	groundShader->SendUniformFloat("dmap_depth", 3.f);
	check->Bind(0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArraysInstanced(GL_PATCHES, 0, 4, 200 * 200);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
}

void Level1::UnLoad()
{
}

Level1::~Level1()
{
	delete deerObj;
	delete quadObj;
	delete center_circle;
	delete right_circle;
	delete light;
	delete ground;
	delete deer;
	delete tree;
}

void Level1::Init_Objects()
{
	center_circle = new Object(&snub_mesh, Point(0.f, 1.f, 0.f), &emerald);
	right_circle = new Object(&snub_mesh, Point(2.f, 1.f, 0.f), &obsidian);
	light = new Object(&light_mesh, Point{ 1.f, 3.f, 0.f }, &pearl, WHITE);
	ground = new Object(&cube_mesh, Point(0.f, 0.f, 0.f), &emerald);
	deer = new Object(deerObj, Point(0.f, 1.f, 0.f), nullptr);
	tree = new Object(quadObj, Point(2.f, 0.f, 0.f), &emerald);
	const Vector scale{ 0.35f, 0.35f, 0.35f };
	center_circle->Set_Scale(scale);
	right_circle->Set_Scale(scale);
	light->Set_Scale(scale);
	ground->Set_Scale(Vector(10.f, -0.1f, 10.f));
}

void Level1::Initialize_Material()
{
	emerald.ambient = Vector(0.0215f, 0.1745f, 0.0215f);
	emerald.diffuse = Vector(0.07568f, 0.61424f, 0.07568f);
	emerald.specular = Vector(0.633f, 0.727811f, 0.633f);
	emerald.shiness = 0.6f;

	obsidian.ambient = Vector(0.05375f, 0.05f, 0.06625f);
	obsidian.diffuse = Vector(0.18275f, 0.17f, 0.22525f);
	obsidian.specular = Vector(0.332741f, 0.328634f, 0.346435f);
	obsidian.shiness = 0.3f;

	jade.ambient = Vector(0.135f, 0.2225f, 0.1575f);
	jade.diffuse = Vector(0.54f, 0.89f, 0.63f);
	jade.specular = Vector(0.316228f, 0.316228f, 0.316228f);
	jade.shiness = 0.1f;

	pearl.ambient = Vector(0.25f, 0.20725f, 0.20725f);
	pearl.diffuse = Vector(1.f, 0.829f, 0.829f);
	pearl.specular = Vector(0.296648f, 0.296648f, 0.296648f);
	pearl.shiness = 0.088f;

	ruby.ambient = Vector(0.1745f, 0.01175f, 0.01175f);
	ruby.diffuse = Vector(0.61424f, 0.04136f, 0.04136f);
	ruby.specular = Vector(0.727811f, 0.626959f, 0.626959f);
	ruby.shiness = 0.6f;

	chrome.ambient = Vector(0.25f, 0.25f, 0.25f);
	chrome.diffuse = Vector(0.4f, 0.4f, 0.4f);
	chrome.specular = Vector(0.774597f, 0.774597f, 0.774597f);
	chrome.shiness = 0.6f;

	red_plastic.ambient = Vector(0.0f, 0.0f, 0.0f);
	red_plastic.diffuse = Vector(0.5f, 0.f, 0.0f);
	red_plastic.specular = Vector(0.70f, 0.60f, 0.60f);
	red_plastic.shiness = 0.25f;
}