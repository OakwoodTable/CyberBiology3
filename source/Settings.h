#pragma once




//-----------------------------------------------------------------
//OpenGL

#define GLSL_version "#version 130"

#define SDL_glCont_Major 3
#define SDL_glCont_Minor 2

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//Window

#define WindowCaption "CyberBio v1.2.1"

#define WindowWidth 1920
#define WindowHeight 1080

#define BackgroundColorFloat 0.0f,0.0f,0.0f,255.0f

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//System

#define RandomSeed
//#define Seed 0



//choose only 1 option------
//#define UseOneThread
//#define UseFourThreads
#define UseEightThreads
//--------------------------

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
#define GUI_FPSWhenNoRender 10

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

#define Keyboard_ShowSaveLoad_Window SDL_SCANCODE_Z
#define Keyboard_ShowDangerous_Window SDL_SCANCODE_X
#define Keyboard_ShowAdaptation_Window SDL_SCANCODE_C
#define Keyboard_ShowChart_Window SDL_SCANCODE_V
#define Keyboard_ShowBrain_Window SDL_SCANCODE_B

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

#define FieldCellsWidth 13*16*6		//ƒолжно делитьс€ на 8 без остатка если нужны 4 потока! » на 16 без остатка если 8 потоков!
#define FieldCellsHeight 133

#define FieldRenderCellsWidth 202

#define FieldCellSize 8
#define FieldCellSizeHalf FieldCellSize/2
#define FieldWidth FieldCellSize*FieldRenderCellsWidth
#define FieldHeight FieldCellSize*FieldCellsHeight

//-----------------------------------------------------------------



//-----------------------------------------------------------------
//Drawing

#define DrawBotOutline
#define DrawBotHead

#define BotOutlineColor 111,111,111,255

#define FieldBackgroundColor 255,255,255,255

#define RockDrawColor 62, 62, 62, 255
#define OrganicWasteDrawColor 0xC8C8C8FF
#define OrganicWasteOutlineColor 0x808080FF
#define AppleDrawColorRGBA 0x006400FF

#define DrawOcean
#define OceanColor 150,150,255,255		//Blue water
//#define OceanColor 100,255,100,255	//Green water

#define DrawMudLayer
#define MudColor 140,80,62,255

#define RenderTypeAtStart natural

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//Simulation

#define FPSLimitAtStart 60
#define LimitFPSAtStart 60

#define StartOnPause

//#define SpawnOneAtStart
//#define SpawnControlGroupAtStart
#define ControlGroupSize 8000

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//World rules

#define FoodbaseInitial 6
#define PhotosynthesisReward_Summer 4
#define PhotosynthesisReward_Autumn 2
#define PhotosynthesisReward_Winter 1

#define MaxPossibleEnergyForABot 500
#define MaxBotLifetime 2000

#define NewbornGetsHalf
#define EnergyPassedToAChild 100

#define EveryTickEnergyPenalty 0
#define AttackCost 4
#define MoveCost 1
#define RotateCost 0
#define GiveBirthCost 10

#define StunAfterBirth 1	//How many turns a creature cannot act after his birth
#define FertilityDelay 0	//Delay before next birth

//#define BotCanEatRock
#define BotCanEatBot

#define OrganicWasteAlwaysFalls

#define DefaultAppleEnergy 100
#define AppleSpawnInterval 1
#define SpawnAppleInCellChance 1

//#define NoPhotosynthesis
//#define ForbidMutations

#define MutationChancePercent 20
//#define UseTotalMutation
#define TotalMutationChancePercentX10 0
//#define RandomColorChancePercentX10 4
#define ChangeColorSlightly
#define BotColorChangeStrength 3

#define PresetRandomColors
//#define RandomColorEveryNewMarkersSet

//#define UseSeasons
#define ChangeSeasonInterval 4000

#define NumberOfMutationMarkers 3
#define HowMuchDifferenseCantBeTold 1

//#define BotDiesIfEnergyOverflow

#define InitialOceanHeight 65
//#define NoPhotosyntesisInOcean

#define InitialMudLayerHeight 25

//-----------------------------------------------------------------



//-----------------------------------------------------------------
//Save and load

#define DirectoryName "SavedObjects/"

#define MagicNumber_ObjectFile 0xfcfa47fe
#define MagicNumber_WorldFile 0xbffa51fd

//-----------------------------------------------------------------




//-----------------------------------------------------------------
//Neural net

#define NeuronsInLayer 5
#define NumNeuronLayers 6
#define NeuronOutputLayerIndex (NumNeuronLayers-1)
#define NeuronInputLayerIndex 0

//#define FullyConnected

#define MaxConnections_Basic 4
#define MaxConnections_Input 4
#define MaxConnections_Random 1
#define MaxConnections_RadialBasis 4
#define MaxConnections_Memory 4

#define UseMemoryNeuron
#define UseRandomNeuron

#define NeuronBiasMin -1.0f
#define NeuronBiasMax 1.0f

#define ChangeBiasMax 0.2f
#define ChangeBiasMin 0.05f

#define ChangeConnectionWeightMax 0.2f
#define ChangeConnectionWeightMin 0.05f

#define ConnectionWeightMin -2.0f
#define ConnectionWeightMax 2.0f

#define MutateNeuronsMaximum 8
//#define MutateNeuronsSlightlyMaximum 2

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

//-----------------------------------------------------------------




//-----------------------------------------------------------------
// Population chart window

#define ChartNumValues 250
#define AddToChartEvery 125

#define ChartLineThickness 1.5f

//-----------------------------------------------------------------
