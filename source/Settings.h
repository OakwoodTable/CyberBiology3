#pragma once

#include "Utils.h"


//-----------------------------------------------------------------
//OpenGL

#define GLSL_version "#version 130"

#define SDL_glCont_Major 3
#define SDL_glCont_Minor 2

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//Window

#define WindowCaption "CyberBio v1.3.1"

#define WindowWidth 1920
#define WindowHeight 1080

#define BackgroundColorFloat 0.0f, 0.0f, 0.0f, 255.0f

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//System

#define RandomSeed
//#define Seed 0

//Number of threads (only 1, 4, 8, 16, 24 or 48)
#define NumThreads 48

//-----------------------------------------------------------------



//-----------------------------------------------------------------
//GUI

//#define ShowGUIDemoWindow

#define InterfaceBorder 10

#define GUIWindowWidth 284

#define LogBackgroundColor 0.1f, 0.1f, 0.3f, 1.0f

#define DefaultBrushRadius 1
#define GUI_Max_tps 180
#define GUI_Max_food 25
#define GUI_Max_fps 120
#define GUI_Max_brush 50

#define CursorBlinkRate 2
#define GUI_FPSWhenNoRender 20

#define RepaintTolerance 2

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//Keyboard

#define Keyboard_Pause SDL_SCANCODE_PAUSE
#define Keyboard_Pause2 SDL_SCANCODE_SPACE
#define Keyboard_NextFrame SDL_SCANCODE_KP_PLUS

#define Keyboard_RenderNatural SDL_SCANCODE_KP_1
#define Keyboard_RenderPredators SDL_SCANCODE_KP_2
#define Keyboard_RenderEnergy SDL_SCANCODE_KP_3
#define Keyboard_NoRender SDL_SCANCODE_KP_4

#define Keyboard_SpawnRandoms SDL_SCANCODE_F1
#define Keyboard_PlaceWall SDL_SCANCODE_F2
#define Keyboard_DropOrganics SDL_SCANCODE_F3
#define Keyboard_SpawnRocks SDL_SCANCODE_F4

#define Keyboard_Quicksave SDL_SCANCODE_F5
#define Keyboard_Quickload SDL_SCANCODE_F9

#define Keyboard_MutateScreen SDL_SCANCODE_F11

#define Keyboard_ShowSaveLoad_Window SDL_SCANCODE_Z
#define Keyboard_ShowDangerous_Window SDL_SCANCODE_X
#define Keyboard_ShowAdaptation_Window SDL_SCANCODE_C
#define Keyboard_ShowChart_Window SDL_SCANCODE_V
#define Keyboard_ShowBrain_Window SDL_SCANCODE_B
#define Keyboard_ShowAutomaticAdaptation_Window SDL_SCANCODE_N

#define Keyboard_Reset_RenderX SDL_SCANCODE_HOME
#define Keyboard_Jump_Up_RenderX SDL_SCANCODE_PAGEUP
#define Keyboard_Jump_Down_RenderX SDL_SCANCODE_PAGEDOWN
#define Keyboard_Jump_To_First_bot SDL_SCANCODE_END

#define MoveCameraSpeed 1
#define MoveCameraFastSpeed 6
#define MoveCameraJump 125

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//Field

#define FieldX InterfaceBorder
#define FieldY InterfaceBorder

#define FieldCellSize 8
#define FieldCellSizeHalf FieldCellSize/2

//constexpr uint ScreenCellsWidth = ((WindowWidth - InterfaceBorder * 2 - GUIWindowWidth) / FieldCellSize);
#define ScreenCellsWidth 12*16

//ƒолжно делитьс€ на 8 без остатка если нужны 4 потока! » на 16 без остатка если 8 потоков и т.д.!
#define FieldCellsWidth ScreenCellsWidth*30
#define FieldCellsHeight 133

#define FieldRenderCellsWidth 202

#if ScreenCellsWidth < FieldRenderCellsWidth
	#undef FieldRenderCellsWidth
	#define FieldRenderCellsWidth ScreenCellsWidth
#endif

#define FieldWidth FieldCellSize*FieldRenderCellsWidth
#define FieldHeight FieldCellSize*FieldCellsHeight

//-----------------------------------------------------------------



//-----------------------------------------------------------------
//Drawing

#define DrawBotOutline
#define DrawBotHead

#define BotOutlineColor 111,111,111,255

#define FieldBackgroundColor 255,255,255,255

#define RockDrawColor 0x626262ff
#define OrganicWasteDrawColor 0xC8C8C8FF
#define OrganicWasteOutlineColor 0x808080FF
#define AppleDrawColorRGBA 0x006400FF

#define OceanColor 150,150,255,255		//Blue water
//#define OceanColor 100,255,100,255	//Green water
#define MudColor 140,80,62,255	

#define DrawUnderwaterMask
#define UnderwaterMaskColor 100, 100, 255, 80

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//Simulation

#define TPSLimitAtStart 60
#define LimitFPSAtStart 60

#define ControlGroupSize 2000
#define SpawnRocksSize FieldCellsWidth

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//World rules

//#define BotDiesIfEnergyOverflow
#define BotMaxEnergyInitial 500

#define BotDiesOfOldAge
#define MaxBotLifetimeInitial 200
#define MaxBotLifetime_Min 100
#define MaxBotLifetime_Max 10000

#define EveryTickEnergyPenalty 0
#define AttackCost 10
#define EatOrganicsCost 8
#define MoveCost 1
#define RotateCost 0
#define GiveBirthCost 10

//How many turns a creature cannot act after his birth
#define StunAfterBirth 0	
//Delay before next birth
#define FertilityDelayInitial 0		

//#define BotCanEatRock
#define BotCanEatBot

#define OrganicWasteAlwaysFalls

#define DefaultAppleEnergy 100
#define AppleSpawnInterval 1
#define SpawnAppleInCellChance 1

#define MutationChancePercent 20

//#define UseRandomColorOccasionaly
//#define RandomColorChancePercentX10 2

#define ChangeColorSlightly
#define BotColorChangeMin 3
#define BotColorChangeMax 12

#define PresetRandomColors
//#define RandomColorEveryNewMarkersSet

#define PSRewardInitial 6
#define PS_Multiplier_Summer 1.0f
#define PS_Multiplier_AutumnSpring .5f
#define PS_Multiplier_Winter .25f

#define NumberOfMutationMarkers 4
#define HowMuchDifferenseCantBeTold 1

#define InitialOceanHeight 65
#define InitialMudLayerHeight 25

//-----------------------------------------------------------------



//-----------------------------------------------------------------
//Save and load

#define DirectoryName "SavedObjects/"

#define OuicksaveFilename "SavedObjects/Quicksave"

#define MagicNumber_ObjectFile 0xfcfa47fe
#define MagicNumber_WorldFile 0xbffd52fd

//-----------------------------------------------------------------



//-----------------------------------------------------------------
// Population chart window

#define ChartNumValues 250
#define AddToChartEvery 125

#define ChartLineThickness 2.0f

#define ChartBotsColor 1, 1, 0, 1
#define ChartApplesColor 0, 1, 0, 1
#define ChartOrganicsColor 0, 0, 1, 1
#define ChartPredatorsColor 1, 0, 0, 1
#define ChartAVGLifetimeColor 1, 1, 1, 1

//-----------------------------------------------------------------



//-----------------------------------------------------------------
//Neural net

//Only change this
constexpr uint neuronsInLayer[] =
{
	8,
	6,
	6,
	6,
	6,
	8
};

constexpr uint NumNeuronsInLayerMax = neuronsInLayer[0];
constexpr uint NumInputNeurons = neuronsInLayer[0];
constexpr uint NumNeuronLayers = static_cast<uint>(std::size(neuronsInLayer));

constexpr uint NeuronOutputLayerIndex = (NumNeuronLayers-1);
constexpr uint NeuronInputLayerIndex = 0;

constexpr uint NumHiddenNeurons = neuronsInLayer[1];
constexpr uint NumOutputNeurons = neuronsInLayer[NeuronOutputLayerIndex];

constexpr uint NumNeuronsTotal = (NumInputNeurons + NumOutputNeurons + (NumHiddenNeurons * (NumNeuronLayers - 2)));

//#define FullyConnected

#define MaxConnections_Basic 3
#define MaxConnections_Input 3
#define MaxConnections_Random 1
#define MaxConnections_RadialBasis 3
#define MaxConnections_Memory 3

#define UseMemoryNeuron
#define UseRandomNeuron

#define BiasMultiplier 0.01f
#define BiasValueCorrespondingTo_1 100

constexpr char NeuronBiasMin = -100;
constexpr char NeuronBiasMax = 100;

#define ChangeBiasMax 10
#define ChangeBiasMin 2

#define WeightMultiplier 0.02f
#define WeightValueCorrespondingTo_1 50

#define ChangeConnectionWeightMax 5
#define ChangeConnectionWeightMin 1

constexpr char ConnectionWeightMin = -100;
constexpr char ConnectionWeightMax = 100;

#define MutateNeuronsMaximum (NumNeuronsTotal/6)	// (NumNeuronsTotal/8) is acceptable too	

//-----------------------------------------------------------------



//-----------------------------------------------------------------
//Neural net renderer (bot brain window)

#define Render_PositiveWeightColor 185, 0, 0
#define Render_NegativeWeightColor 0, 0, 185
#define Render_NeutralWeightColor 100, 100, 100

#define Render_GreyThreshold 0.15f

#define Render_NeuronSize 30
#define Render_LayerDistance 100
#define Render_VerticalDistance 60

#define Render_LineThickness 7


constexpr const char* inputLayerCaptions[] =
{
	"energy",
	"age",
	"rotation",
	"height",
	"area",
	"eye1",
	"is relative",
	"is looking at me"
	//"eye2"	
};

constexpr const char* outputLayerCaptions[] =
{
	"desired_rotation_x",
	"desired_rotation_y",

	"move",
	"photosynthesis",
	"attack",
	"digestOrganics",

	"divide_num",
	"divide_energy"
};


//-----------------------------------------------------------------