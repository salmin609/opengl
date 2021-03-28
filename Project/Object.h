/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#pragma once
#include "Affine.h"
#include "Mesh_Object.h"
#include "UniformManager.h"
class Object_ExceptShadow;

struct Material_
{
    Vector ambient;
    Vector diffuse;
    Vector specular;
    float shiness;
};

class Object
{
public:
    Object(Mesh_Object* mesh_val, Point pos, Material_* mat, Mesh* shadow_mesh = nullptr);
    Object(Mesh* mesh_val, Point pos, Material_* mat, Vector color);
    ~Object();
    void Init();
    void Draw(Matrix& ndc, Matrix& cam_mat);
    void DrawByElement(Matrix& ndc, Matrix& camMat);
    void End_Draw();
    void Select_Mesh();
    void Send_Uniform(Matrix& ndc_mat, Matrix& cam_mat, Matrix world_mat);
    void Set_Light_Pos(Point light);
    void Set_Second_Light_Pos(Point light);
    void Set_Shadow(Mesh* mesh_val);
    void Set_Rot_Rate(float val);
    void Set_Rot_Axis(Vector axis);
    void Set_Camera_Pos(Point cam_pos);
    Matrix Get_Model_To_World();
    Matrix GetModelToWorldOutlineScaling(float increment);
    Mesh* Get_Mesh();
    Point& Get_Obj_Pos();
    Vector Get_Scale();
    void Set_Scale(Vector scale);
    void Set_T(float t_);
    float Get_T();
    void Set_Selected(bool toggle);
    bool Get_Selected();
    float& Get_Rot_Rate();
    Vector& Get_Rot_Axis();
    void Set_Material(Material_* mat_);
    Object_ExceptShadow* Get_Shadow();
    void SetVAO();
    void Initialize_Uniform();
    void IncreScale();
    bool IsElemented();
private:
    struct LightInfo
    {
        Point lightPos;
        Vector diffuse{0.5f, 0.5f, 0.5f};
        Vector ambient{0.2f, 0.2f, 0.2f};
        Vector specular{1.f, 1.f, 1.f};
    };
	struct ObjInfo
	{
        Vector scale{ 1.f, 1.f, 1.f };
        Point pos;
        Vector color;
        Vector rotAxis{ 0.f, 0.f, 1.f };
        float rotRate = 0.f;
	};
	
    Mesh* mesh;
    
    Object_ExceptShadow* shadow;
    Point camera_pos;
    UniformManager uniforms;
	
    unsigned int shader_id;
    unsigned int vao_id;
	
    float t;
    bool selected = false;

    Material_* obj_mat;

    //Vector color;
    ObjInfo objInfo;
    LightInfo lightInfo;
	
};