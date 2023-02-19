#include "Rock.h"

SDL_Texture* Rock::image;


constexpr ObjectTypes Rock::type()
{
	return rock;
}

constexpr float Rock::image_sensor_val()
{
	return -1.0f;
}

void Rock::draw()
{
	CalcScreenX();
	CalcObjectRect();

	//Draw from texture
	SDL_RenderCopy(renderer, image, &image_rect, &object_rect);
}

Rock::Rock(int X, int Y) :Object(X, Y) 
{}

void Rock::SetImage(SDL_Texture* img)
{
	image = img;
}
