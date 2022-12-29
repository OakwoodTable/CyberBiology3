#pragma once
//#pragma message("   Field_h")


class Object;
class Bot;
class Apple;
class Rock;
class Organics;
class ObjectSaver;



#include "Object.h"
#include "Bot.h"
#include "Apple.h"
#include "Rock.h"
#include "Organics.h"

#include "ObjectSaver.h"

#include "SDL.h"


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

struct FieldDynamicParams
{
    int oceanLevel;
    int mudLevel;
    int appleEnergy;

    int adaptation_DeathChance_Winds;
    int adaptation_StepsNum_Winds;

    int adaptation_landBirthBlock;
    int adaptation_seaBirthBlock;
    int adaptation_PSInOceanBlock;
    int adaptation_PSInMudBlock;
    int adaptation_botShouldBeOnLandOnceToMultiply;
    int adaptation_botShouldDoPSOnLandOnceToMultiply;
    int adaptation_forceBotMovements;

    int adaptation_organicSpawnRate;

    bool spawnApples;

    int reserved[38];

    void Reset();

    FieldDynamicParams();
};



//Simulation field class
class Field final
{
    //All cells as 2d array
    Object* allCells[FieldCellsWidth][FieldCellsHeight];

    //Rectangles
    const SDL_Rect mainRect = { FieldX , FieldY, FieldWidth, FieldHeight };

    SDL_Rect oceanRect = { FieldX , FieldY + (FieldHeight - (InitialOceanHeight * FieldCellSize)),
        FieldWidth, InitialOceanHeight * FieldCellSize };

    SDL_Rect mudLayerRect = { FieldX , FieldY + (FieldHeight - (InitialMudLayerHeight * FieldCellSize)),
        FieldWidth, InitialMudLayerHeight * FieldCellSize };

    //Needed to calculate number of active objects and bots (calculated on every frame)
    uint objectsTotal = 0;
    uint botsTotal = 0;
    uint applesTotal = 0;
    uint organicsTotal = 0;

    //Apple spawn timer
    uint spawnApplesInterval = 0;

    //threads
    abool threadGoMarker[NumThreads];
    std::thread* threads[NumThreads];
    uint counters[NumThreads][4];
    abool threadTerminated[NumThreads];
    abool terminateThreads = false;
    abool pauseThreads = false;


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

    //Wait for a signal 
    inline void ThreadWait(const uint index);
    

public:

    void shiftRenderPoint(int cx);

    void jumpToFirstBot();

    int photosynthesisReward = FoodbaseInitial;


    //Move objects from one cell to another
    int MoveObject(int fromX, int fromY, int toX, int toY);
    int MoveObject(Object* obj, int toX, int toY);

    bool AddObject(Object* obj);

    //Remove object and delete object class
    void RemoveObject(int X, int Y);
    void RemoveAllObjects();

    //Remove a bot (same as remove object but for a bot)
    void RemoveBot(int X, int Y, int energyVal = 0);

    //Repaint bot
    void RepaintBot(Bot* b, Color newColor, int differs = 1);

    //Tick function for every object
    void ObjectTick(Object* tmpObj);

    //Tick function
    void tick(uint thisFrame);

    //Draw simulation field with all its objects
    void draw(RenderTypes render = natural);

    //Is cell out if bounds?
    bool IsInBounds(int X, int Y);
    bool IsInBounds(Point p);

    bool IsInWater(int Y);
    bool IsInMud(int Y);

    //Find empty cell nearby, otherwise return {-1, -1}
    Point FindFreeNeighbourCell(int X, int Y);

    Point FindRandomNeighbourBot(int X, int Y);

    //How may free cells are available around a given one
    int FindHowManyFreeCellsAround(int X, int Y);

    //This function is needed to tile world horizontally (change X = -1 to X = FieldCellsWidth etc.)
    int ValidateX(int X);

    //Is cell out of bounds, given absolute screen space coordinates
    bool IsInBoundsScreenCoords(int X, int Y);

    //Transform absolute screen coords to cell position on field
    Point ScreenCoordsToLocal(int X, int Y);

    Object* GetObjectLocalCoords(int X, int Y);

    bool ValidateObjectExistance(Object* obj);


    //How many objects on field, prev. frame
    uint GetNumObjects();
    uint GetNumBots();
    uint GetNumApples();
    uint GetNumOrganics();

    //Spawn group of random bots
    void SpawnControlGroup();
    void SpawnApples();

    void PauseThreads();
    void UnpauseThreads();

    Field();
    ~Field();

    static int seed;
    static int renderX;

    FieldDynamicParams params;
};

