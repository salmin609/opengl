#pragma once
#include <vector>
class Shader;
#include "Affine.h"

class VAO
{
public:
	
	VAO(Shader* shaderData);
	void Init(const std::vector<Vertex>& datas);
	unsigned GetId();
	~VAO();
private:
	unsigned vaoId;
	Shader* shader;
	unsigned vboSlotId;
};

