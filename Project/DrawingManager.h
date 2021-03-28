#pragma once
#include <vector>
#include "Affine.h"

class Shader;
class Object;
/**
 * \brief
 * Manager which managing the drawing methods for Graphic class
 */
class DrawingManager
{
public:
	DrawingManager();
	void Drawing();
private:
	void DrawingGround();
	void DrawingShadow();
	void DrawingObjs();
	void DrawingOutline();
	void OutlinePrepare();
	
	
	Matrix camMat;
	Matrix ndcMat;
	size_t objectsSize;
	std::vector<Object*> object;

	Shader* simpleShader;
};