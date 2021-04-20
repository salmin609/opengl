#pragma once
#include <vector>
#include "Affine.h"

class Texture;
class FrameBufferObject;
class Shader;
class Object;
class SkyBox;
class OutLine;
class FrameBuffer;
class WaterFrameBuffer;
class WaterRenderer;
/**
 * \brief
 * Manager which managing the drawing methods for Graphic class
 */
class DrawingManager
{
public:
	DrawingManager();
	void Drawing(float dt);
private:
	void ClearBuffer();
	
	void DrawingGround(float dt);
	void DrawingShadow();
	void DrawingObjs();
	void DrawLight();
	void WaterInitialize(Hcoord planeVec);
	
	std::vector<Object*> object;

	Matrix camMat;
	Matrix ndcMat;
	float totalTime = 0.f;
	size_t objectsSize;
	SkyBox* skyBox;
	FrameBufferObject* frameBufferObj;
	OutLine* outLine;
	WaterRenderer* waterRenderer;
};