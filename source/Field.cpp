
#include "Field.h"


int Field::seed;
int Field::renderX=0;

Season season;


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
                    if(allCells[tx][Y + cy]->type == bot)
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
    if (allCells[obj->x][obj->y])
        return false;

    allCells[obj->x][obj->y] = obj;

    return true;
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

    #ifdef SpawnOrganicWasteWhenBotDies
    if (energyVal > 0)
        AddObject(new Organics(X, Y, energyVal));
    #endif
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
                if (tmpObj->type == bot)
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
        if (tmpObj->type == bot)
            RemoveBot(tmpObj->x, tmpObj->y, tmpObj->energy);
        else
            RemoveObject(tmpObj->x, tmpObj->y);

        return;
    }
}

//tick function for single threaded build
inline void Field::tick_single_thread()
{
    Object* tmpObj;

    objectsTotal = 0;
    botsTotal = 0;
    applesTotal = 0;
    organicsTotal = 0;

    for (uint ix = 0; ix < FieldCellsWidth; ++ix)
    {
        for (uint iy = 0; iy < FieldCellsHeight; ++iy)
        {
            tmpObj = allCells[ix][iy];

            if (tmpObj)
            {
                ++objectsTotal;

                if (tmpObj->type == bot)
                    ++botsTotal;
                else if (tmpObj->type == apple)
                    ++applesTotal;
                else if (tmpObj->type == organic_waste)
                    ++organicsTotal;

                ObjectTick(tmpObj);
            }
        }

    }
}

//Wait for a signal 
inline void Field::ThreadWait(const uint index)
{
    for (;;)
    {
        if (threadGoMarker[index])
            return;

        if (terminateThreads)
            TerminateThread();

        std::this_thread::yield();
    }
}

//Process function for 4 threaded simulation
void Field::ProcessPart_MultipleThreads(const uint X1, const uint Y1, const uint X2, const uint Y2, const uint index)
{

    srand(seed + index);

    Object* tmpObj;

Again:

    ThreadWait(index);

    for (int X = X1; X < X1 + ((X2 - X1) / 2); ++X)
    {

        for (int Y = Y1; Y < Y2; ++Y)
        {

            tmpObj = allCells[X][Y];

            if (tmpObj == NULL)
                continue;

            ++counters[index][0];

            if (tmpObj->type == bot)
                ++counters[index][1];
            else if (tmpObj->type == apple)
                ++counters[index][2];
            else if (tmpObj->type == organic_waste)
                ++counters[index][3];

            ObjectTick(tmpObj);

        }

    }

    threadGoMarker[index] = false;

    ThreadWait(index);

    for (int X = X1 + ((X2 - X1) / 2); X < X2; ++X)
    {

        for (int Y = Y1; Y < Y2; ++Y)
        {

            tmpObj = allCells[X][Y];

            if (tmpObj == NULL)
                continue;

            ++counters[index][0];

            if (tmpObj->type == bot)
                ++counters[index][1];
            else if (tmpObj->type == apple)
                ++counters[index][2];
            else if (tmpObj->type == organic_waste)
                ++counters[index][3];

            ObjectTick(tmpObj);

        }

    }

    threadGoMarker[index] = false;

    goto Again;

}

//Start all threads
void Field::StartThreads()
{
    repeat(NumThreads)
    {
        threadGoMarker[i] = true;
    }
}

//Wait for all threads to finish their calculations
void Field::WaitForThreads()
{
    uint threadsReady;

    for (;;)
    {

        threadsReady = 0;

        repeat(NumThreads)
        {
            if (threadGoMarker[i] == false)
                threadsReady++;
        }

        if (threadsReady == NumThreads)
            break;

        std::this_thread::yield();

    }
}

//Multithreaded tick function
inline void Field::tick_multiple_threads()
{
    auto clear_counters = [&]()
    {
        repeat(NumThreads)
        {
            counters[i][0] = 0;
            counters[i][1] = 0;
            counters[i][2] = 0;
            counters[i][3] = 0;
        }
    };

    objectsTotal = 0;
    botsTotal = 0;
    applesTotal = 0;
    organicsTotal = 0;

    auto addToCounters = [&]()
    {
        repeat(NumThreads)
        {
            objectsTotal += counters[i][0];
            botsTotal += counters[i][1];
            applesTotal += counters[i][2];
            organicsTotal += counters[i][3];
        }
    };

    //Clear object counters
    clear_counters();

    //Starting signal for all threads
    StartThreads();

    //Wait for threads to synchronize first time
    WaitForThreads();

    //Add object counters
    addToCounters();

    //Clear object counters
    clear_counters();

    //Starting signal for all threads
    StartThreads();

    //Wait for threads to synchronize second time
    WaitForThreads();

    //Add object counters
    addToCounters();

}

void Field::TerminateThread() {};

//Tick function
void Field::tick(uint thisFrame)
{
    Object::currentFrame = thisFrame;

    #ifdef UseApples

        if (spawnApplesInterval++ == AppleSpawnInterval)
        {
            SpawnApples();

            spawnApplesInterval = 0;
        }
    #endif

    #ifdef UseOneThread
        tick_single_thread();
    #else
        tick_multiple_threads();
    #endif
}


//Draw simulation field with all its objects
void Field::draw(RenderTypes render)
{
    //Background
    SDL_SetRenderDrawColor(renderer, FieldBackgroundColor);
    SDL_RenderFillRect(renderer, &mainRect);

    //Ocean
#ifdef DrawOcean
    SDL_SetRenderDrawColor(renderer, OceanColor);
    oceanRect.y = (FieldHeight + FieldY) - (params.oceanLevel * FieldCellSize);
    oceanRect.h = params.oceanLevel * FieldCellSize;
    SDL_RenderFillRect(renderer, &oceanRect);
#endif

    //Mud layer
#ifdef DrawMudLayer
    SDL_SetRenderDrawColor(renderer, MudColor);
    mudLayerRect.y = (FieldHeight + FieldY) - (params.mudLevel * FieldCellSize);
    mudLayerRect.h = params.mudLevel * FieldCellSize;
    SDL_RenderFillRect(renderer, &mudLayerRect);
#endif

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
                //Draw function switch, based on selected render type
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
}

//Is cell out if bounds?
bool Field::IsInBounds(int X, int Y)
{
    return ((X >= 0) && (Y >= 0) && (X < FieldCellsWidth) && (Y < FieldCellsHeight));
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
#ifdef TileWorldHorizontally
    if (X < 0)
    {
        return X + FieldCellsWidth;
    }
    else if (X >= FieldCellsWidth)
    {
        return (X - FieldCellsWidth);
    }
#endif      

    return X;
}


bool Field::IsInBoundsScreenCoords(int X, int Y)
{
    return ((X >= mainRect.x) && (X <= mainRect.x + mainRect.w) && (Y >= mainRect.y) && (Y <= mainRect.y + mainRect.h));
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
    for (uint ix = 0; ix < FieldCellsWidth; ++ix)
    {
        for (uint iy = 0; iy < FieldCellsHeight; ++iy)
        {
            if (allCells[ix][iy] == obj)
                return true;
        }
    }

    return false;
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
    Object* tmpObj; 

    for (uint ix = 0; ix < FieldCellsWidth; ++ix)
    {
        for (uint iy = 0; iy < (FieldCellsHeight - params.oceanLevel); ++iy)
        {

            tmpObj = allCells[ix][iy];

            if (tmpObj == NULL)
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


//Create field
Field::Field()
{
    //Clear array
    memset(allCells, 0, sizeof(Point*) * FieldCellsWidth * FieldCellsHeight);

    //Spawn objects
    #ifdef SpawnControlGroupAtStart
        SpawnControlGroup();
    #endif

    #ifdef SpawnOneAtStart
        Bot* tmpBot = new Bot(80, 60, MaxPossibleEnergyForABot);

        AddObject(tmpBot);
    #endif

    //Start threads

    //4 threads
    #ifdef UseFourThreads
    repeat(NumThreads)
        threadGoMarker[i] = false;

    threads[0] = new std::thread(&Field::ProcessPart_MultipleThreads, this, 0, 0, FieldCellsWidth / 4, FieldCellsHeight, 0);
    threads[1] = new std::thread(&Field::ProcessPart_MultipleThreads, this, FieldCellsWidth / 2, 0, (FieldCellsWidth / 4) * 3, FieldCellsHeight, 1);
    threads[2] = new std::thread(&Field::ProcessPart_MultipleThreads, this, FieldCellsWidth / 4, 0, FieldCellsWidth / 2, FieldCellsHeight, 2);
    threads[3] = new std::thread(&Field::ProcessPart_MultipleThreads, this, (FieldCellsWidth / 4) * 3, 0, FieldCellsWidth, FieldCellsHeight, 3);

    #endif

    //8 threads
    #ifdef UseEightThreads
    repeat(NumThreads)
        threadGoMarker[i] = false;

    threads[0] = new std::thread(&Field::ProcessPart_MultipleThreads, this, 0, 0, FieldCellsWidth / 8, FieldCellsHeight, 0);
    threads[1] = new std::thread(&Field::ProcessPart_MultipleThreads, this, FieldCellsWidth / 8, 0, FieldCellsWidth / 4, FieldCellsHeight, 1);
    threads[2] = new std::thread(&Field::ProcessPart_MultipleThreads, this, FieldCellsWidth / 4, 0, (FieldCellsWidth / 8) * 3, FieldCellsHeight, 2);
    threads[3] = new std::thread(&Field::ProcessPart_MultipleThreads, this, (FieldCellsWidth / 8) * 3, 0, FieldCellsWidth / 2, FieldCellsHeight, 3);
    threads[4] = new std::thread(&Field::ProcessPart_MultipleThreads, this, FieldCellsWidth / 2, 0, (FieldCellsWidth / 8) * 5, FieldCellsHeight, 4);
    threads[5] = new std::thread(&Field::ProcessPart_MultipleThreads, this, (FieldCellsWidth / 8) * 5, 0, (FieldCellsWidth / 4) * 3, FieldCellsHeight, 5);
    threads[6] = new std::thread(&Field::ProcessPart_MultipleThreads, this, (FieldCellsWidth / 4) * 3, 0, (FieldCellsWidth / 8) * 7, FieldCellsHeight, 6);
    threads[7] = new std::thread(&Field::ProcessPart_MultipleThreads, this, (FieldCellsWidth / 8) * 7, 0, FieldCellsWidth, FieldCellsHeight, 7);
    #endif

    Object::SetPointers(this, (Object***)allCells);

}