#pragma once



#ifndef UNIX
#include <windows.h>

//Libs
#pragma comment ( lib, "Winmm.Lib")
#pragma comment ( lib, "Version.lib")
#pragma comment ( lib, "Setupapi.lib")
#else
#include <cstdint>
#define _In_
#define _In_opt_
#define ULONGLONG uint64_t
#define strcpy_s(dest, n, src) strncpy(dest, src, n)
#define sprintf_s snprintf
#endif

#include "Field.h"

#include "NeuralNetRenderer.h"



//Seasons
Season season = summer;

void ChangeSeason()
{
	season = (Season)((int)season + 1);

	if (season > spring)
	{
		season = summer;
	}
}



//Window
SDL_Renderer* renderer;
SDL_Window* window = NULL;

bool simulate = true;	//Set false to pause
uint ticknum = 0;

void Pause();

//Show more windows
bool showSaveLoad = false;
bool showDangerous = false;
bool showSummary = false;
bool showBrain = false;
bool showSelectionInterface = false;
bool showChart = false;

//Population chart
float populationChartData[ChartNumValues];
int chart_numValues = 0;
int chart_shift = 0;
int chart_currentPosition = 0;

int timeBeforeNextDataToChart = AddToChartEvery;

void ClearChart();
void AddToChart(float newVal);


//Neural net renderer
NeuralNetRenderer nn_renderer;



//Selection
bool selection = false;
Object* selectedObject;

void Deselect();

int blink = 0;
bool cursorShow = true;
int brushSize = DefaultBrushRadius;

enum MouseFunction
{
	mouse_select,
	mouse_remove,
	mouse_place_rock,
	mouse_from_file,
	mouse_force_mutation
};




//Console
char consoleText[ConsoleCharLength] = "";

void ClearConsole();
void ConsoleInput(const char* str, bool newLine = false);
void ConsoleInput(int num, bool newLine=true);



//Save/load
int listCurrentItem = 0;

char filenamesFull[50][FilenameMaxLen];
char filenamesPartial[50][FilenameMaxLen];
bool selectedFilenames[FilenameMaxLen] = {};
int selectedFilename = -1;
int numFilenames = 0;

void LoadFilenames();
void CreateNewFile();


