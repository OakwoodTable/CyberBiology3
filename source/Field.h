#pragma once
//#pragma message("   Field_h")

class Object;
class Bot;
class Apple;
class Rock;
class Organics;
class ObjectSaver;


#include "Settings.h"
#include "Utils.h"

#include "Object.h"
#include "Bot.h"
#include "Apple.h"
#include "Rock.h"
#include "Organics.h"

#include "ObjectSaver.h"
#include "Chart.h"
#include "ImageFactory.h"



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

constexpr const char* SeasonNames[] =
{
    "summer",
    "autumn",
    "winter",
    "spring"
};



struct FieldDynamicParams
{
    int botMaxLifetime;
    int botMaxEnergy;
    int fertility_delay;

    int oceanLevel;
    int mudLevel;    

    bool spawnApples;
    int appleEnergy;

    int adaptation_StepsNumToDivide_Winds;

    int adaptation_landBirthBlock;
    int adaptation_seaBirthBlock;
    int adaptation_PSInOceanBlock;
    int adaptation_PSInMudBlock;
    int adaptation_botShouldDoPSOnLandOnceToMultiply;    

    int adaptation_organicSpawnRate;

    int adaptation_forceBotMovementsY;
    int adaptation_forceBotMovementsX;

    bool noPredators;
    bool noMutations;

    int PSreward;

    bool useSeasons;
    int seasonInterval;

    int reserved[31];


    void Reset();

    FieldDynamicParams();
};



//Simulation field class
class Field final
{

private:

    //All cells as 2d array
    Object* allCells[FieldCellsWidth][FieldCellsHeight];

    //Rectangles
    const SDL_Rect mainRect = { FieldX , FieldY, FieldWidth, FieldHeight };

    SDL_Rect oceanRect = { FieldX , FieldY + (FieldHeight - (InitialOceanHeight * FieldCellSize)),
        FieldWidth, InitialOceanHeight * FieldCellSize };

    SDL_Rect mudLayerRect = { FieldX , FieldY + (FieldHeight - (InitialMudLayerHeight * FieldCellSize)),
        FieldWidth, InitialMudLayerHeight * FieldCellSize };


    //Needed to calculate number of active objects (calculated on every frame)
    uint objectsTotal = 0;
    uint botsTotal = 0;
    uint applesTotal = 0;
    uint organicsTotal = 0;
    uint predatorsTotal = 0;
    uint averageLifetime = 0;

    uint spawnApplesCounter = 0;

    //Seasons
    Season season = summer;
    uint changeSeasonCounter = 0;

    void SeasonTick();
    void ChangeSeason();

    //Multithreading
    uint objectCounters[NumThreads][6]; 
    atomic_flag threadsGo;
    atomic_flag threadsReady[NumThreads];
    bool terminateThreads = false;    

    void StartThreads();

    inline void tick_single_thread();
    inline void tick_multiple_threads();

    //Process function for multi threaded simulation
    void ProcessPart_MultipleThreads(const uint X1, const uint X2, const uint index);  

    //Tick function for every object
    void ObjectTick(Object* tmpObj);
    

public:

    FieldDynamicParams params;

    void shiftRenderPoint(int cx);
    void jumpToFirstBot();
    void mutateWorld();
    void placeWall(uint width = 2);

    //Move objects from one cell to another
    int MoveObject(int fromX, int fromY, int toX, int toY);
    int MoveObject(Object* obj, int toX, int toY);

    bool AddObject(Object* obj);
    void ObjectAddOrReplace(Object* obj);

    //Remove object and delete object class
    void RemoveObject(int X, int Y);
    void RemoveAllObjects();

    //Remove a bot (same as remove object but for a bot)
    void RemoveBot(int X, int Y, int energyVal = 0);

    //Repaint bot
    void RepaintBot(Bot* b, Color newColor, int differs = 1);

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
    int FindDistanceX(int X1, int X2);

    //Is cell out of bounds, given absolute screen space coordinates
    bool IsInBoundsScreenCoords(int X, int Y);

    //Transform absolute screen coords to cell position on field
    Point ScreenCoordsToLocal(int X, int Y);

    Object* GetObjectLocalCoords(int X, int Y);

    bool ValidateObjectExistance(Object* obj);

    uint GetNumObjects();
    uint GetNumBots();
    uint GetNumApples();
    uint GetNumOrganics();
    uint GetNumPredators();
    uint GetAverageLifetime();

    //Spawn group of random bots
    void SpawnControlGroup();
    void SpawnApples();
        
    Season GetSeason();
    uint GetSeasonCounter();

    Field();
    ~Field();


    static int seed;
    static int renderX;
};

