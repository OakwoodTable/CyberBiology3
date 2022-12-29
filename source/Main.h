#pragma once

#include "GUI.h"
#include "Field.h"
#include "NeuralNetRenderer.h"


enum MouseFunction
{
	mouse_select,
	mouse_remove,
	mouse_place_rock,
	mouse_from_file,
	mouse_force_mutation
};


class Main final
{

private:

	Clock clock;

	const SDL_Rect screenRect = { 0, 0, WindowWidth, WindowHeight };

	//Save/load object interface
	ObjectSaver saver;

	//Keyboard
	const Uint8* keyboard;

	//Simulation
	int seed, id;
	uint realTPS = 0;
	int limit_interval = 0;
	int limit_ticks_per_second = FPSLimitAtStart;

	Field* field;	

	uint ticknum = 0;

	TimePoint prevTick;
	TimePoint lastSecondTick;
	uint tpsTickCounter = 0;
	TimePoint currentTick;

	//FPS
	int limitFPS = LimitFPSAtStart;
	int fpsInterval;
	uint realFPS = 0;
	uint fpsCounter = 0;
	TimePoint lastTickFps;
	TimePoint lastSecondTickFps;

	RenderTypes renderType = RenderTypeAtStart;
	MouseFunction mouseFunc = mouse_select;

	//Seasons
	Season season = summer;
	uint changeSeasonCounter = 0;

	//Windows
	void DrawDemoWindow();
	void DrawMainWindow();
	void DrawSystemWindow();
	void DrawControlsWindow();
	void DrawSelectionWindow();
	void DrawRenderWindow();
	void DrawConsoleWindow();
	void DrawMouseFunctionWindow();
	void DrawAdditionalsWindow();

	//Hidden windows
	void DrawSaveLoadWindow();
	void DrawDangerousWindow();
	void DrawSummaryWindow();
	void DrawAdaptationWindow();
	void DrawChartWindow();
	void DrawBotBrainWindow();

	//Show more windows
	bool showSaveLoad = false;
	bool showDangerous = false;
	bool showBrain = false;
	bool showAdaptation = false;
	bool showChart = false;	

	//Chart (TODO)
	float chartData_bots[ChartNumValues];
	float chartData_apples[ChartNumValues];
	float chartData_organics[ChartNumValues];
	int chart_numValues = 0;
	int chart_currentPosition = 0;

	int timeBeforeNextDataToChart = AddToChartEvery;

	bool chartShow_apples = false;
	bool chartShow_organics = false;

	void ClearChart();
	void AddToChart(float, float, float);

	//Neural net renderer
	NeuralNetRenderer nn_renderer;
	//Show initial brain or active brain
	int brainToShow = 0;

	//Bot selection
	Object* selectedObject;

	int cursorBlink = 0;
	bool cursorShow = true;	
	int selectionShadowScreen = 0;

	void Deselect();

	int brushSize = DefaultBrushRadius;

	//Log
	ImGuiTextBuffer logText;

	void ClearLog();
	void LogPrint(const char* str, bool newLine = false);
	void LogPrint(int num, bool newLine = true);

	//Save/load

	struct listed_file
	{
		string nameFull;
		string nameShort;
		string fileSize;
		string fullCaption;

		bool isSelected = false;

		bool isWorld;
	};

	std::vector<listed_file> allFilenames;
	listed_file* selectedFile = NULL;

	void LoadFilenames();
	void CreateNewFile();

	void DrawWindows();

	void HighlightSelection();
	void SelectionShadowScreen();

	void ChangeSeason();

	void Pause();

	void ClearWorld();
	

public:		

	//Set false to pause
	bool simulate = true;

	//Set to true to close the app
	bool terminate = false;


	void MakeStep();
	
	void MouseClick();

	void Render();
	

	Main();
	~Main();

	void CatchKeyboard();

};


extern Main simulation;

