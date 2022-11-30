#pragma once
//#pragma message("	Organics_h")


#include "Field.h"


class Organics :public Object
{

	bool doneFalling = false;

	static SDL_Texture* image;

public:

	int tick() override;

	void draw() override;
	void drawEnergy() override;


	Organics(int X, int Y, int Energy);


	static void CreateImage();
	static void DeleteImage();

};

