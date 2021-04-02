/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#include <algorithm>
#include "Client.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

Client::Client(void)
{
	graphic = new Graphic();
	
	level1 = new Level1();
	level2 = new Level2();
	level3 = new Level3();

	graphic->InitUniformBlockMatrices();
	
	currState = level1;
}

void Client::mousepress()
{
	graphic->mousepress();
}

void Client::mouse_motion()
{
	graphic->mouse_motion();
}

void Client::mousewheel(SDL_Event event)
{
	graphic->mousewheel(event);
}

void Client::Update_AB(Point curr_mouse_pos)
{
	graphic->Update_AB(curr_mouse_pos);
}

void Client::Move(SDL_Keycode keycode)
{
	graphic->Move(keycode);
}

void Client::draw(float dt)
{
	graphic->Draw(dt);
	currState->Update(dt);
}

void Client::Increase_Graphic_Level()
{
	if(currState == level1)
	{
		level2->Load();
		currState = level2;
	}
	else if(currState == level2)
	{
		level3->Load();
		currState = level3;
	}
}

void Client::Decrease_Graphic_Level()
{
	if (currState == level2)
	{
		level1->Load();
		currState = level1;
	}
	else if (currState == level3)
	{
		level2->Load();
		currState = level2;
	}
}


void Client::Set_Prev_Mousepos(Point p)
{
	graphic->Set_Prev_Mousepos(p);
}

Client::~Client(void) {
	delete graphic;
}

void Client::Set_Selected_Null()
{
	graphic->Set_Selected_Null();
}

void Client::Set_Traverse_Mode(bool toggle)
{
	graphic->Set_Traverse_Mode(toggle);
}

void Client::Set_Move_Faster(bool toggle)
{
	graphic->Set_Move_Faster(toggle);
}

void Client::Set_Window_WH(int width, int height)
{
	graphic->Set_Window_WH(width, height);
}
