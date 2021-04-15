#pragma once
#include <vector>
class Shader;
#include "Affine.h"

class VAO
{
public:
	
	VAO(Shader* shaderData);
	void Init(const std::vector<Vertex>& datas);
	void Init(float* data, int size, int indexNum, const std::vector<int>& sizePerIndex);
	void Bind();
	unsigned GetId();
	~VAO();
private:
	unsigned vaoId;
	Shader* shader;
	unsigned vboSlotId;
};

