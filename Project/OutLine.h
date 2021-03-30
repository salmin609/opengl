#pragma once
#include "Affine.h"

class Shader;


class OutLine
{
public:
	OutLine();
	void OutlinePrepare();
	void Draw(Matrix& camMat, Matrix& ndcMat);
private:
	Shader* outlineShader;
	
};