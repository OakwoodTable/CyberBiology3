#pragma once

#include "Field.h"


class ImageFactory final
{

private:

	static SDL_Texture* apple;
	static SDL_Texture* organics;
	static SDL_Texture* rock;

	static SDL_Texture* bot_body;
	static SDL_Texture* bot_head[8];

	static SDL_Texture* TurnSurfaceToStaticTexture(SDL_Surface* surf);
	static void DrawRectOnSurface(vector<uint32_t>& pixels, Rect rect, int surf_w, int color = 0x000000ff);

	static void CreateApple();
	static void CreateOrganics();
	static void CreateRock();
	static void CreateBot();


public:

	static void CreateImages();
	static void DeleteImages();

};