#pragma once
#include <vector>
#include "Affine.h"

class FrameBufferObject;
class Shader;
class Object;
class SkyBox;
class OutLine;
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
	void ClearBuffer();
	
	void DrawingGround();
	void DrawingShadow();
	void DrawingWater();
	void DrawingObjs();

	void WaterInitialize(Hcoord planeVec);
	
	std::vector<Object*> object;

	Matrix camMat;
	Matrix ndcMat;
	
	size_t objectsSize;
	//SkyBox* skyBox;
	FrameBufferObject* frameBufferObj;
	FrameBufferObject* waterReflectframeBufferObj;
	FrameBufferObject* waterReflactframeBufferObj;
	OutLine* outLine;
};