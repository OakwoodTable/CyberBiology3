#pragma once


//System headers
#include <windows.h>

#pragma comment ( lib, "Winmm.Lib")
#pragma comment ( lib, "Version.lib")
#pragma comment ( lib, "Setupapi.lib")

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>

//ImGUI
#include "imgui.h"
#include "imgui_sdl.h"

//SDL + OpenGL
#include <glad/glad.h>
#include <SDL.h>


//My headers
#include "MyTypes.h"
#include "Settings.h"



//Seasons
enum Season
{
	summer,
	autumn,
	winter,
	spring
};

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

bool showSaveLoad = false;
bool showDangerous = false;

#include "Field/Field.h"



//Selection
bool selection = false;
Object* selectedObject;
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



