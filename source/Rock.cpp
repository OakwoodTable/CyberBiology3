#include "Rock.h"

void Rock::draw()
{
	SDL_Rect rect = { FieldX + x * FieldCellSize + 1, FieldY + y * FieldCellSize + 1, FieldCellSize - 2, FieldCellSize - 2};

	//Draw stone
	SDL_SetRenderDrawColor(renderer, RockDrawColor);
	SDL_RenderFillRect(renderer, &rect);
}

Rock::Rock(int X, int Y) :Object(X, Y)
{
	type = rock;
}
