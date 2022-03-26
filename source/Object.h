#pragma once

//My headers
#include "Settings.h"
#include "MyTypes.h"



extern SDL_Renderer* renderer;


//Only 2 object types atm
enum ObjectTypes
{
	abstract,
	bot
};


class Object
{

private:
	uint lastTickFrame = 0;

protected:

	//Time in ticks since object was created
	uint lifetime = 0;

	//static SDL_Renderer* renderer;

public:

	//Coordinates
	int x, y;

	//Object type
	ObjectTypes type;

	Object(int X, int Y) :x(X), y(Y), type(abstract) {};


	//Basic 'dummy' draw function if needed
	virtual void draw();

	/*This function returns true when the creature dies
	You should call it on every simulation tick before you
	call same function in descendant class
	Returns:
	0 - all fine
	1 - object destroyed
	2 - nothing to do(last tick frame matches current frame)*/
	virtual int tick();

	static uint currentFrame;



	//Returns lifetime
	uint GetLifetime();

};

