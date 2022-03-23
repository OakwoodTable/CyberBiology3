#include "Object.h"


void Object::draw()
{
	SDL_Rect rect = { FieldX + x * FieldCellSize, FieldY + y * FieldCellSize, FieldCellSize, FieldCellSize };

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 111);
	SDL_RenderFillRect(renderer, &rect);
}

bool Object::tick()
{
	++lifetime;

	return false;
}

//Returns lifetime
uint Object::GetLifetime()
{
	return lifetime;
}
