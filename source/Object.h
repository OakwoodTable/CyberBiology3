#pragma once
//#pragma message("	Object_h")

struct FieldDynamicParams;
class Field;

#include "Field.h"


enum ObjectTypes
{
	abstract,
	bot,
	rock,
	organic_waste,
	apple
};


//Base class for any object on field
class Object
{
private:

	//Prev. tick frame number
	uint lastTickFrame = 0;

	//Static pointers to field class and cells array
	static Field* static_pField;
	static Object*** static_pCells;

protected:

	//X coordinate, corrected with Field::RenderX
	int screenX;

	void CalcScreenX();
	void CalcObjectRect();
	void CalcObjectRectShrinked(int shrink);

	//Time in ticks since object was created
	uint lifetime = 0;

	//Used for drawing
	SDL_Rect object_rect;

	//Pointers to Field class and cells array
	Object* (*pCells)[FieldCellsWidth][FieldCellsHeight];
	Field* pField;
	FieldDynamicParams* pParams;


public:

	int x, y;

	//If an object stores energy it's here
	int energy;

	virtual constexpr ObjectTypes type();
	virtual constexpr float image_sensor_val();

	Object(int X, int Y);


	//Basic 'dummy' draw functions if needed
	virtual void draw();
	virtual void drawEnergy();
	virtual void drawPredators();

	/*This function returns 1 when the object is destroyed.
	You should call it on every simulation tick before you
	call same function in derived class
	Returns:
	0 - all fine
	1 - object destroyed
	2 - nothing to do(last tick frame matches current frame)*/
	virtual int tick();	

	uint GetLifetime();
	void SetLifetime(uint);


	static uint currentFrame;
	static void SetPointers(Field* field, Object*** cells);

protected:

	//Texture rectangle
	static const Rect image_rect;
};