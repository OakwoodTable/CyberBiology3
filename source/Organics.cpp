#include "Organics.h"




void Organics::draw()
{
	SDL_Rect rect = { FieldX + x * FieldCellSize + 1, FieldY + y * FieldCellSize + 1, FieldCellSize - 2, FieldCellSize - 2 };

	//Draw
	SDL_SetRenderDrawColor(renderer, OrganicWateDrawColor);
	SDL_RenderFillRect(renderer, &rect);
}

void Organics::drawEnergy()
{
	SDL_Rect rect = { FieldX + x * FieldCellSize + 2, FieldY + y * FieldCellSize + 2, FieldCellSize - 4, FieldCellSize - 4 };

	//Draw
	SDL_SetRenderDrawColor(renderer, 255, (1.0f - ((energy * 1.0f) / (MaxPossibleEnergyForABot * .5f))) * 255.0f, 0, 255);
	SDL_RenderFillRect(renderer, &rect);
}

Organics::Organics(int X, int Y, int Energy) :Object(X, Y), energy(Energy)
{
	type = organic_waste;
}

