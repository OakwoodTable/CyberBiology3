
//-----------------------------------------------------------------
// 
// 
// Примерно как устроено:
// - Входная точка в файле Main.cpp
// - Все настраиваемые параметры в файле Settings.h
// - Все объекты на поле наследуется от Object
// - Neuron.h это нейрон, BotNeuralNet.h - вся нейросеть
// - Field, это класс игрового поля
// 
// Также:
// - NeuralNetRenderer это класс, который рисует мозг бота в отдельном окошке
// - ObjectSaver - сохраняет объекты и мир в файл
// - ImageFactory - создает текстуры для объектов
// - AutomaticAdaptation - автоматизированный эксперимент адаптация
// 
//
//-----------------------------------------------------------------


#include "Main.h"


Main simulation;


inline void ValidateThreadsNumber()
{
	if ((FieldCellsWidth % (NumThreads * 2)) != 0)
	{
		//Ширина поля должна делиться на (число потоков х2) без остатка!
		ErrorMessage(L"FieldCellsWidth should be divisible by (NumThreads * 2) without remainder!", L"Wrong params!");
		exit(0);
	}

	if ((FieldCellsWidth / (NumThreads * 2)) < 4)
	{
		//Поле должно быть шире или потоков должно быть меньше
		ErrorMessage(L"Field chunk too small for selected number of threads!", L"Wrong params!");
		exit(0);
	}
	
	for (uint n : {1, 4, 8, 16, 24, 48})
	{
		if (n == NumThreads)
			return;	//All fine
	}

	ErrorMessage(L"Invalid number of threads!", L"Wrong params!");
	exit(0);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	//Init
	ValidateThreadsNumber();
	InitSDL();

	if (!CreateWindowSDL())
		return -2;

	CreateRenderer();
	InitImGUI();
	ImageFactory::CreateImages();

	//Main cycle
	simulation.MainLoop();

	//Clear memory
	ImageFactory::DeleteImages();
	DeInitImGUI();
	DeInitSDL();

	return 0;
}

void Main::SwitchPause()
{
	simulate = !simulate;
}

void Main::BrushIterate(Point p, void(*callback)(uint, uint, Field*))
{
	uint X = p.x, Y = p.y;

	for (int cx = -brushSize; cx < brushSize + 1; ++cx)
	{
		for (int cy = -brushSize; cy < brushSize + 1; ++cy)
		{
			if (field->IsInBounds(X + cx, Y + cy))
			{
				if ((brushSize * brushSize) > ((cx * cx) + (cy * cy)))
				{
					callback(X + cx, Y + cy, field);
				}
			}
		}
	}
}

void Main::Start()
{
	simulate = true;
}

void Main::Pause()
{
	simulate = false;
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

	if ((TimeMSBetween(currentTick, prevTick) >= limit_interval) or (limit_interval == 0) or (renderType == noRender))
	{
		prevTick = currentTick;

		field->tick(ticknum);

		//Manage auto adaptation
		auto_adapt->AdaptationStep(ticknum);

		++ticknum;
		++tpsTickCounter;

		//Add data to chart
		if (chart.Tick())
		{
			chart.AddToChart(field->GetNumBots() * 1.0f,
				field->GetNumApples() * 1.0f,
				field->GetNumOrganics() * 1.0f,
				field->GetNumPredators() * 1.0f,
				field->GetAverageLifetime() * 1.0f);
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

	if (cursorBlink-- == 0)
	{
		cursorBlink = CursorBlinkRate;
		cursorShow = !cursorShow;
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
		auto size = entry.file_size();
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

		f.fileSize += to_string(size);
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
	string fileName = "New1";
	int fileCounter = 1;

	for (;;)
	{
		if (std::filesystem::exists(DirectoryName + fileName))
		{
			fileName = "New" + to_string(++fileCounter);
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

	//Set seed and id
	#ifdef RandomSeed		
		seed = (uint)GetTickCount64();
	#else
		seed = Seed;
	#endif

	Field::seed = seed;
	srand(seed);

	id = rand();

	LogPrint(seed);

	field = new Field();
	auto_adapt = new AutomaticAdaptation(field, this);

	Pause();

	LoadFilenames();

	keyboard = SDL_GetKeyboardState(NULL);
}

Main::~Main()
{
	delete field;
	delete auto_adapt;
}

void Main::MainLoop()
{
	//Main loop	
	SDL_Event e;

	for (;!terminate;)
	{
		//Events
		mouseState.wheel = 0;

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				return;
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				mouseState.wheel = e.wheel.y;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				CatchKeyboard();
			}
			else if (e.type == SDL_TEXTINPUT)
			{
				io->AddInputCharacter(*e.text.text);
			}
			else if (e.type == SDL_WINDOWEVENT)
			{
				if(e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
				{
					windowIsVisible = false;
				}
				else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
				{
					windowIsVisible = true;
				}
			}

		}

		//Mouse down event
		if (ReadMouseState())
		{
			MouseClick();
		}

		//Simulation
		if (simulate)
		{
			MakeStep();
		}
		else
		{
			//Delay so it would not eat too many resourses while on pause
			SDL_Delay(5);
		}

		if(windowIsVisible)
			Render();
	}
}

void Main::CatchKeyboard()
{
	if (keyboard[Keyboard_Pause] || keyboard[Keyboard_Pause2])
	{
		SwitchPause();
	}
	else if (keyboard[Keyboard_SpawnRandoms])
	{
		field->SpawnControlGroup();
	}
	else if (keyboard[Keyboard_PlaceWall])
	{
		field->placeWall();
	}
	else if (keyboard[Keyboard_DropOrganics])
	{
		for (int X = 0; X < FieldCellsWidth; ++X)
		{
			for (int Y = 0; Y < 25 + RandomVal(20); ++Y)
			{
				field->AddObject(new Organics(X, Y, BotMaxEnergyInitial/2));
			}
		}
	}
	else if (keyboard[Keyboard_SpawnRocks])
	{
		for (int i = 0; i < SpawnRocksSize; ++i)
		{
			Rock* tmp = new Rock(RandomVal(FieldCellsWidth), RandomVal(FieldCellsHeight));

			if (!field->AddObject(tmp))
				delete tmp;
		}
	}
	else if (keyboard[Keyboard_MutateScreen])
	{
		field->mutateWorld();

		LogPrint("[Solar flare!]");
	}
	else if (keyboard[Keyboard_Quicksave])
	{
		if (saver.SaveWorld(field, (char*)OuicksaveFilename, id, ticknum))
		{
			LogPrint("World saved\r\n");

			LoadFilenames();
		}
		else
		{
			LogPrint("Error while saving world\r\n");
		}
	}
	else if (keyboard[Keyboard_Quickload])
	{
		ObjectSaver::WorldParams ret = saver.LoadWorld(field, (char*)OuicksaveFilename);

		if (ret.id != -1)
		{
			if (ret.width != FieldCellsWidth)
				LogPrint("World loaded(width mismatch)\r\n");
			else
				LogPrint("World loaded\r\n");

			seed = ret.seed;
			ticknum = ret.tick;
			id = ret.id;

			field->seed = seed;
		}
		else
			LogPrint("Error while loading world\r\n");
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
	else if (keyboard[Keyboard_ShowAutomaticAdaptation_Window])
	{
		showAutomaticAdaptation = !showAutomaticAdaptation;
	}
}
