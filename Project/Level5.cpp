#include "Level5.h"

#include "Graphic.h"
#include "Shader.h"
#include "Shader_Table.hpp"
#include "Texture.h"
#include "vs2017/SimpleMeshes.h"
#include "VAO.h"
Level5::Level5()
{
	int workGroupCount[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);

	printf("max global (total) work group counts x:%i y:%i z:%i\n",
		workGroupCount[0], workGroupCount[1], workGroupCount[2]);

	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);

	int work_grp_inv;
	
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);
	
	shaderCheck = new Shader(shaderComputePrefix.c_str());
	//shaderCheck->SendUniformInt("imgOutput", 0);
	shaderTexture = new Shader(shaderFrameBufferVertex.c_str(), shaderFrameBufferFragment.c_str());
	shaderTexture->Use();
	shaderTexture->SendUniformInt("screenTextures", 0);
	
	computeInput = new Texture("container.png");
	computeOutput = new Texture();
	computeInput->GetImageWidthHeight(imgWidth, imgHeight);

	computeInput->BindImageTexture(0, GL_READ_ONLY);
	computeOutput->BindImageTexture(1, GL_WRITE_ONLY);
	//glDispatchCompute(imgWidth / 32, imgHeight / 32, 1);
	
	/*shaderCheck->Use();
	glBindImageTexture(0, computeInput->GetTextureId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, computeOutput->GetTextureId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute(imgWidth / 32, imgHeight / 32, 1);*/
	
	int count = 0;
	std::vector<int> sizePerIndex;
	
	sizePerIndex.push_back(2);
	sizePerIndex.push_back(2);
	
	float* simpleRect = SimpleMesh::SimpleRectangleWithTextureCoord(count);
	vao = new VAO(shaderTexture);
	vao->Init(simpleRect, count, 2, sizePerIndex);

	delete[] simpleRect;
}

Level5::~Level5()
{
}

void Level5::Load()
{
	Graphic::objects.clear();
	Graphic::water = nullptr;
}

void Level5::Update(float dt)
{
	(dt);
	glViewport(0, 0, 1024, 768);

	shaderCheck->Use();
	glDispatchCompute(imgWidth / 32, imgHeight / 32, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//glBindTexture(GL_TEXTURE_2D, 0);
	

	//glBindImageTexture(0, computeInput->GetTextureId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	//glBindImageTexture(0, computeOutput->GetTextureId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	
	vao->Bind();
	computeOutput->Bind(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Level5::UnLoad()
{
}
