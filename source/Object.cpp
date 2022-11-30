#include "Bot.h"


uint Object::currentFrame = 0;
Field* Object::static_pField = NULL;
Object*** Object::static_pCells = NULL;

const Rect Object::image_rect = { 0, 0, FieldCellSize, FieldCellSize };



Object::Object(int X, int Y) :x(X), y(Y), type(abstract)
{
	energy = 0;

	//Set up pointers on field class and cells array
	pField = static_pField;
	pCells = (Object* (*)[FieldCellsWidth][FieldCellsHeight])static_pCells;
}


void Object::CalcScreenX()
{
	screenX = x - Field::renderX;

	if (screenX < 0)
	{
		screenX += FieldCellsWidth;
	}
}

void Object::CalcObjectRect()
{
	object_rect = { FieldX + screenX * FieldCellSize, FieldY + y * FieldCellSize, FieldCellSize, FieldCellSize };
}

void Object::CalcObjectRectShrinked(int shrink)
{
	object_rect = { FieldX + screenX * FieldCellSize + shrink, FieldY + y * FieldCellSize + shrink,
		FieldCellSize - 2*shrink, FieldCellSize - 2*shrink };
}

void Object::draw()
{
	CalcScreenX();	
	CalcObjectRect();

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &object_rect);
}

void Object::drawEnergy() { draw(); };
void Object::drawPredators() { draw(); };

int Object::tick()
{
	//if already made a move on this turn
	if (currentFrame == lastTickFrame)
		return 2;

	++lifetime;
	lastTickFrame = currentFrame;

	return 0;
}


uint Object::GetLifetime()
{
	return lifetime;
}

void Object::SetLifetime(uint val)
{
	lifetime = val;
}

void Object::SetPointers(Field* field, Object*** cells)
{
	static_pField = field;
	static_pCells = cells;
}