#pragma once
//#pragma message("	Apple_h")


#include "Field.h"


class Apple :public Object
{
private:

	static SDL_Texture* image;

public:

	void draw() override;

	Apple(int X, int Y, int Energy = 0);


	static void CreateImage();
	static void DeleteImage();

};