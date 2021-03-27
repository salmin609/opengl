#include "DrawingManager.h"
#include "Graphic.h"
#include "Projection.h"

DrawingManager::DrawingManager()
{
	simpleShader = new Shader(shader_object_vertex.c_str(),
		shaderSingleColorFragment.c_str());
}

void DrawingManager::Drawing()
{
	ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	camMat = WorldToCamera(*CameraManager::instance->GetCamera());
	objectsSize = Graphic::objects.size();
	object = Graphic::objects;
	DrawingGround();
	DrawingShadow();
	DrawingObjs();
}

void DrawingManager::DrawingGround()
{
	if(Graphic::ground != nullptr)
	{
		Point& lightPos = Graphic::light->Get_Obj_Pos();

		Graphic::ground->Set_Light_Pos(lightPos);
		Graphic::ground->Draw(ndcMat, camMat);
	}
}
void DrawingManager::DrawingShadow()
{
	Point& lightPos = Graphic::light->Get_Obj_Pos();
	
	glDisable(GL_DEPTH_TEST);
	Matrix shadowMat = Shadow(lightPos);
	for(size_t i = 0 ; i < objectsSize; ++i)
	{
		Object* obj = object[i];
		Object_ExceptShadow* shadow = obj->Get_Shadow();
		Matrix objModelMat = obj->Get_Model_To_World();

		if(shadow != nullptr)
		{
			shadow->Set_Light_Pos(lightPos);
			shadow->Draw(ndcMat, camMat, objModelMat, shadowMat);
		}
	}
	glEnable(GL_DEPTH_TEST);
}

void DrawingManager::DrawingObjs()
{
	//simpleShader->Use();
	//simpleShader->SendUniformMat("to_ndc", &ndcMat);
	//simpleShader->SendUniformMat("cam", &camMat);
	//glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	
	for(size_t i = 0; i < objectsSize; ++i)
	{
		Object* obj = object[i];
		obj->Set_Camera_Pos(CameraManager::instance->CameraPos());
		obj->Set_Light_Pos(Graphic::light->Get_Obj_Pos());
		obj->Draw(ndcMat, camMat);
	}
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	simpleShader->Use();
	simpleShader->SendUniformMat("to_ndc", &ndcMat);
	simpleShader->SendUniformMat("cam", &camMat);
	for(size_t i = 0 ; i < objectsSize; ++i)
	{
		Object* obj = object[i];
		obj->SetVAO();
		Matrix model2World = obj->GetModelToWorldOutlineScaling(0.1f);
		//Matrix model2World = obj->Get_Model_To_World();
		simpleShader->SendUniformMat("model", &model2World);

		if(obj->IsElemented())
		{
			glDrawElements(GL_TRIANGLES, obj->Get_Mesh()->FaceCount() * 3, GL_UNSIGNED_INT, nullptr);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, obj->Get_Mesh()->FaceCount() * 3);
		}
	}
	glBindVertexArray(0);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);

}
