#include "Organics.h"




void Organics::draw()
{
    CalcScreenX();
    CalcObjectRectShrinked(1);

	//Draw
	SDL_SetRenderDrawColor(renderer, OrganicWateDrawColor);
	SDL_RenderFillRect(renderer, &object_rect);
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
            doneFalling = true; //Once done it shouldn't fall anymore
    }

    return 0;
}
