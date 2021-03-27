#ifndef CS250_MESH_OBJECT_H
#define CS250_MESH_OBJECT_H

#include "Mesh.h"

class Mesh_Object : public Mesh
{
public:
	virtual unsigned GetVAOId();
	virtual unsigned GetShaderId();
	virtual void SetVertices() = 0;
	//void InitializeTextureCoord();
	void InitializeElement();
	void InitializeVerticesDatas();
	//void InitializeNormalDatas();
	void SetNormal();
	void InitializeTextureObj(std::string sprite_path);
protected:
	std::vector<Hcoord> normal;
	std::vector<Point> vertices;
	
};

#endif