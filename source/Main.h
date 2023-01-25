#pragma once

#include "GUI.h"
#include "Field.h"
#include "NeuralNetRenderer.h"
#include "AutomaticAdaptation.h"



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

	//Automatic adaptation
	AutomaticAdaptation* auto_adapt;

	//Keyboard
	const Uint8* keyboard;

	//Simulation
	uint seed, id;
	uint realTPS = 0;
	int limit_interval = 0;
	int limit_ticks_per_second = TPSLimitAtStart;
	uint ticknum = 0;

	TimePoint prevTick;
	TimePoint lastSecondTick;
	uint tpsTickCounter = 0;
	TimePoint currentTick;

	Field* field;			

	//FPS
	int limitFPS = LimitFPSAtStart;
	int fpsInterval;
	uint realFPS = 0;
	uint fpsCounter = 0;
	TimePoint lastTickFps;
	TimePoint lastSecondTickFps;

	RenderTypes renderType = natural;
	MouseFunction mouseFunc = mouse_select;

	//Windows
	void DrawDemoWindow();
	void DrawMainWindow();
	void DrawSystemWindow();
	void DrawControlsWindow();
	void DrawSelectionWindow();
	void DrawDisplayWindow();
	void DrawLogWindow();
	void DrawMouseFunctionWindow();
	void DrawAdditionalsWindow();

	//Hidden windows
	void DrawSaveLoadWindow();
	void DrawDangerousWindow();
	void DrawSummaryWindow();
	void DrawAdaptationWindow();
	void DrawChartWindow();
	void DrawBotBrainWindow();
	void DrawAAWindow();

	//Show more windows
	bool showSaveLoad = false;
	bool showDangerous = false;
	bool showBrain = false;
	bool showAdaptation = false;
	bool showChart = false;	
	bool showAutomaticAdaptation = false;

	//Chart
	Chart chart;

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
	ObjectSaver saver;

	struct listed_file
	{
		string nameFull;
		string nameShort;
		string fileSize;
		string fullCaption;

		bool isSelected = false;
		bool isWorld = false;
	};

	vector<listed_file> allFilenames;
	listed_file* selectedFile = NULL;

	void LoadFilenames();
	void CreateNewFile();

	void DrawWindows();

	void HighlightSelection();
	void SelectionShadowScreen();

	//Set false to pause
	bool simulate = true;
	//Set to true to close the app
	bool terminate = false;

	void SwitchPause();		

	void MouseClick();
	void CatchKeyboard();
	

public:		

	void ClearWorld();

	void MakeStep();
	void Render();	

	void Start();
	void Pause();
	void RunWithNoRender();
	void RunWithMinimumRender();

	void Print(string s);

	Main();
	~Main();

	void MainLoop();
};


extern Main simulation;


