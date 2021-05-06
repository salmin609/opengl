#pragma once
#include <vector>
class Shader;
class Buffer;
#include "Affine.h"

class VAO
{
public:
	
	VAO(Shader* shaderData = nullptr);
	void Init(std::vector<Vertex>& datas, bool onlyPos = false);
	void Init(float* data, int size, int indexNum, const std::vector<int>& sizePerIndex);
	void Bind();
	unsigned GetId();
	~VAO();
private:
	unsigned vaoId;
	Shader* shader;
	Buffer* buffer;
};

