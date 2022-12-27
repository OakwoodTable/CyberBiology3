
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

	InitImGUI();

	Apple::CreateImage();
	Bot::CreateImage();
	Organics::CreateImage();

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
				simulation.CatchKeyboard();
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
		if (simulation.simulate)
		{
			simulation.MakeStep();
		}
		else
		{
			//Delay so it would not eat too many resourses while on pause
			SDL_Delay(1);
		}

		simulation.Render();

		if (simulation.terminate)
			goto exitfor;

	}
exitfor:

	//Clear memory
	Apple::DeleteImage();
	Bot::DeleteImage();
	Organics::DeleteImage();

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
	field->season=season;	
}


void Main::Pause()
{
	simulate = !simulate;

	if (simulate)
	{
		field->UnpauseThreads();
	}
	else
	{
		field->PauseThreads();
	}
}

void Main::MakeStep()
{
	//Simulation step
	currentTick = clock.now();

	if (limit_ticks_per_second > 0)
	{
		limit_interval = 1000 / limit_ticks_per_second;
	}
	else
	{
		limit_interval = 0;
	}

	if ((TimeMSBetween(currentTick, prevTick) >= limit_interval) || (limit_interval == 0) || (renderType == noRender))
	{
		prevTick = currentTick;

		field->tick(ticknum);

		++ticknum;
		++tpsTickCounter;

	#ifdef UseSeasons	
		if (++changeSeasonCounter >= ChangeSeasonInterval)
		{
			ChangeSeason();

			changeSeasonCounter = 0;
		}
	#endif

		//Add data to chart
		if (--timeBeforeNextDataToChart == 0)
		{
			AddToChart(field->GetNumBots() * 1.0f,
				field->GetNumApples() * 1.0f, field->GetNumOrganics() * 1.0f);

			timeBeforeNextDataToChart = AddToChartEvery;
		}
	}

	//Calculate simulation speed
	if (TimeMSBetween(currentTick, lastSecondTick) >= 1000)
	{
		lastSecondTick = currentTick;

		realTPS = tpsTickCounter;
		tpsTickCounter = 0;
	}
}

void Main::HighlightSelection()
{
	if (selectionShadowScreen > 0)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, selectionShadowScreen);
		SDL_RenderFillRect(renderer, &screenRect);
	}

	if (selectedObject)
	{
		if (cursorShow)
			selectedObject->Object::draw();
	}
}

void Main::SelectionShadowScreen()
{
	if (selectedObject)
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
	if (cursorBlink-- == 0)
	{
		cursorBlink = CursorBlinkRate;
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
	selectedObject = NULL;
	showBrain = false;
	nn_renderer.selectedNeuron = NULL;
}


void Main::ClearLog()
{
	logText.clear();
}

void Main::LogPrint(const char* str, bool newLine)
{
	logText.append(str);

	if (newLine)
		LogPrint("\r\n");
}

void Main::LogPrint(int num, bool newLine)
{
	logText.appendf("%i", num);
	
	if(newLine)
		LogPrint("\r\n");
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
	allFilenames.clear();

	for (const auto& entry : std::filesystem::directory_iterator(DirectoryName))
	{
		//Skip folders
		if (entry.is_directory())
			continue;

		listed_file f;

		//Full paths to files
		f.nameFull = entry.path().string();

		//Only file name
		f.nameShort = entry.path().filename().string();

		//File size
		uint size = entry.file_size();
		string unit;		

		//Units
		if (size > 1000000)
		{
			size /= 1000000;
			unit += "MB";
		}
		else if (size > 1000)
		{
			size /= 1000;
			unit += "KB";
		}
		else
		{
			unit += "b";
		}			

		f.fileSize += std::to_string(size);
		f.fileSize += unit;

		//Is world (open file briefly and look for file type)
		MyInputStream file((char*)f.nameFull.c_str(), std::ios::in | std::ios::binary | std::ios::beg);

		if (!file.is_open())
			continue;

		if (size > 0)
			f.isWorld = (file.ReadInt() == MagicNumber_WorldFile);
		else
			f.isWorld = false;

		file.close();		

		//Full file description
		f.fullCaption = f.nameShort;
		f.fullCaption.resize(25, ' ');
		f.fullCaption += f.fileSize;
		f.fullCaption.resize(40, ' ');
		f.fullCaption += (f.isWorld) ? ("[world]") : ("");

		allFilenames.push_back(f);
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
	LogPrint((char*)"Started. Seed:\r\n");

	//Set seed
	#ifdef RandomSeed		
		seed = GetTickCount();
	#else
		seed = Seed;
	#endif

	Field::seed = seed;
	srand(seed);

	id = rand();

	LogPrint(seed);

	field = new Field();

	#ifdef StartOnPause
		Pause();
	#endif

	LoadFilenames();

	keyboard = SDL_GetKeyboardState(NULL);
}

Main::~Main()
{
	delete field;
}

void Main::CatchKeyboard()
{
	if (keyboard[Keyboard_Pause] || keyboard[Keyboard_Pause2])
	{
		Pause();
	}
	else if (keyboard[Keyboard_SpawnRandoms])
	{
		field->SpawnControlGroup();
	}
	else if (keyboard[Keyboard_PlaceWall])
	{
		repeat(FieldCellsHeight)
			field->AddObject(new Rock(0, i));
	}
	else if (keyboard[Keyboard_DropOrganics])
	{
		for (int X = 0; X < FieldCellsWidth; ++X)
		{
			for (int Y = 0; Y < 25 + RandomVal(20); ++Y)
			{
				field->AddObject(new Organics(X, Y, MaxPossibleEnergyForABot/2));
			}
		}
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
	else if (keyboard[Keyboard_Jump_To_First_bot])
	{
		field->jumpToFirstBot();
	}
	//Additional windows hotkeys
	else if (keyboard[Keyboard_ShowSaveLoad_Window])
	{
		LoadFilenames();

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
	else if (keyboard[Keyboard_ShowBrain_Window])
	{
		showBrain = !showBrain;
	}	
}
