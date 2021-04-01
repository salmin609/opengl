/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "Client.h"

int main(int /*argc*/, char* /*argv*/[]) {

	// SDL: initialize and create a window
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	const char* title = "CS250: Project#2 Kimsangmin";
	int width = 600,
		height = 600;
	SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, width, height,
		SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	bool traverse_mode = false;
	bool move_faster_mode = false;

	// GLEW: get function bindings (if possible)
	GLenum value = glewInit();
	if (value != GLEW_OK) {
		std::cout << glewGetErrorString(value) << std::endl;
		SDL_GL_DeleteContext(context);
		SDL_Quit();
		return -1;
	}

	// animation loop
	try {
		Client* client = new Client();
		client->Set_Window_WH(width, height);
		Uint32 ticks_last = SDL_GetTicks();
		bool done = false;
		while (!done) {
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_QUIT:
					done = true;
					break;
				case SDL_KEYDOWN:
				{
					SDL_Keycode code = event.key.keysym.sym;
					if (code == SDLK_ESCAPE)
						done = true;
					else if (code == SDLK_SPACE)
					{
						traverse_mode = !traverse_mode;
						client->Set_Traverse_Mode(traverse_mode);
					}
					else if (code == SDLK_LSHIFT)
					{
						move_faster_mode = !move_faster_mode;
						client->Set_Move_Faster(move_faster_mode);
					}
					else if (code == SDLK_w ||
						code == SDLK_a ||
						code == SDLK_s ||
						code == SDLK_d)
					{
						client->Move(code);
					}
					else if (code == SDLK_RIGHT)
					{
						client->Increase_Graphic_Level();
					}
					else if (code == SDLK_LEFT)
					{
						client->Decrease_Graphic_Level();
					}
				}
					break;
				case SDL_MOUSEWHEEL:
					client->mousewheel(event);
					break;
				case SDL_MOUSEBUTTONDOWN:
					client->mousepress();
					break;
				case SDL_MOUSEMOTION:
					int mouse_x, mouse_y;

					SDL_GetMouseState(&mouse_x, &mouse_y);

					if (traverse_mode == true)
					{
						client->Update_AB(Point(static_cast<float>(mouse_x), static_cast<float>(mouse_y), 0));
					}
					client->mouse_motion();
					client->Set_Prev_Mousepos(Point(static_cast<float>(mouse_x), static_cast<float>(mouse_y), 0));
					break;
				case SDL_MOUSEBUTTONUP:
					client->Set_Selected_Null();
					break;
				}
			}
			Uint32 ticks = SDL_GetTicks();
			float dt = 0.001f * (ticks - ticks_last);
			ticks_last = ticks;
			client->draw(dt);
			SDL_GL_SwapWindow(window);
		}

		delete client;
	}

	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

