
//-----------------------------------------------------------------
// 
// 
// Примерно как устроено:
// -Входная точка в файле Main.cpp
// -Все настраиваемые параметры в файле Settings.h в виде дефайнов
// -Все объекты на поле наследуется от Object
// -Neuron.h это нейрон, BotNeuralNet.h - вся нейросеть
// -Field, это класс игрового поля
// 
// Также:
// -NeuralNetRenderer это класс, который рисует мозг бота в отдельном окошке
// -ObjectSaver - сохраняет объекты и мир в файл
// -MyTypes - определение некоторых дополнительных типов данных для удобства
// 
//
//-----------------------------------------------------------------



#include "Main.h"


Main simulation;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	
	InitSDL();

	if (!CreateWindowSDL())
		return -2;

	CreateRenderer();

	//Setup Dear ImGui context
	InitImGUI();

	//Main loop	
	SDL_Event e;

	for (;;)
	{
		//Events
		mouseState.wheel = 0;

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				goto exitfor;
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				mouseState.wheel = e.wheel.y;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				simulation.CatchKeypress(SDL_GetKeyboardState(NULL));
			}
			else if (e.type == SDL_TEXTINPUT)
			{
				io->AddInputCharacter(*e.text.text);
			}

		}

		//Mouse down event
		if (ReadMouseState())
		{
			simulation.MouseClick();
		}

		//Simulation
		if(simulation.simulate)
			simulation.MakeStep();

		simulation.Render();

	}
exitfor:

	//Clear memory
	DeInitImGUI();
	DeInitSDL();

	return 0;
}


void Main::ChangeSeason()
{
	season = (Season)((int)season + 1);

	if (season > spring)
	{
		season = summer;
	}
}

//Pause/unpause
void Main::Pause()
{
	simulate = !simulate;
}

void Main::MakeStep()
{
	//Simulation step
	currentTick = GetTickCount64();

	if (((currentTick - prevTick) >= simulation.interval) || (simulation.interval == 0) || (simulation.renderType == noRender))
	{
	MakeStep:
		prevTick = currentTick;

		simulation.field->tick(simulation.ticknum);

		++simulation.ticknum;
		++secondTickCount;

	#ifdef UseSeasons	
		if (++changeSeasonCounter >= ChangeSeasonAfter)
		{
			ChangeSeason();

			changeSeasonCounter = 0;
		}
	#endif

		//Add data to chart
		if (--simulation.timeBeforeNextDataToChart == 0)
		{
			simulation.AddToChart(simulation.field->GetNumBots() * 1.0f,
				simulation.field->GetNumApples() * 1.0f, simulation.field->GetNumOrganics() * 1.0f);

			simulation.timeBeforeNextDataToChart = AddToChartEvery;
		}
	}

	//Calculate simulation speed
	if ((currentTick - lastSecond) >= 1000)
	{
		lastSecond = currentTick;

		simulation.simulationSpeed = secondTickCount;
		secondTickCount = 0;
	}
}

void Main::HighlightSelection()
{
	if (selectionShadowScreen > 0)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, selectionShadowScreen);
		SDL_RenderFillRect(renderer, &screenRect);
	}

	if (selection)
	{
		if (cursorShow)
			selectedObject->Object::draw();
	}
}

void Main::SelectionShadowScreen()
{
	if (selection)
	{
		if (selectionShadowScreen < 200)
			selectionShadowScreen += 5;
	}
	else
	{
		if (selectionShadowScreen > 0)
			selectionShadowScreen -= 5;
	}

	//Cursor blinking
	if (blink-- == 0)
	{
		blink = CursorBlinkRate;
		cursorShow = !cursorShow;
	}
}

void Main::ClearChart()
{
	memset(chartData_bots, 0, sizeof(chartData_bots));
	memset(chartData_organics, 0, sizeof(chartData_organics));
	memset(chartData_apples, 0, sizeof(chartData_apples));

	chart_numValues = 0;
	chart_currentPosition = 0;
}

void Main::AddToChart(float newVal_bots, float newVal_apples, float newVal_organics)
{
	chartData_bots[chart_currentPosition] = newVal_bots;
	chartData_apples[chart_currentPosition] = newVal_apples;
	chartData_organics[chart_currentPosition] = newVal_organics;

	if (chart_numValues < ChartNumValues)
	{
		++chart_numValues;
		++chart_currentPosition;
	}
	else
	{
		if (chart_currentPosition == ChartNumValues)
			ClearChart();
		else
			++chart_currentPosition;
	}
}

void Main::Deselect()
{
	selection = false;
	selectedObject = NULL;
	showBrain = false;
	nn_renderer.selectedNeuron = NULL;
}


void Main::ClearConsole()
{
	consoleText[0] = '\0';
}

//This is temporary
void Main::ConsoleInput(const char* str, bool newLine)
{
	if (strlen(consoleText) > (ConsoleCharLength/2 - 50))
		ClearConsole();

	strcpy_s(consoleText + strlen(consoleText), ConsoleCharLength / 2, str);

	if (newLine)
		ConsoleInput("\r\n");
}

void Main::ConsoleInput(int num, bool newLine)
{
	if (strlen(consoleText) > (ConsoleCharLength / 2 - 50))
		ClearConsole();

	sprintf_s(consoleText + strlen(consoleText), ConsoleCharLength / 2, "%i", num);

	if(newLine)
		ConsoleInput("\r\n");
}

void Main::LoadFilenames()
{
	//Check if folder exists
	if (!std::filesystem::exists(DirectoryName))
	{
		//If not create a folder
		std::filesystem::create_directory(DirectoryName);
	}

	//Load list of filenames
	numFilenames = 0;

	for (const auto& entry : std::filesystem::directory_iterator(DirectoryName))
	{
		#ifdef DoNotShowFolders
		if (entry.is_directory())
			continue;
		#endif

		//Full paths to files
		filenamesFull[numFilenames] = entry.path().string();

		//Only file names and sizes
		filenamesPartial[numFilenames] = entry.path().filename().string();

		filenamesPartial[numFilenames].resize(30, ' ');

		uint filesize = entry.file_size();
		std::string units;

		//bytes, kb or mb
		if (filesize > 1000000)
		{
			filesize /= 1000000;
			units = "MB";
		}
		else if (filesize > 1000)
		{
			filesize /= 1000;
			units = "KB";
		}
		else
		{
			units = "b";
		}

		filenamesPartial[numFilenames] += std::to_string(filesize);
		filenamesPartial[numFilenames] += units;

		++numFilenames;
	}
}




void Main::CreateNewFile()
{
	std::string fileName = "New1";
	int fileCounter = 1;

	for (;;)
	{
		if (std::filesystem::exists(DirectoryName + fileName))
		{
			fileName = "New" + std::to_string(++fileCounter);
		}
		else
		{
			break;
		}
	}

	std::ofstream file(DirectoryName + fileName);
	file.close();

	LoadFilenames();
}


Main::Main()
{
	ConsoleInput((char*)"Started. Seed:\r\n");

	//Set seed
	#ifdef RandomSeed		
		seed = GetTickCount();
	#else
		seed = Seed;
	#endif

	Field::seed = seed;
	srand(seed);

	id = rand();

	ConsoleInput(seed);

	field = new Field();

	#ifdef StartOnPause
		simulate = false;
	#endif

	LoadFilenames();
}

void Main::CatchKeypress(const Uint8* keyboard)
{
	if (keyboard[Keyboard_Pause])
	{
		Pause();
	}
	else if (keyboard[Keyboard_SpawnRandoms])
	{
		field->SpawnControlGroup();
	}
	else if (keyboard[Keyboard_NextFrame])
	{
		if (!simulate)
		{
			MakeStep();
		}
	}
	else if (keyboard[Keyboard_RenderNatural])
	{
		renderType = natural;
	}
	else if (keyboard[Keyboard_RenderPredators])
	{
		renderType = predators;
	}
	else if (keyboard[Keyboard_RenderEnergy])
	{
		renderType = energy;
	}
	else if (keyboard[Keyboard_NoRender])
	{
		renderType = noRender;
	}
	else if (keyboard[SDL_SCANCODE_RIGHT])
	{
		field->shiftRenderPoint((io->KeyShift)	? MoveCameraFastSpeed : MoveCameraSpeed);
	}
	else if (keyboard[SDL_SCANCODE_LEFT])
	{
		field->shiftRenderPoint((io->KeyShift) ? -MoveCameraFastSpeed : -MoveCameraSpeed);
	}
	else if (keyboard[Keyboard_Reset_RenderX])
	{
		field->renderX = 0;
	}
	else if (keyboard[Keyboard_Jump_Up_RenderX])
	{
		field->shiftRenderPoint(MoveCameraJump);
	}
	else if (keyboard[Keyboard_Jump_Down_RenderX])
	{
		field->shiftRenderPoint(-MoveCameraJump);
	}
	//Additional windows hotkeys
	else if (keyboard[Keyboard_ShowSaveLoad_Window])
	{
		showSaveLoad = !showSaveLoad;
	}
	else if (keyboard[Keyboard_ShowDangerous_Window])
	{
		showDangerous = !showDangerous;
	}
	else if (keyboard[Keyboard_ShowAdaptation_Window])
	{
		showAdaptation = !showAdaptation;
	}
	else if (keyboard[Keyboard_ShowChart_Window])
	{
		showChart = !showChart;
	}
}
