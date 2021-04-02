/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#include "Object.h"
#include "Object_ExceptShadow.h"
#include "Projection.h"

Object::Object(Mesh_Object* mesh_val, Point pos, Material_* mat, Mesh* shadow_mesh) : shadow(nullptr), obj_mat(mat), t(0.f)
{
	objInfo.pos = pos;
	
	mesh = mesh_val;
	if (shadow_mesh != nullptr)
	{
		Set_Shadow(shadow_mesh);
	}
	Init();
}

Object::Object(Mesh* mesh_val, Point pos, Material_* mat, Vector color) : shadow(nullptr), obj_mat(mat), t(0.f)
{
	objInfo.pos = pos;
	objInfo.color = color;
	mesh = mesh_val;
	Init();
}

Object::~Object()
{
	if (shadow != nullptr)
	{
		delete shadow;
	}
}

void Object::Init()
{
	shader_id = mesh->Get_Shader_Id();
	vao_id = mesh->Get_VAO_Id();
	uniforms.SetShaderId(shader_id);
	Initialize_Uniform();
	mesh->Unbind();
}

void Object::Draw()
{
	Select_Mesh();
	Send_Uniform(Get_Model_To_World());

	if (mesh->IsElemented())
	{
		glDrawElements(GL_TRIANGLES, mesh->FaceCount() * 3, GL_UNSIGNED_INT, nullptr);
	}
	else if(mesh->IsInstancing())
	{
		glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->FaceCount() * 3, 
			mesh->InstancingNum());
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, mesh->FaceCount() * 3);
	}

	End_Draw();
}



void Object::End_Draw()
{
	mesh->Unbind();
}

void Object::Select_Mesh()
{
	shader_id = mesh->Get_Shader_Id();
	vao_id = mesh->Get_VAO_Id();
	mesh->SetTexture();

	glUseProgram(shader_id);
	glBindVertexArray(vao_id);

	if (mesh->IsElemented())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetElementId());
	}
	Initialize_Uniform();

}

void Object::Send_Uniform(Matrix world_mat)
{
	uniforms("model", &world_mat);
	uniforms("view_pos", &camera_pos);
	uniforms("light_pos", &lightInfo.lightPos);
	
	if (obj_mat != nullptr)
	{
		uniforms("material.ambient", &obj_mat->ambient);
		uniforms("material.diffuse", &obj_mat->diffuse);
		uniforms("material.specular", &obj_mat->specular);
		uniforms("material.shininess", &obj_mat->shiness);
	}
	uniforms("light_.ambient", &lightInfo.ambient);
	uniforms("light_.diffuse", &lightInfo.diffuse);
	uniforms("light_.specular", &lightInfo.specular);
	uniforms("color_val", &objInfo.color);
}

void Object::Set_Light_Pos(Point light)
{
	lightInfo.lightPos = light;
}

void Object::Set_Second_Light_Pos(Point light)
{
	(light);
	//second_light_pos = light;
}

void Object::Set_Shadow(Mesh* mesh_val)
{
	shadow = new Object_ExceptShadow(mesh_val);
	shadow->Set_Color(Vector(0, 0, 0));
}

void Object::Set_Rot_Rate(float val)
{
	objInfo.rotRate = val;
}

void Object::Set_Rot_Axis(Vector axis)
{
	objInfo.rotAxis = axis;
}

void Object::Set_Camera_Pos(Point cam_pos)
{
	camera_pos = cam_pos;
}

Matrix Object::Get_Model_To_World()
{
	return translate(objInfo.pos - Point(0, 0, 0)) * rotate(objInfo.rotRate, objInfo.rotAxis) * scale(objInfo.scale.x, objInfo.scale.y, objInfo.scale.z);
}

Matrix Object::GetModelToWorldOutlineScaling(float increment)
{
	return translate(objInfo.pos - Point(0, 0, 0)) * rotate(objInfo.rotRate, objInfo.rotAxis) * scale(objInfo.scale.x + increment, objInfo.scale.y + increment, objInfo.scale.z + increment);
}

Mesh* Object::Get_Mesh()
{
	return mesh;
}

Point& Object::Get_Obj_Pos()
{
	return objInfo.pos;
}

Vector Object::Get_Scale()
{
	return objInfo.scale;
}

void Object::Set_Scale(Vector scale)
{
	this->objInfo.scale = scale;
}

void Object::Set_T(float t_)
{
	t = t_;
}

float Object::Get_T()
{
	return t;
}

void Object::Set_Selected(bool toggle)
{
	selected = toggle;
}

bool Object::Get_Selected()
{
	return selected;
}

float& Object::Get_Rot_Rate()
{
	return objInfo.rotRate;
}

Vector& Object::Get_Rot_Axis()
{
	return objInfo.rotAxis;
}

void Object::Set_Material(Material_* mat_)
{
	obj_mat = mat_;
}

Object_ExceptShadow* Object::Get_Shadow()
{
	return shadow;
}

void Object::SetVAO()
{
	glBindVertexArray(mesh->Get_VAO_Id());
}


void Object::Initialize_Uniform()
{
	uniforms.AddUniform("to_ndc", UniformManager::Matrix4x4);
	uniforms.AddUniform("light_pos", UniformManager::Vector4);
	uniforms.AddUniform("second_light_pos", UniformManager::Vector3);
	uniforms.AddUniform("model", UniformManager::Matrix4x4);
	uniforms.AddUniform("cam", UniformManager::Matrix4x4);
	uniforms.AddUniform("view_pos", UniformManager::Vector3);
	uniforms.AddUniform("material.ambient", UniformManager::Vector3);
	uniforms.AddUniform("material.diffuse", UniformManager::Vector3);
	uniforms.AddUniform("material.specular", UniformManager::Vector3);
	uniforms.AddUniform("material.shininess", UniformManager::Float);
	uniforms.AddUniform("light_.ambient", UniformManager::Vector3);
	uniforms.AddUniform("light_.diffuse", UniformManager::Vector3);
	uniforms.AddUniform("color_val", UniformManager::Vector3);
}

void Object::IncreScale()
{
	objInfo.scale.x = 0.45f;
	objInfo.scale.y = 0.45f;
}

bool Object::IsElemented()
{
	return mesh->IsElemented();
}
