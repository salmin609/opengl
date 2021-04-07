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
	
	void DrawingGround();
	void DrawingShadow();
	void DrawingWater();
	void DrawingObjs();
	void DrawLight();
	void WaterInitialize(Hcoord planeVec);
	
	std::vector<Object*> object;

	Matrix camMat;
	Matrix ndcMat;
	
	size_t objectsSize;
	SkyBox* skyBox;
	FrameBufferObject* frameBufferObj;
 	FrameBufferObject* waterReflectframeBufferObj;
	FrameBufferObject* waterReflactframeBufferObj;
	WaterFrameBuffer* waterFrameBuffer;
	OutLine* outLine;
	FrameBuffer* reflectFramebuffer;
	FrameBuffer* refractFramebuffer;
	FrameBuffer* checkBuffer;
	Texture* waterDuDv;
	Texture* normalMap;

	unsigned quadVao;
	unsigned quadVbo;
	Shader* frameBufferObjectShader;
	
	float moveSpeed = 0.03f;
	float moveFactor = 0.f;
};