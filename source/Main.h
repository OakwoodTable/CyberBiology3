#pragma once

#include "SDL.h"
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

public:
	
	//Simulation
	int seed, id;
	uint simulationSpeed = 0;
	int interval = SimTickIntervalAtStart;

	Field* field;	


	//Seasons
	Season season = summer;

	void ChangeSeason();


	bool simulate = true;	//Set false to pause

	ULONGLONG prevTick = GetTickCount64();
	ULONGLONG lastSecond = GetTickCount64();
	ULONGLONG currentTick;

	uint secondTickCount = 0;
	uint changeSeasonCounter = 0;
	int skipping = 0;
	uint ticknum = 0;
	int skipFrames = SkipFramesAtStart;
	int skippingRender = 0;

	RenderTypes renderType = RenderTypeAtStart;
	MouseFunction mouseFunc = mouse_select;

	//Set to true to close the app
	bool terminate = false; 

	void Pause();

	void MakeStep();

	void HighlightSelection();
	void SelectionShadowScreen();
	
	void DrawWindows();

	void MouseClick();

	void Render();

	void ClearWorld();


	//Show more windows
	bool showSaveLoad = false;
	bool showDangerous = false;
	bool showSummary = false;
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

	void ClearChart();
	void AddToChart(float, float, float);


	//Neural net renderer
	NeuralNetRenderer nn_renderer;
	//Show initial brain or active brain
	int brainToShow = 0;


	//Bot selection
	bool selection = false;
	Object* selectedObject;

	void Deselect();

	int blink = 0;
	bool cursorShow = true;
	int brushSize = DefaultBrushRadius;
	int selectionShadowScreen = 0;

	SDL_Rect screenRect = { 0, 0, WindowWidth, WindowHeight };
		

	//Console (TODO)
	char consoleText[ConsoleCharLength] = "";

	void ClearConsole();
	void ConsoleInput(const char* str, bool newLine = false);
	void ConsoleInput(int num, bool newLine = true);


	//Save/load
	int listCurrentItem = 0;

	std::string filenamesFull[MaxFilenames];
	std::string filenamesPartial[MaxFilenames];
	bool selectedFilenames[MaxFilenames] = {};
	int selectedFilename = -1;
	int numFilenames = 0;

	void LoadFilenames();
	void CreateNewFile();


	Main();

	void CatchKeypress(const Uint8*);

};


extern Main simulation;

