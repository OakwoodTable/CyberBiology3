#pragma once
//#pragma message("	Apple_h")


#include "Field.h"


class Apple :public Object
{
private:

	static SDL_Texture* image;


public:

	constexpr ObjectTypes type() override;
	constexpr float image_sensor_val() override;
	void draw() override;


	Apple(int X, int Y, int Energy = 0);

	static void SetImage(SDL_Texture* img);

};