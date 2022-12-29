#include "Rock.h"



void Rock::draw()
{
	CalcScreenX();
	CalcObjectRectShrinked(1);

	//Draw stone
	SDL_SetRenderDrawColor(renderer, RockDrawColor);
	SDL_RenderFillRect(renderer, &object_rect);
}

Rock::Rock(int X, int Y) :Object(X, Y)
{
	type = rock;
}
