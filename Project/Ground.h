/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#pragma once

#include "Affine.h"
#include "Mesh.h"

class Ground
{
public:
    Ground(Mesh* mesh_val, Vector color = Vector(0.f, 0.f, 0.f));
    Ground(Mesh* mesh_val, Point obj_pos, Vector obj_scale, Vector color);
    void Draw(Matrix& ndc, Matrix& cam_mat);
    void Set_Color(Vector color);
    void End_Draw();
    void Select_Mesh();
    Matrix Get_Model_To_World();
    void Send_Uniform(Matrix& ndc_mat, Matrix& cam_mat, Matrix& world_mat);

private:
    Mesh* mesh;
    Affine model_2_world;
    Vector color_val;

    unsigned int color_uniform;
    unsigned int ndc_uniform;
    unsigned int cam_uniform;
    unsigned int world_uniform;

    unsigned int shader_id;
    unsigned int vao_id;

    Vector obj_scale;
    Point obj_pos;
};