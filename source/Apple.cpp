#include "Apple.h"

SDL_Texture* Apple::image = NULL;


void Apple::CreateImage()
{
    //Create surface
    SDL_Surface* surf = SDL_CreateRGBSurface(0, FieldCellSize, FieldCellSize, 32,
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

    //Lock
    SDL_LockSurface(surf);

    //Create pixels array
    std::vector<uint32_t> pixels(surf->h * surf->w, 0x00000000);
    
    //Draw circle

    for (int _x = 0; _x < FieldCellSize; ++_x)
    {
        for (int _y = 0; _y < FieldCellSize; ++_y)
        {
            int
            x_dist = FieldCellSizeHalf - _x,
            y_dist = FieldCellSizeHalf - _y;

            //Pythagorean theorem
            if (x_dist*x_dist + y_dist*y_dist <= FieldCellSizeHalf * FieldCellSizeHalf)
            {
                pixels[(_y * surf->w + _x)] = AppleDrawColorRGBA;
            }
        }
    }

    //Copy pixels on surface
    SDL_memcpy(surf->pixels, pixels.data(), surf->w * surf->h * 4);

    //Unlock
    SDL_UnlockSurface(surf);   
    
    //Set blend mode
    SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_NONE);

    //Create new texture
    image = SDL_CreateTextureFromSurface(renderer, surf);
    
    //Destroy surface
    SDL_FreeSurface(surf);
    
    //Set texture blend mode
    SDL_SetTextureBlendMode(image, SDL_BLENDMODE_BLEND);
}

void Apple::DeleteImage()
{
    SDL_DestroyTexture(image);
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

    type = apple;
}
