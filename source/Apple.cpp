#include "Apple.h"

SDL_Texture* Apple::image = NULL;



constexpr ObjectTypes Apple::type()
{
    return apple;
}

constexpr float Apple::image_sensor_val()
{
    return 1.5f;
}

void Apple::draw()
{
    CalcScreenX();
    CalcObjectRect();	

    //Draw from texture
    SDL_RenderCopy(renderer, image, &image_rect, &object_rect);
}

Apple::Apple(int X, int Y, int Energy): Object(X, Y)
{
    energy = Energy;

    if (energy == 0)
        energy = pField->params.appleEnergy;
}

void Apple::SetImage(SDL_Texture* img)
{
    image = img;
}
