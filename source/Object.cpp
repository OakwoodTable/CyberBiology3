#include "Object.h"


void Object::draw()
{
	const SDL_Rect rect = { FieldX + x * FieldCellSize, FieldY + y * FieldCellSize, FieldCellSize, FieldCellSize };

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 111);
	SDL_RenderFillRect(renderer, &rect);
}

int Object::tick()
{
	if (currentFrame == lastTickFrame)
		return 2;

	++lifetime;
	lastTickFrame = currentFrame;

	return 0;
}

uint Object::currentFrame = 0;

//Returns lifetime
uint Object::GetLifetime()
{
	return lifetime;
}
