#include "Apple.h"



//Temporary, very slow
void RenderFilledCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int
            _x = radius - w,
            _y = radius - h; 

            //Pythagorean theorem
            if ((_x * _x + _y * _y) <= (radius * radius))
            {
                //Draw using dots
                SDL_RenderDrawPoint(renderer, x + _x, y + _y);
            }
        }
    }
}


void Apple::draw()
{
	//Draw
	SDL_SetRenderDrawColor(renderer, AppleDrawColor);
	
    RenderFilledCircle(renderer, FieldX + x * FieldCellSize + FieldCellSizeHalf, FieldY + y * FieldCellSize + FieldCellSizeHalf, FieldCellSizeHalf);
}

Apple::Apple(int X, int Y, int Energy): Object(X, Y), energy(Energy)
{
    if (energy == 0)
        energy = DefaultAppleEnergy;

    type = apple;
}
