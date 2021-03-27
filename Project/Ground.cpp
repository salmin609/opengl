/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#include "Ground.h"

Ground::Ground(Mesh* mesh_val, Vector color) : mesh(mesh_val), color_val(color)
{
	shader_id = mesh->Get_Shader_Id();
	vao_id = mesh->Get_VAO_Id();
	color_uniform = glGetUniformLocation(shader_id, "color_val");
	ndc_uniform = glGetUniformLocation(shader_id, "to_ndc");
	world_uniform = glGetUniformLocation(shader_id, "model");
	cam_uniform = glGetUniformLocation(shader_id, "cam");
}

Ground::Ground(Mesh* mesh_val, Point obj_pos, Vector obj_scale, Vector color) : mesh(mesh_val), obj_pos(obj_pos), obj_scale(obj_scale), color_val(color)
{
	shader_id = mesh->Get_Shader_Id();
	vao_id = mesh->Get_VAO_Id();
	color_uniform = glGetUniformLocation(shader_id, "color_val");
	ndc_uniform = glGetUniformLocation(shader_id, "to_ndc");
	world_uniform = glGetUniformLocation(shader_id, "model");
	cam_uniform = glGetUniformLocation(shader_id, "cam");
}

void Ground::Draw(Matrix& ndc, Matrix& cam_mat)
{
	Select_Mesh();
	Matrix world_mat = Get_Model_To_World();
	Send_Uniform(ndc, cam_mat, world_mat);
	glDrawArrays(GL_TRIANGLES, 0, mesh->FaceCount() * 3);
	End_Draw();
}

void Ground::Set_Color(Vector color)
{
	color_val = color;
}

void Ground::End_Draw()
{
	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Ground::Select_Mesh()
{
	glUseProgram(mesh->Get_Shader_Id());
	glBindVertexArray(mesh->Get_VAO_Id());
}

Matrix Ground::Get_Model_To_World()
{
	return translate(obj_pos - Point(0, 0, 0)) * scale(obj_scale.x, obj_scale.y, obj_scale.z);
}

void Ground::Send_Uniform(Matrix& ndc_mat, Matrix& cam_mat, Matrix& world_mat)
{
	glUniformMatrix4fv(ndc_uniform, 1, GL_TRUE, &ndc_mat[0][0]);
	glUniformMatrix4fv(world_uniform, 1, GL_TRUE, &world_mat[0][0]);
	glUniformMatrix4fv(cam_uniform, 1, GL_TRUE, &cam_mat[0][0]);
	glUniform3f(color_uniform, color_val.x, color_val.y, color_val.z);
}

