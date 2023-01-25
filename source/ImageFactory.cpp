#include "ImageFactory.h"


SDL_Texture* ImageFactory::apple = NULL;
SDL_Texture* ImageFactory::organics = NULL;
SDL_Texture* ImageFactory::rock = NULL;

SDL_Texture* ImageFactory::bot_body = NULL;
SDL_Texture* ImageFactory::bot_head[8] = {};


SDL_Texture* ImageFactory::TurnSurfaceToStaticTexture(SDL_Surface* surf)
{
    SDL_Texture* toRet = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_SetTextureBlendMode(apple, SDL_BLENDMODE_BLEND);

    SDL_FreeSurface(surf);

    return toRet;
}

void ImageFactory::DrawRectOnSurface(vector<uint32_t>& pixels, Rect rect, int surf_w, int color)
{
    for (int _x = rect.x; _x < rect.w; ++_x)
    {
        for (int _y = rect.y; _y < rect.h; ++_y)
        {
            pixels[(_y * surf_w + _x)] = color;
        }
    }
}

void ImageFactory::CreateApple()
{
    //Create surface
    SDL_Surface* surf = SDL_CreateRGBSurface(0, FieldCellSize, FieldCellSize, 32,
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

    //Lock
    SDL_LockSurface(surf);

    //Create pixels array
    vector<uint32_t> pixels(surf->h * surf->w, 0x00000000);

    //Draw circle
    for (int _x = 0; _x < FieldCellSize; ++_x)
    {
        for (int _y = 0; _y < FieldCellSize; ++_y)
        {
            int
            x_dist = FieldCellSizeHalf - _x,
            y_dist = FieldCellSizeHalf - _y;

            //Pythagorean theorem
            if (x_dist * x_dist + y_dist * y_dist <= FieldCellSizeHalf * FieldCellSizeHalf)
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
    apple = TurnSurfaceToStaticTexture(surf);

    //Set apple texture
    Apple::SetImage(apple);
}

void ImageFactory::CreateOrganics()
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
    vector<uint32_t> pixels(surf->h * surf->w, 0x00000000);

    //Draw outlined rectangle 
    DrawRectOnSurface(pixels, {1, 1, FieldCellSize - 1 , FieldCellSize - 1 }, surf->w, OrganicWasteOutlineColor);
    DrawRectOnSurface(pixels, {2, 2, FieldCellSize - 2 , FieldCellSize - 2 }, surf->w, OrganicWasteDrawColor);

    //Copy pixels on surface
    SDL_memcpy(surf->pixels, pixels.data(), surf->w * surf->h * 4);

    SDL_UnlockSurface(surf);

    //Create static texture
    organics = TurnSurfaceToStaticTexture(surf);

    Organics::SetImage(organics);
}

void ImageFactory::CreateRock()
{
    SDL_Surface* surf;

    //Another surface
    surf = SDL_CreateRGBSurface(0, FieldCellSize, FieldCellSize, 32,
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

    SDL_LockSurface(surf);

    //Clear surface
    SDL_memset(surf->pixels, 0xFF, surf->w * surf->h * 4);

    //Create pixels array
    vector<uint32_t> pixels(surf->h * surf->w, 0x00000000);

    //Draw rectangle 
    DrawRectOnSurface(pixels, { 1, 1, FieldCellSize - 1 , FieldCellSize - 1 }, surf->w, RockDrawColor);

    //Copy pixels on surface
    SDL_memcpy(surf->pixels, pixels.data(), surf->w * surf->h * 4);

    SDL_UnlockSurface(surf);

    //Create static texture
    rock = TurnSurfaceToStaticTexture(surf);

    Rock::SetImage(rock);
}

void ImageFactory::CreateBot()
{
    SDL_Surface* surf;
    SDL_Texture* tmpTexTarget;
    SDL_Texture* tmpTex2;

    Rect image_rect = {0, 0, FieldCellSize, FieldCellSize };

    //Create rotation sprites (outline and head positions)
    repeat(8)
    {
        //Target texture
        tmpTexTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, FieldCellSize, FieldCellSize);

        //Set render target
        SDL_SetRenderTarget(renderer, tmpTexTarget);

        //Clear texture
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        //Render outline
        #ifdef DrawBotOutline
        {
            SDL_SetRenderDrawColor(renderer, BotOutlineColor);
            SDL_RenderDrawRect(renderer, &image_rect);
        }
        #endif

        //Render head
        #ifdef DrawBotHead
        {
            SDL_RenderDrawLine(renderer,
                FieldCellSizeHalf,
                FieldCellSizeHalf,
                FieldCellSizeHalf + Rotations[i].x * FieldCellSizeHalf,
                FieldCellSizeHalf + Rotations[i].y * FieldCellSizeHalf);
        }
        #endif

        //Create surface
        surf = SDL_CreateRGBSurface(0, FieldCellSize, FieldCellSize, 32,
            0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

        //Copy previously rendered texture on surface
        SDL_LockSurface(surf);

        int32_t pixels[FieldCellSize * FieldCellSize];

        SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, pixels, surf->pitch);

        SDL_memcpy(surf->pixels, pixels, surf->w * surf->h * 4);

        SDL_UnlockSurface(surf);

        //Create static texture
        tmpTex2 = SDL_CreateTextureFromSurface(renderer, surf);

        //Save texture
        bot_head[i] = tmpTex2;

        Bot::SetHeadImages(bot_head);

        //Free target texture and surface
        SDL_DestroyTexture(tmpTexTarget);
        SDL_FreeSurface(surf);
    }

    //Create bot 'body' image (white rectangle)

    //Another surface
    surf = SDL_CreateRGBSurface(0, FieldCellSize, FieldCellSize, 32,
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

    //Clear surface
    SDL_LockSurface(surf);
    SDL_memset(surf->pixels, 255, surf->w * surf->h * 4);
    SDL_UnlockSurface(surf);

    //Create static texture
    bot_body = SDL_CreateTextureFromSurface(renderer, surf);

    Bot::SetBodyImage(bot_body);

    //Free surface
    SDL_FreeSurface(surf);

    //Free render target
    SDL_SetRenderTarget(renderer, NULL);
}

void ImageFactory::CreateImages()
{
	CreateApple();
    CreateOrganics();
    CreateRock();
    CreateBot();
}

void ImageFactory::DeleteImages()
{
    SDL_DestroyTexture(apple);
    SDL_DestroyTexture(organics);
    SDL_DestroyTexture(rock);

    repeat(8)
        SDL_DestroyTexture(bot_head[i]);

    SDL_DestroyTexture(bot_body);
}
