/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#include <SDL2/SDL.h>
#include "Graphic.h"
class State;
class Level1;
class Level2;
class Level3;
class Level4;
class Level5;
class Client {
public:
	Client(void);
	~Client(void);

	void Set_Window_WH(int width, int height);
	void Set_Traverse_Mode(bool toggle);
	void Set_Move_Faster(bool toggle);
	void mousewheel(SDL_Event event);
	void mousepress();
	void Update_AB(Point curr_mouse_pos);
	void Move(SDL_Keycode keycode);
	void mouse_motion();
	void Set_Prev_Mousepos(Point p);
	void Set_Selected_Null();
	void draw(float dt);
	void Increase_Graphic_Level();
	void Decrease_Graphic_Level();
	
private:
	Graphic* graphic;
	Level1* level1;
	Level2* level2;
	Level3* level3;
	Level4* level4;
	Level5* level5;
	State* currState;
};