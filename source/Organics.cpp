#include "Organics.h"

SDL_Texture* Organics::image = NULL;


void Organics::CreateImage()
{
    SDL_Surface* surf;

    //Create bot 'body' image (white rectangle)

    //Another surface
    surf = SDL_CreateRGBSurface(0, FieldCellSize, FieldCellSize, 32,
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

    SDL_LockSurface(surf);

    //Clear surface
    SDL_memset(surf->pixels, 0xFF, surf->w * surf->h * 4);

    //Create pixels array
    std::vector<uint32_t> pixels(surf->h * surf->w, 0x00000000);

    //Draw outlined rectangle 

    for (int _x = 1; _x < FieldCellSize - 1; ++_x)
    {
        for (int _y = 1; _y < FieldCellSize - 1; ++_y)
        {
            pixels[(_y * surf->w + _x)] = OrganicWasteOutlineColor;
        }
    }

    for (int _x = 2; _x < FieldCellSize - 2; ++_x)
    {
        for (int _y = 2; _y < FieldCellSize - 2; ++_y)
        {
            pixels[(_y * surf->w + _x)] = OrganicWasteDrawColor;
        }
    }

    //Copy pixels on surface
    SDL_memcpy(surf->pixels, pixels.data(), surf->w * surf->h * 4);

    SDL_UnlockSurface(surf);

    //Create static texture
    image = SDL_CreateTextureFromSurface(renderer, surf);

    //Free surface
    SDL_FreeSurface(surf);

    //Free render target
    SDL_SetRenderTarget(renderer, NULL);
}

void Organics::DeleteImage()
{
    SDL_DestroyTexture(image);
}


void Organics::draw()
{
    CalcScreenX();
    CalcObjectRect();

    //Draw from texture
    SDL_RenderCopy(renderer, image, &image_rect, &object_rect);
}

void Organics::drawEnergy()
{
    CalcScreenX();
    CalcObjectRectShrinked(2);

	//Draw
	SDL_SetRenderDrawColor(renderer, 255, (1.0f - ((energy * 1.0f) / (MaxPossibleEnergyForABot * .5f))) * 255.0f, 0, 255);
	SDL_RenderFillRect(renderer, &object_rect);
}

Organics::Organics(int X, int Y, int Energy) :Object(X, Y)
{
	energy = Energy;

	type = organic_waste;
}


int Organics::tick()
{
    int ret = Object::tick();

    if(ret != 0)
        return ret;

    //Organic waste should fall until it hits an obstacle

    int next_y = y + 1;

    //If not done falling
    if (!doneFalling)
    {
        //What is underneath?
        if (pField->IsInBounds(x, next_y))
        {
            if ((*pCells)[x][next_y] == NULL)
            {
                //Fall
                pField->MoveObject(this, x, next_y);
            }
            #ifndef OrganicWasteAlwaysFalls
            else                
                doneFalling = true;
            #endif
        }
        else
        {
            doneFalling = true; //Once done it shouldn't fall anymore
        }
    }

    return 0;
}
