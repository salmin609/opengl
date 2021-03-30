#pragma once

#include <SDL2/SDL.h>
#include "Affine.h"
#include "Object.h"
#include "Object_ExceptShadow.h"
#include "DrawingManager.h"
#include "CameraManager.h"
#include "RaycastManager.h"
class Graphic
{
public:
	static Graphic* instance;
	Graphic(void);
	~Graphic(void);

	void Draw(float dt);
	void mousewheel(SDL_Event event);
	void mousepress();
	void mouse_motion();
	//void ClearBuffers();
	void Initialize_Camera();
	void Update_AB(Point curr_mouse_pos);
	void Move(SDL_Keycode keycode);
	void Set_Prev_Mousepos(Point p);
	Object* Ray_To_Object(Point mouse_pos_world);
	void Set_Selected_Null();
	void Set_Traverse_Mode(bool toggle);
	void Set_V_V();
	void Set_Move_Faster(bool toggle);
	void Set_Window_WH(int width, int height);
	Hcoord GetWindowWH();
	void Indicate_Level();
	Point GetPrevMousePos() const;
	
	Object* SelectedObject();
	static std::vector<Object*> objects;
	static Object* light;
	static Object* ground;
private:
	Point prev_mouse_pos;
	//Picked object
	Object* obj_selected;

	int window_width;
	int window_height;

	int graphic_level = 0;
	
	/**
	 * \brief
	 * Manager 
	 */
	CameraManager* cameraManager;
	DrawingManager* drawingManager;
	RaycastManager* raycastManager;
};