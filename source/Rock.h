#pragma once
//#pragma message("	Rock_h")


#include "Field.h"


class Rock :public Object
{

private:

	static SDL_Texture* image;


public:

	constexpr ObjectTypes type() override;
	void draw() override; 


	Rock(int X, int Y);

	static void SetImage(SDL_Texture* img);

};