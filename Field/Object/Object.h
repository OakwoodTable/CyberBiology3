#pragma once


#include "Main.h"


//Only 2 object types atm
enum ObjectTypes
{
	abstract,
	bot
};


class Object
{

protected:

	//Time in ticks since object was created
	uint lifetime = 0;

public:

	//Coordinates
	int x, y;

	//Object type
	ObjectTypes type;

	Object(int X, int Y) :x(X), y(Y), type(abstract){};


	//Basic 'dummy' draw function if needed
	virtual void draw()
	{
		SDL_Rect rect = { FieldX + x * FieldCellSize, FieldY + y * FieldCellSize, FieldCellSize, FieldCellSize };

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 111);
		SDL_RenderFillRect(renderer, &rect);
	}

	//This function returns true when the creature dies
	//You should call it on every simulation tick before you
	//call same function in descendant class
	virtual bool tick()
	{
		++lifetime;

		return false;
	}

	//Returns lifetime
	uint GetLifetime()
	{
		return lifetime;
	}

};
