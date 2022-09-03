#include "Field.h"


int Field::seed;



Point Field::FindFreeNeighbourCell(int X, int Y)
{
    //If this cell is empty
    if (allCells[X][Y] == NULL)
    {
        return { X,Y };
    }

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

void Field::RemoveBot(int X, int Y, int energyVal)
{
    RemoveObject(X, Y);

    #ifdef SpawnOrganicWasteWhenBotDies
    if (energyVal > 0)
        AddObject(new Organics(X, Y, energyVal));
    #endif
}

void Field::RepaintBot(Bot* b, Uint8 newColor[3], int differs)
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
                        ((Bot*)tmpObj)->Repaint(newColor);
                    }
                }
            }
        }

    }
}

//Tick function for every object,
//Returns true if object was destroyed
void Field::ObjectTick(Object* tmpObj)
{
    int t = tmpObj->tick();

    if (t == 1)
    {
        //Object destroyed
        if (tmpObj->type == bot)
            RemoveBot(tmpObj->x, tmpObj->y, ((Bot*)tmpObj)->GetEnergy());
        else
            RemoveObject(tmpObj->x, tmpObj->y);

        return;
    }
    else if (t == 2)
    {
        //Skip tick
        return;
    }
    else if (tmpObj->type == bot)   //Object is a bot
    {
        int TEMP_x1 = tmpObj->x;

        Bot* tmpBot = (Bot*)tmpObj;
        BrainOutput tmpOut;
        Point lookAt = tmpBot->GetDirection();

        //Fill brain input structure
        BrainInput input;

        //Desired destination,
        //that is what bot is lookinig at
        int
        cx = tmpBot->x + lookAt.x,
        cy = tmpBot->y + lookAt.y;

        cx = ValidateX(cx);

        //If destination is out of bounds
        if (!IsInBounds(cx, cy))
        {
            //1 if unpassable
            input.vision = 1.0f;
        }
        else
        {
            //Destination cell is empty
            if (!allCells[cx][cy])
            {
                //0 if empty
                input.vision = 0.0f;
            }
            else
            {
                //Destination not empty
                switch (allCells[cx][cy]->type)
                {
                case bot:
                    //0.5 if someone in that cell
                    input.vision = .5f;

                    //Calculate how close they are as relatives, based on mutation markers
                    input.isRelative = 1.0f - ((((Bot*)allCells[cx][cy])->FindKinship(tmpBot)) * 1.0f) / (NumberOfMutationMarkers * 1.0f);
                    break;

                case rock:
                    //1.0 if cell is unpassable
                    input.vision = 1.0f;
                    break;

                case organic_waste:
                    //-.5 if cell contains organics
                    input.vision = -.5f;
                    break;

                case apple:
                    //-1.0 if cell contains an apple
                    input.vision = -1.0f;
                    break;
                }
            }
        }

        //Bot brain does its stuff
        tmpOut = tmpBot->think(input);

        //Multiply first
        for (int b = 0; b < tmpOut.divide; ++b)
        {
            //Dies if energy is too low
            if (tmpBot->GetEnergy() <= EnergyPassedToAChild + GiveBirthCost)
            {
                RemoveBot(tmpObj->x, tmpObj->y);
                return;
            }
            else
            {
                //Gives birth otherwise
                //Ignore all commented lines
                //Point freeSpace = FindFreeNeighbourCell(tmpObj->x, tmpObj->y);
                Point freeSpace;
                //Point freeSpace = { tmpObj->x - 1, tmpObj->y };
                //freeSpace.x = ValidateX(freeSpace.x);

                //if(allCells[freeSpace.x][freeSpace.y] != NULL)
                   // freeSpace = FindFreeNeighbourCell(tmpObj->x, tmpObj->y);

                //if ((tmpOut.divideDirX == 0) && (tmpOut.divideDirY == 0))
                //{
                    freeSpace = FindFreeNeighbourCell(tmpObj->x, tmpObj->y);

                    if (freeSpace.x != -1)
                    {
                        tmpBot->TakeEnergy(EnergyPassedToAChild + GiveBirthCost);
                        AddObject(new Bot(freeSpace.x, freeSpace.y, EnergyPassedToAChild, tmpBot, RandomPercent(MutationChancePercent)));
                        return;
                    }
                /* }
                else
                {
                    //freeSpace = { tmpObj->x -1, tmpObj->y };

                    cx = tmpObj->x + tmpOut.divideDirX;
                    cy = tmpObj->y + tmpOut.divideDirY;

                    if (IsInBounds(cx, cy))
                    {
                        if (allCells[cx][cy] == NULL)
                        {
                            tmpBot->TakeEnergy(EnergyPassedToAChild + GiveBirthCost);
                            AddObject(new Bot(freeSpace.x, freeSpace.y, EnergyPassedToAChild, tmpBot, RandomPercent(MutationChancePercent)));
                            return;
                        }
                    }
                }*/
            }
        }

        //Then attack
        if (tmpOut.attack)
        {
            //If dies of low energy
            if (tmpBot->TakeEnergy(AttackCost))
            {
                RemoveBot(tmpObj->x, tmpObj->y);
                return;
            }
            else
            {
                //Get direction of attack
                Point dir = tmpBot->GetDirection();

                cx = ValidateX(tmpBot->x + dir.x);
                cy = tmpBot->y + dir.y;

                if (IsInBounds(cx, cy))
                {
                    //If there is an object
                    if (allCells[cx][cy])
                    {
                        if (allCells[cx][cy]->type == bot)
                        {
                            //New rule: attack only from behind
                            /*int diff = tmpBot->GetRotationVal() - ((Bot*)allCells[cx][cy])->GetRotationVal();

                            diff = abs(diff);

                            if ((diff <= 1) || (diff >= 6))
                            {*/
                                //Kill an object
                                tmpBot->GiveEnergy(((Bot*)allCells[cx][cy])->GetEnergy(), kills);
                                RemoveBot(cx, cy);

                                tmpBot->numAttacks++;
                            //}
                        }
                        else if (allCells[cx][cy]->type == organic_waste)
                        {
                            //Eat organics
                            tmpBot->GiveEnergy(((Organics*)allCells[cx][cy])->energy, organics);
                            RemoveObject(cx, cy);
                        }
                        else if (allCells[cx][cy]->type == apple)
                        {
                            //Eat apple
                            tmpBot->GiveEnergy(((Apple*)allCells[cx][cy])->energy, organics);
                            RemoveObject(cx, cy);
                        }
                        #ifdef RockCanBeEaten
                        else if (allCells[cx][cy]->type == rock)
                        {
                            //Eat rock
                            RemoveObject(cx, cy);
                        }
                        #endif
                    }
                }
            }
        }
        /*else if(tmpOut.eatOrganicWaste)
        {
            //Now eat organics
            //Get direction 
            Point dir = tmpBot->GetDirection();

            cx = ValidateX(tmpBot->x + dir.x);
            cy = tmpBot->y + dir.y;

            if (IsInBounds(cx, cy))
            {
                //If there is an object
                if (allCells[cx][cy])
                {
                    if (allCells[cx][cy]->type == organic_waste)
                    {
                        //Eat organics
                        tmpBot->GiveEnergy(((Organics*)allCells[cx][cy])->energy, organics);
                        RemoveObject(cx, cy);
                    }
                }
            }
        }*/
        else
        {
            //Rotate after
            if (tmpOut.rotate != 0.0f)
            {
                //If dies of low energy
                if (tmpBot->TakeEnergy(RotateCost))
                {
                    RemoveBot(tmpObj->x, tmpObj->y);
                    return;
                }

                tmpBot->Rotate(tmpOut.rotate);
            }

            //Move
            if (tmpOut.move)
            {
                if (tmpBot->TakeEnergy(MoveCost))
                {
                    RemoveBot(tmpObj->x, tmpObj->y);
                    return;
                }

                Point dir = tmpBot->GetDirection();

                cx = tmpBot->x + dir.x;
                cy = tmpBot->y + dir.y;

                cx = ValidateX(cx);

                //Place object in a new place
                if(MoveObject(tmpBot, cx, cy) == 0)
                    ++tmpBot->numMoves;
                
            }
            //Photosynthesis
            else if (tmpOut.photosynthesis)
            {
                #ifdef NoPhotosynthesis
                return;
                #endif

                //If not in ocean
                //if (tmpBot->y < FieldCellsHeight - OceanHeight)
                if (tmpBot->y < (FieldCellsHeight - Bot::adaptationStep5))
                {
                    int toGive;

                    //Give energy depending on a season
                    switch (season)
                    {
                    case summer:
                        #ifdef UseSeasons
                        toGive = PhotosynthesisReward_Summer;
                        #else
                        toGive = foodBase;
                        //toGive = FindHowManyFreeCellsAround(tmpBot->x, tmpBot->y) - 3;

                        //if (toGive < 0) toGive = 0;
                        #endif
                        break;
                    case autumn: case spring:
                        toGive = PhotosynthesisReward_Autumn;
                        break;
                    case winter:
                        //toGive = (ticknum%4 == 0)?PhotosynthesisReward/8:0;
                        toGive = PhotosynthesisReward_Winter;
                        //toGive = (ticknum % 5 == 0) ? 2 : 1;
                        break;
                    }

                    tmpBot->GiveEnergy(toGive, PS);
                }
                else
                {
                    //tmpBot->GiveEnergy(foodBase/2, PS);
                #ifndef NoPhotosyntesisInOcean
                    if(RandomPercentX10(Bot::adaptationStep4))
                        tmpBot->GiveEnergy(foodBase, PS);
                #endif
                }
            }
        }
    }
    else if (tmpObj->type == organic_waste)
    {
        //Organic waste should fall until it hits an obstacle
        Organics* tmpOrg = (Organics*)tmpObj;

        int
        x = tmpOrg->x,
        y = tmpOrg->y + 1;

        //If not done falling
        if (!tmpOrg->doneFalling)
        {
            //What is underneath?
            if (IsInBounds(x, y))
            {
                if (allCells[x][y] == NULL)
                {
                    //Fall
                    MoveObject(tmpOrg, x, y);
                }
                else
                {
                    #ifndef OrganicWasteAlwaysFalls
                        tmpOrg->doneFalling = true;
                    #endif
                }
            }
            else
                tmpOrg->doneFalling = true; //Once done it shouldn't fall anymore
        }
    }
}

//tick function for single threaded build
inline void Field::tick_single_thread()
{

    Object* tmpObj;
    objectsTotal = 0;
    botsTotal = 0;

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

            ++objectCounters[index];

            if (tmpObj->type == bot)
                ++botsTotal;

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

            ++objectCounters[index];

            if (tmpObj->type == bot)
                ++botsTotal;

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
            objectCounters[i] = 0;
            botsCounters[i] = 0;
        }
    };

    objectsTotal = 0;
    botsTotal = 0;

    //Clear object counters
    clear_counters();

    //Starting signal for all threads
    StartThreads();

    //Wait for threads to synchronize first time
    WaitForThreads();

    //Add object counters
    repeat(NumThreads)
    {
        objectsTotal += objectCounters[i];
        botsTotal += botsCounters[i];
    }

    //Clear object counters
    clear_counters();

    //Starting signal for all threads
    StartThreads();

    //Wait for threads to synchronize second time
    WaitForThreads();

    //Add object counters
    repeat(NumThreads)
    {
        objectsTotal += objectCounters[i];
        botsTotal += botsCounters[i];
    }

}

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
    oceanRect.y = (FieldHeight + FieldY) - (Bot::adaptationStep5 * FieldCellSize);
    oceanRect.h = Bot::adaptationStep5 * FieldCellSize;
    SDL_RenderFillRect(renderer, &oceanRect);
#endif

    //Mud layer
#ifdef DrawMudLayer
    SDL_SetRenderDrawColor(renderer, MudColor);
    mudLayerRect.y = (FieldHeight + FieldY) - (Bot::adaptationStep7 * FieldCellSize);
    mudLayerRect.h = Bot::adaptationStep7 * FieldCellSize;
    SDL_RenderFillRect(renderer, &mudLayerRect);
#endif

    //Bots
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
                    //Draw function switch, based on selected render type
                    switch (render)
                    {
                    case natural:
                        tmpObj->draw();
                        break;
                    case predators:
                        ((Bot*)tmpObj)->drawPredators();
                        break;
                    case energy:
                        ((Bot*)tmpObj)->drawEnergy();
                        break;
                    }

                }
                else if (tmpObj->type == organic_waste)
                {
                    switch (render)
                    {
                    case energy:
                        ((Organics*)tmpObj)->drawEnergy();
                        break;
                    default:
                        tmpObj->draw();
                        break;
                    }
                }
                else
                    tmpObj->draw();

            }
        }
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


//This function is needed to tile world horizontally (change X = -1 to X = FieldCellsWidth etc.)
int Field::ValidateX(int X)
{
#ifdef TileWorldHorizontally
    if (X < 0)
    {
        return (X % FieldCellsWidth) + FieldCellsWidth;
    }
    else if (X >= FieldCellsWidth)
    {
        return (X % FieldCellsWidth);
    }
#endif      

    return X;
}

//Is cell out of bounds, given absolute screen space coordinates
bool Field::IsInBoundsScreenCoords(int X, int Y)
{
    return ((X >= mainRect.x) && (X <= mainRect.x + mainRect.w) && (Y >= mainRect.y) && (Y <= mainRect.y + mainRect.h));
}

//Transform absolute screen coords to cell position on field
Point Field::ScreenCoordsToLocal(int X, int Y)
{
    X -= FieldX;
    Y -= FieldY;

    X /= FieldCellSize;
    Y /= FieldCellSize;

    return { X, Y };
}

//Get object at certain point on field
Object* Field::GetObjectLocalCoords(int X, int Y)
{
    return allCells[X][Y];
}

//Validates if object exists
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

//How many objects on field, previous frame
uint Field::GetNumObjects()
{
    return objectsTotal;
}

//How many bots
uint Field::GetNumBots()
{
    return botsTotal;
}


/*Save / load
TODO!!!
File format:
4b - 0xfafa458e (no meaning)
4b - creature type
4b - uint num layers
4b - uint neurons in layer
4b - sizeof (Neuron)
following all neurons from first to last layer by layer
*/
bool Field::SaveObject(Object* obj, char* filename)
{
    if (obj->type != bot)
        return false;

    //Open file for writing, binary type, all contents to be deleted
    std::ofstream file(filename, std::ios::in | std::ios::binary | std::ios::trunc);

    if (file.is_open())
    {
        int i = 0xfafa458e;

        file.write((char*)&i, sizeof(int));
        i = 1;
        file.write((char*)&i, sizeof(int));
        i = NumNeuronLayers;
        file.write((char*)&i, sizeof(int));
        i = NeuronsInLayer;
        file.write((char*)&i, sizeof(int));
        i = sizeof(Neuron);
        file.write((char*)&i, sizeof(int));

        file.write((char*)((Bot*)obj)->GetNeuralNet(), NumNeuronLayers * NeuronsInLayer * sizeof(Neuron));

        file.close();

        return true;
    }

    return false;

}

bool Field::LoadObject(Object* obj, char* filename)
{
    //Open file for reading, binary type
    std::ifstream file(filename, std::ios::openmode(std::ios::in | std::ios::binary | std::ios::beg));

    if (file.is_open())
    {
        int i = 0;

        file.read((char*)&i, sizeof(int));

        if (i != 0xfafa458e)
            return false;

        file.read((char*)&i, sizeof(int));

        if (i != 1)
            return false;

        file.read((char*)&i, sizeof(int));
        if (i != NumNeuronLayers)
            return false;
        file.read((char*)&i, sizeof(int));
        if (i != NeuronsInLayer)
            return false;
        file.read((char*)&i, sizeof(int));
        if (i != sizeof(Neuron))
            return false;

        file.read((char*)((Bot*)obj)->GetNeuralNet(), NumNeuronLayers * NeuronsInLayer * sizeof(Neuron));

        file.close();

        ((Bot*)obj)->GiveInitialEnergyAndMarkers();

        return true;

    }

    return false;

}


//Spawn group of random bots
void Field::SpawnControlGroup()
{
    for (int i = 0; i < ControlGroupSize; ++i)
    {
        Bot* tmpBot = new Bot(RandomVal(FieldCellsWidth), RandomVal(FieldCellsHeight), 100);

        if (!AddObject(tmpBot))
            delete tmpBot;
    }
}

void Field::SpawnApples()
{
    Object* tmpObj;

    for (uint ix = 0; ix < FieldCellsWidth; ++ix)
    {
        for (uint iy = 0; iy < FieldCellsHeight; ++iy)
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

    threads[0] = new std::thread(&Field::ProcessPart_4Threads, this, 0, 0, FieldCellsWidth / 4, FieldCellsHeight, 0);
    threads[1] = new std::thread(&Field::ProcessPart_4Threads, this, FieldCellsWidth / 2, 0, (FieldCellsWidth / 4) * 3, FieldCellsHeight, 1);
    threads[2] = new std::thread(&Field::ProcessPart_4Threads, this, FieldCellsWidth / 4, 0, FieldCellsWidth / 2, FieldCellsHeight, 2);
    threads[3] = new std::thread(&Field::ProcessPart_4Threads, this, (FieldCellsWidth / 4) * 3, 0, FieldCellsWidth, FieldCellsHeight, 3);

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
}