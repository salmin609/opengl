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
#include "Level4.h"
#include "Level5.h"
#include "Level6.h"

Client::Client(void)
{
	graphic = new Graphic();
	
	level1 = new Level1();
	level2 = new Level2();
	level3 = new Level3();
	level4 = new Level4();
	level5 = new Level5();
	level6 = new Level6();

	level1->SetNextPrevState(nullptr, level2);
	level2->SetNextPrevState(level1, level3);
	level3->SetNextPrevState(level2, level4);
	level4->SetNextPrevState(level3, level5);
	level5->SetNextPrevState(level4, level6);
	level6->SetNextPrevState(level5, nullptr);
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
	State* nextState = currState->GetNextState();

	if(nextState != nullptr)
	{
		currState->UnLoad();
		nextState->Load();
		currState = nextState;
	}
}

void Client::Decrease_Graphic_Level()
{
	State* prevState = currState->GetPrevState();

	if (prevState != nullptr)
	{
		currState->UnLoad();
		prevState->Load();
		currState = prevState;
	}
}


void Client::Set_Prev_Mousepos(Point p)
{
	graphic->Set_Prev_Mousepos(p);
}

Client::~Client(void)
{
	delete graphic;
	delete level1;
	delete level2;
	delete level3;
	delete level4;
	delete level5;
	delete level6;
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
