#pragma once

#include "Settings.h"
#include "MyTypes.h"

//Window
extern SDL_Renderer* renderer;
extern SDL_Window* window;

//Input output
extern ImGuiIO* io;

extern struct MouseState
{
	int
	buttons,
	wheel,
	mouseX, mouseY;
} mouseState;


void InitSDL();
void DeInitSDL();

bool CreateWindowSDL();
bool CreateRenderer();

bool ReadMouseState();


extern inline void SDLPresentScene();