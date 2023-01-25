#include "Field.h"


int Field::seed;
int Field::renderX=0;


void Field::ChangeSeason()
{
    season = (Season)((int)season + 1);

    if (season > spring)
    {
        season = summer;
    }
}

void Field::SeasonTick()
{
    if (++changeSeasonCounter >= (uint)params.seasonInterval)
    {
        ChangeSeason();

        changeSeasonCounter = 0;
    }
}

Season Field::GetSeason()
{
    return season;
}

uint Field::GetSeasonCounter()
{
    return changeSeasonCounter;
}

void Field::shiftRenderPoint(int cx)
{
    renderX += cx;

    if (renderX < 0)
    {
        renderX = FieldCellsWidth - 1;
    }
    else if (renderX >= FieldCellsWidth)
    {
        renderX = 0;
    }
}

void Field::jumpToFirstBot()
{
    Object* obj;

    for (int X = 0; X < FieldCellsWidth; ++X)
    {
        for (int Y = 0; Y < FieldCellsHeight; ++Y)
        {
            obj = allCells[X][Y];

            if (obj)
            {
                if (obj->type() == bot)
                {
                    renderX = X;

                    return;
                }
            }
        }
    }
}


Point Field::FindFreeNeighbourCell(int X, int Y)
{
    //If this cell is empty
    if (allCells[X][Y] == NULL)
    {
        return { X,Y };
    }

    //Form an array of nearby free cells

    int tx;
    Point tmpArray[9];
    int i = 0;

    for (int cx = -1; cx < 2; ++cx)
    {
        for (int cy = -1; cy < 2; ++cy)
        {
            tx = ValidateX(X + cx);

            if (IsInBounds(tx, Y + cy))
            {
                if (allCells[tx][Y + cy] == NULL)
                {
                    tmpArray[i++].Set(tx, Y + cy);
                }
            }
        }
    }

    //Get random free cell from array
    if (i > 0)
    {
        return tmpArray[RandomVal(i)];
    }

    //No free cells nearby
    return { -1, -1 };
}

Point Field::FindRandomNeighbourBot(int X, int Y)
{
    int tx;
    Point tmpArray[9];
    int i = 0;

    for (int cx = -1; cx < 2; ++cx)
    {
        for (int cy = -1; cy < 2; ++cy)
        {
            tx = ValidateX(X + cx);

            if (IsInBounds(tx, Y + cy))
            {
                if (allCells[tx][Y + cy] != NULL)
                {
                    if(allCells[tx][Y + cy]->type() == bot)
                        tmpArray[i++].Set(tx, Y + cy);
                }
            }
        }
    }

    //Get random bot from array
    if (i > 0)
    {
        return tmpArray[RandomVal(i)];
    }

    //No free cells nearby
    return { -1, -1 };
}

int Field::FindHowManyFreeCellsAround(int X, int Y)
{
    int toRet = 0;

    //If cell itself is empty
    if (allCells[X][Y] == NULL)
    {
        ++toRet;
    }

    //Parse all cells
    int tx;

    for (int cx = -1; cx < 2; ++cx)
    {
        for (int cy = -1; cy < 2; ++cy)
        {

            tx = ValidateX(X + cx);

            if (IsInBounds(tx, Y + cy))
            {
                if (allCells[tx][Y + cy] == NULL)
                {
                    ++toRet;
                }
            }
        }
    }

    return toRet;
}


int Field::MoveObject(int fromX, int fromY, int toX, int toY)
{
    if (!IsInBounds(toX, toY))
        return -2;

    if (allCells[toX][toY])
        return -1;

    Object* tmpObj = allCells[fromX][fromY];

    if (tmpObj)
    {
        allCells[toX][toY] = tmpObj;
        allCells[fromX][fromY] = NULL;

        tmpObj->x = toX;
        tmpObj->y = toY;

        return 0;
    }

    return -3;
}

int Field::MoveObject(Object* obj, int toX, int toY)
{
    return MoveObject(obj->x, obj->y, toX, toY);
}

bool Field::AddObject(Object* obj)
{
    Object** cell = &allCells[obj->x][obj->y];

    if (*cell)
        return false;

    *cell = obj;

    return true;
}

void Field::ObjectAddOrReplace(Object* obj)
{
    Object** cell = &allCells[obj->x][obj->y];

    if (*cell)
        delete *cell;

    *cell = obj;
}

void Field::RemoveObject(int X, int Y)
{
    Object* tmpO = allCells[X][Y];

    if (tmpO)
    {
        delete tmpO;

        allCells[X][Y] = NULL;
    }
}

void Field::RemoveAllObjects()
{
    for (int cx = 0; cx < FieldCellsWidth; ++cx)
    {
        for (int cy = 0; cy < FieldCellsHeight; ++cy)
        {
            RemoveObject(cx, cy);
        }
    }
}

void Field::RemoveBot(int X, int Y, int energyVal)
{
    RemoveObject(X, Y);

    if(RandomPercentX10(params.adaptation_organicSpawnRate))
    {
        if (energyVal > 0)
            AddObject(new Organics(X, Y, energyVal));
    }
}

void Field::RepaintBot(Bot* b, Color newColor, int differs)
{
    Object* tmpObj;

    for (uint ix = 0; ix < FieldCellsWidth; ++ix)
    {
        for (uint iy = 0; iy < FieldCellsHeight; ++iy)
        {
            tmpObj = allCells[ix][iy];

            if (tmpObj)
            {
                if (tmpObj->type() == bot)
                {
                    if (((Bot*)tmpObj)->FindKinship(b) >= (NumberOfMutationMarkers - differs))
                    {
                        ((Bot*)tmpObj)->SetColor(newColor);
                    }
                }
            }
        }

    }
}


void Field::ObjectTick(Object* tmpObj)
{
    int t = tmpObj->tick();

    if (t == 1)
    {
        //Object destroyed
        if (tmpObj->type() == bot)
            RemoveBot(tmpObj->x, tmpObj->y, tmpObj->energy);
        else
            RemoveObject(tmpObj->x, tmpObj->y);

        return;
    }
}

void Field::NotifyThreads()
{
    mut.lock();
    {
        memset(threadsReady, 0, sizeof(threadsReady));
    }
    mut.unlock();

    cond.notify_all();
}


inline void Field::tick_single_thread()
{
    Object* tmpObj;

    objectsTotal = 0;
    botsTotal = 0;
    applesTotal = 0;
    organicsTotal = 0;
    predatorsTotal = 0;
    averageLifetime = 0;

    for (uint ix = 0; ix < FieldCellsWidth; ++ix)
    {
        for (uint iy = 0; iy < FieldCellsHeight; ++iy)
        {
            tmpObj = allCells[ix][iy];

            if (tmpObj)
            {
                ++objectsTotal;

                if (tmpObj->type() == bot)
                {
                    ++botsTotal;

                    if (((Bot*)tmpObj)->isPredator())
                    {
                        ++predatorsTotal;
                    }

                    averageLifetime += tmpObj->GetLifetime();
                }
                else if (tmpObj->type() == apple)
                    ++applesTotal;
                else if (tmpObj->type() == organic_waste)
                    ++organicsTotal;

                ObjectTick(tmpObj);
            }
        }

    }

    if(botsTotal > 0)
        averageLifetime /= botsTotal;
}


void Field::ProcessPart_MultipleThreads(const uint X1, const uint X2, const uint index)
{
    srand(seed + index);

    auto obj_calc = [&](Object* tmpObj)
    {
        if (tmpObj == NULL)
            return;

        ++objectCounters[index][0];

        if (tmpObj->type() == bot)
        {
            ++objectCounters[index][1];

            if (((Bot*)tmpObj)->isPredator())
            {
                ++objectCounters[index][4];
            }

            objectCounters[index][5] += tmpObj->GetLifetime();
        }
        else if (tmpObj->type() == apple)
            ++objectCounters[index][2];
        else if (tmpObj->type() == organic_waste)
            ++objectCounters[index][3];

        ObjectTick(tmpObj);
    };

    auto lifetime_calc = [&]()
    {
        uint c = objectCounters[index][1];

        if (c > 0)
            objectCounters[index][5] /= c;
    };

    auto wait = [&]()
    {
        mut.lock();
        cond.wait(mut, [&] {return !threadsReady[index]; });
        mut.unlock();
    };

    auto thisThreadIsReady = [&]()
    {
        mut.lock();
        threadsReady[index] = true;
        mut.unlock();
    };

    const uint chunkWidth = ((X2 - X1) / 2);
    const uint _X[2] = {X1, X1 + chunkWidth};

    for(;!terminateThreads;)
    {
        repeat(2)
        {
            wait();
            {
                for (uint X = _X[i]; X < _X[i] + chunkWidth; ++X)
                {
                    for (uint Y = 0; Y < FieldCellsHeight; ++Y)
                    {
                        obj_calc(allCells[X][Y]);
                    }
                }

                lifetime_calc();
                thisThreadIsReady();
            }

            cond.notify_all();
        }
    }
}

void Field::StartThreads()
{
    repeat(NumThreads)
    {
        uint X1 = (FieldCellsWidth / NumThreads) * i;
        uint X2 = (FieldCellsWidth / NumThreads) * (i + 1);

        thread t(&Field::ProcessPart_MultipleThreads, this, X1, X2, i);

        t.detach();
    }
}


inline void Field::tick_multiple_threads()
{
    auto waitAllThreads = [&]()
    {
        for (bool& b : threadsReady)
        {
            if (!b) return false;
        }

        return true;
    };

    objectsTotal = 0;
    botsTotal = 0;
    applesTotal = 0;
    organicsTotal = 0;
    predatorsTotal = 0;
    averageLifetime = 0;

    //2 passes
    repeat(2)
    {
        //Clear object counters
        memset(objectCounters, 0, sizeof(objectCounters));

        //Starting signal for all threads
        NotifyThreads();

        //Wait for threads to synchronize
        mut.lock();
        cond.wait(mut, waitAllThreads);
        mut.unlock();

        //Add object counters
        repeat(NumThreads)
        {
            objectsTotal += objectCounters[i][0];
            botsTotal += objectCounters[i][1];
            applesTotal += objectCounters[i][2];
            organicsTotal += objectCounters[i][3];
            predatorsTotal += objectCounters[i][4];
            averageLifetime += objectCounters[i][5];
        }
    }

    averageLifetime /= NumThreads * 2;
}


void Field::tick(uint thisFrame)
{
    //Change season
    if (params.useSeasons)
        SeasonTick();

    //Memorize frame number
    Object::currentFrame = thisFrame;

    //Spawn apples
    if(params.spawnApples)
    {
        if (spawnApplesCounter++ == AppleSpawnInterval)
        {
            SpawnApples();

            spawnApplesCounter = 0;
        }
    }

    //Make simulation step
    #if NumThreads == 1
        tick_single_thread();
    #else
        tick_multiple_threads();
    #endif
}



void Field::draw(RenderTypes render)
{
    //Background
    SDL_SetRenderDrawColor(renderer, FieldBackgroundColor);
    SDL_RenderFillRect(renderer, &mainRect);

    //Mud layer
    SDL_SetRenderDrawColor(renderer, MudColor);
    mudLayerRect.y = (FieldHeight + FieldY) - (params.mudLevel * FieldCellSize);
    mudLayerRect.h = params.mudLevel * FieldCellSize;
    SDL_RenderFillRect(renderer, &mudLayerRect);

    //Ocean
    SDL_SetRenderDrawColor(renderer, OceanColor);
    oceanRect.y = (FieldHeight + FieldY) - (params.oceanLevel * FieldCellSize);
    oceanRect.h = (params.oceanLevel * FieldCellSize) - mudLayerRect.h;
    SDL_RenderFillRect(renderer, &oceanRect);

    //Objects
    Object* tmpObj;
    int ix = renderX;

    for (uint i = 0; i < FieldRenderCellsWidth; ++i)
    {
        for (uint iy = 0; iy < FieldCellsHeight; ++iy)
        {

            if (ix >= FieldCellsWidth)
                ix -= FieldCellsWidth;

            tmpObj = allCells[ix][iy];

            if (tmpObj)
            {
                switch (render)
                {
                case natural:
                    tmpObj->draw();
                    break;
                case predators:
                    tmpObj->drawPredators();
                    break;
                case energy:
                    tmpObj->drawEnergy();
                    break;
                }
            }
        }

        ++ix;
    }

    //Underwater mask
    #ifdef DrawUnderwaterMask
    {
        SDL_SetRenderDrawColor(renderer, UnderwaterMaskColor);
        oceanRect.h = params.oceanLevel * FieldCellSize;
        SDL_RenderFillRect(renderer, &oceanRect);
    }
    #endif

}

bool Field::IsInBounds(int X, int Y)
{
    return ((X >= 0) and (Y >= 0) and (X < FieldCellsWidth) and (Y < FieldCellsHeight));
}

bool Field::IsInBounds(Point p)
{
    return IsInBounds(p.x, p.y);
}

bool Field::IsInWater(int Y)
{
    return (Y >= (FieldCellsHeight - params.oceanLevel));
}

bool Field::IsInMud(int Y)
{
    return (Y >= (FieldCellsHeight - params.mudLevel));
}

int Field::ValidateX(int X)
{
    if (X < 0)
    {
        return X + FieldCellsWidth;
    }
    else if (X >= FieldCellsWidth)
    {
        return (X - FieldCellsWidth);
    }

    return X;
}

int Field::FindDistanceX(int X1, int X2)
{
    uint minDistX1 = min(X1, (FieldCellsWidth - X1));
    uint minDistX2 = min(X2, (FieldCellsWidth - X2));
    uint crossDist = abs(X2 - X1);

    return min(crossDist, minDistX1 + minDistX2);
}

bool Field::IsInBoundsScreenCoords(int X, int Y)
{
    return ((X >= mainRect.x) and (X <= mainRect.x + mainRect.w) and (Y >= mainRect.y) and (Y <= mainRect.y + mainRect.h));
}

Point Field::ScreenCoordsToLocal(int X, int Y)
{
    X -= FieldX;
    Y -= FieldY;

    X /= FieldCellSize;
    Y /= FieldCellSize;

    X += renderX;

    X = ValidateX(X);

    return { X, Y };
}

Object* Field::GetObjectLocalCoords(int X, int Y)
{
    return allCells[X][Y];
}

bool Field::ValidateObjectExistance(Object* obj)
{
    return (allCells[obj->x][obj->y] == obj);
}

uint Field::GetNumObjects()
{
    return objectsTotal;
}

uint Field::GetNumBots()
{
    return botsTotal;
}

uint Field::GetNumApples()
{
    return applesTotal;
}

uint Field::GetNumOrganics()
{
    return organicsTotal;
}

uint Field::GetNumPredators()
{
    return predatorsTotal;
}

uint Field::GetAverageLifetime()
{
    return averageLifetime;
}

void Field::SpawnControlGroup()
{
    for (int i = 0; i < ControlGroupSize; ++i)
    {
        Bot* tmpBot = new Bot(RandomVal(FieldCellsWidth), RandomVal(FieldCellsHeight), MaxPossibleEnergyForABot);

        if (!AddObject(tmpBot))
            delete tmpBot;
    }
}


void Field::SpawnApples()
{
    for (uint ix = 0; ix < FieldCellsWidth; ++ix)
    {
        for (uint iy = 0; iy < (FieldCellsHeight - (uint)params.oceanLevel); ++iy)
        {
            if (allCells[ix][iy] == NULL)
            {
                //Take a chance to spawn an apple
                if (RandomPercentX10(SpawnAppleInCellChance))
                {
                    AddObject(new Apple(ix, iy));
                }
            }
        }
    }
}


Field::Field()
{
    //Clear array
    memset(allCells, 0, sizeof(Point*) * FieldCellsWidth * FieldCellsHeight);    

    #if NumThreads!=1
    {
        StartThreads();
    }
    #endif

    Object::SetPointers(this, (Object***)allCells);

}

Field::~Field()
{
    terminateThreads = true;
}

void FieldDynamicParams::Reset()
{
    oceanLevel = InitialOceanHeight;
    mudLevel = InitialMudLayerHeight;
    appleEnergy = DefaultAppleEnergy;

    spawnApples = false;

    botMaxLifetime = MaxBotLifetimeInitial;

    adaptation_StepsNumToDivide_Winds = 0;

    adaptation_landBirthBlock = 0;
    adaptation_seaBirthBlock = 0;
    adaptation_PSInOceanBlock = 0;
    adaptation_PSInMudBlock = 0;
    adaptation_botShouldBeOnLandOnceToMultiply = 0;
    adaptation_botShouldDoPSOnLandOnceToMultiply = 0;
    adaptation_forceBotMovementsY = 0;

    adaptation_organicSpawnRate = 0;

    adaptation_forceBotMovementsX = 0;

    noPredators = false;
    noMutations = false;

    fertility_delay = FertilityDelayInitial;

    PSreward = PSRewardInitial;

    useSeasons = false;
    seasonInterval = 2000;

    memset(reserved, 0, sizeof(reserved));
}

FieldDynamicParams::FieldDynamicParams()
{
    Reset();
}
