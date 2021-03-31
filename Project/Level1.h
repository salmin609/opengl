#include "State.h"
#include "SimpleCube_Mesh.h"
#include "Sphere_mesh.h"
#include "SnubDodecMesh.h"
#include "CubeMesh.h"
#include "Object.h"
#include "Light_Mesh.h"
#include "LoadedObj.h"

class Level1 : public State
{
public:
	Level1();
	virtual void Load();
	virtual void Update(float dt);
	virtual void UnLoad();
	void Init_Objects();
	void Initialize_Material();

private:
	SnubDodecMesh snub_mesh;
	CubeMesh cube_mesh;
	SimpleCubeMesh simplecube_mesh;
	SphereMesh sphere_mesh;
	Light_Mesh light_mesh;
	LoadedObj* deerObj;
	LoadedObj* quadObj;
	
	Object* center_circle;
	Object* left_cube;
	Object* right_cube;
	Object* right_circle;
	Object* light;
	
	Object* right_heart;
	Object* left_heart;
	Object* ground;
	Object* deer;
	Object* tree;

	Material_ emerald;
	Material_ obsidian;
	Material_ jade;
	Material_ pearl;
	Material_ ruby;
	Material_ chrome;
	Material_ red_plastic;
};