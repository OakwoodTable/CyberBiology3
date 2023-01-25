#include "Organics.h"

SDL_Texture* Organics::image = NULL;


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
	SDL_SetRenderDrawColor(renderer, 255, static_cast<int>((1.0f - ((energy * 1.0f) / (MaxPossibleEnergyForABot * .5f))) * 255.0f), 0, 255);
	SDL_RenderFillRect(renderer, &object_rect);
}

Organics::Organics(int X, int Y, int Energy) : Object(X, Y)
{
	energy = Energy;
}

void Organics::SetImage(SDL_Texture* img)
{
    image = img;
}


constexpr ObjectTypes Organics::type()
{
    return organic_waste;
}

int Organics::tick()
{
    int ret = Object::tick();

    if(ret != 0)
        return ret;

    //Organic waste should fall until it hits an obstacle 

    //If not done falling
    if (!doneFalling)
    {
        int next_y = y + 1;

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
            {
                doneFalling = true;
            }
            #endif
        }
        else
        {
            doneFalling = true; //Once done it shouldn't fall anymore
        }
    }

    return 0;
}
