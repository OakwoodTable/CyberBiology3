#include "Field.h"


Point Field::FindFreeNeighbourCell(int X, int Y)
{
    //If cell itself is empty
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
    if (tmpObj->tick())
    {
        //Object destroyed
        RemoveObject(tmpObj->x, tmpObj->y);
        return;
    }
    else if (tmpObj->type == bot)   //Object is a bot
    {
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
                    input.vision = 0.5f;

                    //Calculate how close they are as relatives, based on mutation markers
                    input.isRelative = 1.0f - ((((Bot*)allCells[cx][cy])->FindKinship(tmpBot)) * 1.0f) / (NumberOfMutationMarkers * 1.0f);
                    break;
                }
            }
        }

        //Bot brain does its stuff
        tmpOut = tmpBot->think(input);

        //IGNORE NEXT LINES
        //Temporary!!! TODO
        //bool minerals = ((tmpOut.fields[0] == 1) && (tmpOut.fields[1] == 1) && (tmpOut.fields[2] == 1) && (tmpOut.fields[3] == 0) && (tmpOut.fields[4] == 0));
        bool minerals = false;
        //if (minerals == true)
            //goto InsaneJump;
        //bool minerals = ((tmpOut.fields[0] == 0) && (tmpOut.fields[1] == 0) && (tmpOut.fields[2] == 1) && (tmpOut.fields[3] == 0));

        //Multiply first
        for (int b = 0; b < tmpOut.divide; ++b)
        {
            //Dies if energy is too low
            if (tmpBot->GetEnergy() <= EnergyPassedToAChild + GiveBirthCost)
            {
                RemoveObject(tmpObj->x, tmpObj->y);
                return;
            }
            else
            {
                //Gives birth otherwise
                Point freeSpace = FindFreeNeighbourCell(tmpObj->x, tmpObj->y);

                if (freeSpace.x != -1)
                {
                    tmpBot->TakeEnergy(EnergyPassedToAChild + GiveBirthCost);
                    AddObject(new Bot(freeSpace.x, freeSpace.y, EnergyPassedToAChild, tmpBot, RandomPercent(MutationChancePercent)));
                    return;
                }
            }
        }

        //Then attack
        if (tmpOut.attack)
        {
            //If dies of low energy
            if (tmpBot->TakeEnergy(AttackCost))
            {
                RemoveObject(tmpObj->x, tmpObj->y);
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
                        //Kill an object
                        tmpBot->GiveEnergy(((Bot*)allCells[cx][cy])->GetEnergy(), kills);
                        RemoveObject(cx, cy);
                    }
                }
            }
        }
        else
        {
            //Rotate after
            if (tmpOut.rotate != 0.0f)
            {
                //If dies of low energy
                if (tmpBot->TakeEnergy(RotateCost))
                {
                    RemoveObject(tmpObj->x, tmpObj->y);
                    return;
                }

                tmpBot->Rotate(tmpOut.rotate);
            }

            //Move
            if (tmpOut.move)
            {
                if (tmpBot->TakeEnergy(MoveCost))
                {
                    RemoveObject(tmpObj->x, tmpObj->y);
                    return;
                }

                Point dir = tmpBot->GetDirection();

                cx = tmpBot->x + dir.x;
                cy = tmpBot->y + dir.y;

                cx = ValidateX(cx);

                //Place object in a new place
                MoveObject(tmpBot, cx, cy);
            }
            //Photosynthesis
            else if (tmpOut.photosynthesis)
            {
                //InsaneJump: <- ignore this
                // 
                //If not in ocean
                if (tmpBot->y < FieldCellsHeight - OceanHeight)
                {
                    if (minerals)
                        return;

                    int toGive;

                    //Give energy depending on a season
                    switch (season)
                    {
                    case summer:
#ifdef ChangeSeasons
                        toGive = PhotosynthesisReward_Summer;
#else
                        toGive = foodBase;
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
                else  //not used atm
                {
                    if (minerals)
                    {
                        tmpBot->GiveEnergy(foodBase, mineral);
                    }
                }
            }
        }
    }
}

//tick function for single threaded build
inline void Field::tick_single_thread()
{

    Object* tmpObj;
    objectsTotal = 0;

    for (uint ix = 0; ix < FieldCellsWidth; ++ix)
    {
        for (uint iy = 0; iy < FieldCellsHeight; ++iy)
        {
            tmpObj = allCells[ix][iy];

            if (tmpObj)
            {
                ++objectsTotal;
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
void Field::ProcessPart_4Threads(const uint X1, const uint Y1, const uint X2, const uint Y2, const uint index)
{

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
            objectCounters[i] = 0;
    };

    objectsTotal = 0;

    //Clear object counters
    clear_counters();

    //Starting signal for all 4 threads
    StartThreads();

    //Wait for threads to synchronize first time
    WaitForThreads();

    //Add object counters
    repeat(NumThreads)
        objectsTotal += objectCounters[i];

    //Clear object counters
    clear_counters();

    //Starting signal for all 4 threads
    StartThreads();

    //Wait for threads to synchronize second time
    WaitForThreads();

    //Add object counters
    repeat(NumThreads)
        objectsTotal += objectCounters[i];

}

//Tick function
void Field::tick()
{
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
    SDL_RenderFillRect(renderer, &oceanRect);
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

//How many objects on field, last frame
uint Field::GetNumObjects()
{
    return objectsTotal;
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
        i = sizeof Neuron;
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
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::beg);

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
        if (i != sizeof Neuron)
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
#ifdef UseFourThreads
    repeat(NumThreads)
        threadGoMarker[i] = false;

    threads[0] = new std::thread(&Field::ProcessPart_4Threads, this, 0, 0, FieldCellsWidth / 4, FieldCellsHeight, 0);
    threads[1] = new std::thread(&Field::ProcessPart_4Threads, this, FieldCellsWidth / 2, 0, (FieldCellsWidth / 4) * 3, FieldCellsHeight, 1);
    threads[2] = new std::thread(&Field::ProcessPart_4Threads, this, FieldCellsWidth / 4, 0, FieldCellsWidth / 2, FieldCellsHeight, 2);
    threads[3] = new std::thread(&Field::ProcessPart_4Threads, this, (FieldCellsWidth / 4) * 3, 0, FieldCellsWidth, FieldCellsHeight, 3);

#endif
#ifdef UseEightThreads
    repeat(NumThreads)
        threadGoMarker[i] = false;

    threads[0] = new std::thread(&Field::ProcessPart_4Threads, this, 0, 0, FieldCellsWidth / 8, FieldCellsHeight, 0);
    threads[1] = new std::thread(&Field::ProcessPart_4Threads, this, FieldCellsWidth / 8, 0, FieldCellsWidth / 4, FieldCellsHeight, 1);
    threads[2] = new std::thread(&Field::ProcessPart_4Threads, this, FieldCellsWidth / 4, 0, (FieldCellsWidth / 8) * 3, FieldCellsHeight, 2);
    threads[3] = new std::thread(&Field::ProcessPart_4Threads, this, (FieldCellsWidth / 8) * 3, 0, FieldCellsWidth / 2, FieldCellsHeight, 3);
    threads[4] = new std::thread(&Field::ProcessPart_4Threads, this, FieldCellsWidth / 2, 0, (FieldCellsWidth / 8) * 5, FieldCellsHeight, 4);
    threads[5] = new std::thread(&Field::ProcessPart_4Threads, this, (FieldCellsWidth / 8) * 5, 0, (FieldCellsWidth / 4) * 3, FieldCellsHeight, 5);
    threads[6] = new std::thread(&Field::ProcessPart_4Threads, this, (FieldCellsWidth / 4) * 3, 0, (FieldCellsWidth / 8) * 7, FieldCellsHeight, 6);
    threads[7] = new std::thread(&Field::ProcessPart_4Threads, this, (FieldCellsWidth / 8) * 7, 0, FieldCellsWidth, FieldCellsHeight, 7);
#endif
}