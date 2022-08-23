#pragma once


#include "Bot.h"
#include "Rock.h"
#include "Organics.h"
#include "Apple.h"


//Don't touch
#define NumThreads 1
#ifdef UseFourThreads
#undef NumThreads
#define NumThreads 4
#endif
#ifdef UseEightThreads
#undef NumThreads
#define NumThreads 8
#endif



enum RenderTypes
{
    natural,
    predators,
    energy,
    noRender
};

enum Season
{
    summer,
    autumn,
    winter,
    spring
};

extern Season season;



//Simulation field class
class Field
{
    //All cells as 2d array
    Object* allCells[FieldCellsWidth][FieldCellsHeight];

    //Rectangles
    const SDL_Rect mainRect = { FieldX , FieldY, FieldWidth, FieldHeight };
    SDL_Rect oceanRect = { FieldX , FieldY + (FieldHeight - (OceanHeight * FieldCellSize)), FieldWidth, OceanHeight * FieldCellSize };
    SDL_Rect mudLayerRect = { FieldX , FieldY + (FieldHeight - (MudLayerHeight * FieldCellSize)), FieldWidth, MudLayerHeight * FieldCellSize };

    //Needed to calculate number of active objects and bots (calculated on every frame)
    uint objectsTotal = 0;
    uint botsTotal = 0;

    //Apple spawn timer
    uint spawnApplesInterval = 0;

    //threads
    bool threadGoMarker[NumThreads];
    std::thread* threads[NumThreads];
    uint objectCounters[NumThreads];
    uint botsCounters[NumThreads];
    bool terminateThreads = false;


    //Find empty cell nearby, otherwise return {-1, -1}
    Point FindFreeNeighbourCell(int X, int Y);

    //How may free cells are available around given one
    int FindHowManyFreeCellsAround(int X, int Y);

    //tick function for single threaded build
    inline void tick_single_thread();

    //Process function for multi threaded simulation
    void ProcessPart_MultipleThreads(const uint X1, const uint Y1, const uint X2, const uint Y2, const uint index);

    //Start all threads
    void StartThreads();

    //Wait for all threads to finish their calculations
    void WaitForThreads();

    //Multithreaded tick function
    inline void tick_multiple_threads();

    //Terminator function
    [[noreturn]] static void TerminateThread() {};

    //Wait for a signal 
    inline void ThreadWait(const uint index);
    

public:

    //Number of food for photosynthesis and other means
    int foodBase = FoodbaseInitial;


    //Move objects from one cell to another
    int MoveObject(int fromX, int fromY, int toX, int toY);
    int MoveObject(Object* obj, int toX, int toY);

    //Add new object
    bool AddObject(Object* obj);

    //Remove object and delete object class
    void RemoveObject(int X, int Y);

    //Remove a bot (same as remove object but for a bot)
    void RemoveBot(int X, int Y, int energyVal = 0);

    //Repaint bot
    void RepaintBot(Bot* b, Uint8 newColor[3], int differs = 1);

    //Tick function for every object,
    //Returns true if object was destroyed
    void ObjectTick(Object* tmpObj);

    //Tick function
    void tick(uint thisFrame);

    //Draw simulation field with all its objects
    void draw(RenderTypes render = natural);

    //Is cell out if bounds?
    bool IsInBounds(int X, int Y);
    bool IsInBounds(Point p);


    //This function is needed to tile world horizontally (change X = -1 to X = FieldCellsWidth etc.)
    int ValidateX(int X);

    //Is cell out of bounds, given absolute screen space coordinates
    bool IsInBoundsScreenCoords(int X, int Y);

    //Transform absolute screen coords to cell position on field
    Point ScreenCoordsToLocal(int X, int Y);

    //Get object at certain point on field
    Object* GetObjectLocalCoords(int X, int Y);

    //Validates if object exists
    bool ValidateObjectExistance(Object* obj);

    //How many objects on field, last frame
    uint GetNumObjects();

    //Same for bots
    uint GetNumBots();


    /*Save / load
    TODO!!!
    File format:
    4b - 0xfafa458e (magic number)
    4b - creature type 
    4b - uint num layers
    4b - uint neurons in layer
    4b - sizeof (Neuron)
    following all neurons from first to last layer by layer
    */
    bool SaveObject(Object* obj, char* filename);
    bool LoadObject(Object* obj, char* filename);


    //Spawn group of random bots
    void SpawnControlGroup();


    //Spawn apples
    void SpawnApples();


    //Create field
    Field();

    static int seed;

};

