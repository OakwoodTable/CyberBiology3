#pragma once
//#pragma message("	Organics_h")


#include "Field.h"


class Organics :public Object
{
private:

	bool doneFalling = false;

	static SDL_Texture* image;

public:

	constexpr ObjectTypes type() override;
	int tick() override;
	void draw() override;
	void drawEnergy() override;


	Organics(int X, int Y, int Energy);

	static void SetImage(SDL_Texture* img);
};

