
#include "SDL.h"


SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;

ImGuiIO* io = NULL;

MouseState mouseState;


void InitSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		exit(-1);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, SDL_glCont_Major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, SDL_glCont_Minor);
}


void DeInitSDL()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


bool CreateWindowSDL()
{
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

	window = SDL_CreateWindow(
		WindowCaption,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WindowWidth,
		WindowHeight,
		window_flags
	);

	if (window == NULL)
		return false;

	//no resize
	SDL_SetWindowMinimumSize(window, WindowWidth, WindowHeight);

	return true;
}


bool CreateRenderer()
{
	//renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//VSync may be here
	SDL_GL_SetSwapInterval(0);

	//Setup GL
	glViewport(0, 0, WindowWidth, WindowHeight);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return (renderer != NULL);
}

bool ReadMouseState()
{
	mouseState.buttons = SDL_GetMouseState(&mouseState.mouseX, &mouseState.mouseY);

	io->DeltaTime = 1.0f / 60.0f;
	io->MousePos = ImVec2(static_cast<float>(mouseState.mouseX), static_cast<float>(mouseState.mouseY));
	io->MouseDown[0] = mouseState.buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io->MouseDown[1] = mouseState.buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
	io->MouseWheel = static_cast<float>(mouseState.wheel);

	io->KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
	io->KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
	io->KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
	io->KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);

	return (io->MouseDown[0]) && (!io->WantCaptureMouse);
}

void SDLPresentScene()
{
	ImGui::Render();

	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(renderer);
}