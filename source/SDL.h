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

bool ReadMouseState();


void InitSDL();
void DeInitSDL();

bool CreateWindowSDL();
bool CreateRenderer();



extern void SDLPresentScene();
