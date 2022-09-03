#pragma once




//-----------------------------------------------------------------
//OpenGL

#define GLSL_version "#version 130"

#define SDL_glCont_Major 3
#define SDL_glCont_Minor 2

//-----------------------------------------------------------------


//-----------------------------------------------------------------
//Window
#define WindowCaption "CyberBio v1.0.5"

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

#define GUIWindowWidth 300

#define ConsoleCharLength 4000 //Temporary (Forever)

#define DefaultBrushRadius 7
#define GUI_Max_interval 1000
#define GUI_Max_skip 40
#define GUI_Max_food 10
#define GUI_Max_brush 50

#define CursorBlinkRate 4
#define SkipRenderingFramesWhileOnPause 10
#define SkipGUIFramesWhenNoRender 40
//-----------------------------------------------------------------


//-----------------------------------------------------------------
//Keyboard
#define Keyboard_Pause SDL_SCANCODE_SPACE
#define Keyboard_SpawnRandoms SDL_SCANCODE_BACKSPACE
#define Keyboard_NextFrame SDL_SCANCODE_KP_PLUS
#define Keyboard_RenderNatural SDL_SCANCODE_KP_1
#define Keyboard_RenderPredators SDL_SCANCODE_KP_2
#define Keyboard_RenderEnergy SDL_SCANCODE_KP_3
#define Keyboard_NoRender SDL_SCANCODE_KP_4
//-----------------------------------------------------------------


//-----------------------------------------------------------------
//Field
#define FieldBackgroundColor 255,255,255,255

#define FieldX InterfaceBorder
#define FieldY InterfaceBorder

#define FieldCellsWidth 192		//������ �������� �� 8 ��� ������� ���� ����� 4 ������! � �� 16 ��� ������� ���� 8 �������!
#define FieldCellsHeight 133

#define FieldCellSize 8
#define FieldCellSizeHalf FieldCellSize/2
#define FieldWidth FieldCellSize*FieldCellsWidth
#define FieldHeight FieldCellSize*FieldCellsHeight

#define TileWorldHorizontally
//-----------------------------------------------------------------



//-----------------------------------------------------------------
//Drawing
#define DrawBotOutline
#define DrawBotHead

#define BotOutlineColor 111,111,111,255

#define RockDrawColor 62, 62, 62, 255
#define OrganicWateDrawColor 200, 200, 200, 255
#define AppleDrawColor 0, 100, 0, 255

#define DrawOcean
#define OceanColor 150,150,255,255		//Blue water
//#define OceanColor 100,255,100,255	//Green water

#define DrawMudLayer
#define MudColor 140,80,62,255		

#define RenderTypeAtStart predators
//-----------------------------------------------------------------


//-----------------------------------------------------------------
//Simulation
#define SimTickIntervalAtStart 0
#define SkipFramesAtStart 10

#define StartOnPause

//#define SpawnOneAtStart
//#define SpawnControlGroupAtStart
#define ControlGroupSize 6000
//-----------------------------------------------------------------


//-----------------------------------------------------------------
//World rules
#define FoodbaseInitial 4
#define PhotosynthesisReward_Summer 4
#define PhotosynthesisReward_Autumn 2
#define PhotosynthesisReward_Winter 1

#define MaxPossibleEnergyForABot 500
#define MaxBotLifetime 1000
#define EnergyPassedToAChild 50

#define EveryTickEnergyPenalty 0
#define AttackCost 4
#define MoveCost 1
#define RotateCost 0
#define GiveBirthCost 10

#define StunAfterBirth 1	//How many turns creature cannot act after his birth
#define FertilityDelay 0	//Delay before next birth

//#define RockCanBeEaten

//#define SpawnOrganicWasteWhenBotDies
#define OrganicWasteAlwaysFalls

//#define UseApples
#define DefaultAppleEnergy 125
#define AppleSpawnInterval 1
#define SpawnAppleInCellChance 4

//#define NoPhotosynthesis
//#define ForbidMutations
#define MutationChancePercent 20
//#define UseTotalMutation
#define TotalMutationChancePercentX10 0
//#define RandomColorChancePercentX10 4
#define ChangeColorSlightly

#define PresetRandomColors
//#define RandomColorEveryNewMarkersSet

#define MutateNeuronsMaximum 6
#define MutateNeuronsSlightlyMaximum 2

//#define UseSeasons
#define ChangeSeasonInterval 4000

#define NumberOfMutationMarkers 3
#define HowMuchDifferenseCantBeTold 1

//#define BotDiesIfEnergyOverflow

#define OceanHeight 50
//#define NoPhotosyntesisInOcean

#define MudLayerHeight 20
//-----------------------------------------------------------------



//-----------------------------------------------------------------
//Save and load
#define DirectoryName "SavedObjects/"

#define FilenameMaxLen 50
//-----------------------------------------------------------------




//-----------------------------------------------------------------
//Neural nets
#define NeuronsInLayer 5
#define NumNeuronLayers 6
#define NeuronOutputLayerIndex (NumNeuronLayers-1)
#define NeuronInputLayerIndex 0
#define MaxConnectionsPerNeuron 3

#define UseMemoryNeuron
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

#define Render_LineThickness 10

#define Render_Font "../ImGUI/misc/fonts/ProggyClean.tff"
//-----------------------------------------------------------------




//-----------------------------------------------------------------
// Population chart window
#define ChartNumValues 250
#define AddToChartEvery 500
//-----------------------------------------------------------------