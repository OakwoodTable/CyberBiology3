
//-----------------------------------------------------------------
// 
//Качество кода, мягко говоря, хромает, но это хобби-проект, изначально публиковать не планировал.
//Программа выросла из одного файла, строго не судите.
// 
// 
// Примерно как устроено:
// -Входная точка в файле Main.cpp
// -Все настраиваемые параметры в файле Settings.h в виде дефайнов
// -Класс бот это бот (невероятно!), наследуется от Object
// -Neuron.h это нейрон, Neurons.h - вся нейросеть
// -Вся логика в классе Field, это класс игрового поля
// -NeuralNetRenderer это класс, который рисует мозг бота в отдельном окошке
// -MyTypes определение некоторых дополнительных типов данных для удобства
// 
//
//-----------------------------------------------------------------



#include "Main.h"

#if !defined(_WIN32)

uint32_t GetTickCount() {
	struct timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		exit(-1);
	}
	uint32_t theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
}

// https://fire-monkey.ru/topic/6571-tthreadgettickcount64-%D1%85%D0%B5%D0%BB%D0%BF%D0%B5%D1%80-%D0%B4%D0%BB%D1%8F-tthread/
uint32_t GetTickCount64() {
	struct timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		exit(-1);
	}
    return (1000000000 * ts.tv_sec + ts.tv_nsec) / 1000000;
}

#endif

void InitSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		exit(-1);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// GL 3.0 + GLSL 130
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, SDL_glCont_Major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, SDL_glCont_Minor);
}


#if defined(_WIN32)
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,	_In_opt_ HINSTANCE hPrevInstance,	_In_ LPWSTR    lpCmdLine,	_In_ int       nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{

	ConsoleInput((char*)"Started. Seed:\r\n");

	int seed, id;

	//Set seed
	#ifdef RandomSeed		
		seed = GetTickCount();
    #else
		seed = Seed;
	#endif

	Field::seed = seed;
	srand(seed);

	//Generate id
	id = rand();

	ConsoleInput(seed);

	// initialize SDL
	InitSDL();	

	//Create window
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI	);

	window = SDL_CreateWindow(
		WindowCaption,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WindowWidth,
		WindowHeight,
		window_flags
	);

	if (window == NULL) 
	{
		return -2;
	}

	// limit to which minimum size user can resize the window
	SDL_SetWindowMinimumSize(window, WindowWidth, WindowHeight);

	//Create renderer
	//renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	SDL_GL_SetSwapInterval(0);

	//Setup GL
	glViewport(0, 0, WindowWidth, WindowHeight);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//Setup Dear ImGui context
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	//Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
	
	//Main simulation field
	Field* field = new Field();

	#ifdef StartOnPause
		simulate = false;
	#endif

	//Load filenames for save/load window
	LoadFilenames();


	//Main loop
	
	ULONGLONG prevTick = GetTickCount64();
	ULONGLONG lastSecond = GetTickCount64();
	ULONGLONG currentTick;
	
	uint secondTickCount = 0;
	uint simulationSpeed = 0;
	uint changeSeasonCounter = 0;

	SDL_Event e;
	int interval = SimTickIntervalAtStart;
	int skipFrames = SkipFramesAtStart;
	int skipping = 0;
	int skippingRender = 0;
	RenderTypes renderType = RenderTypeAtStart;
	MouseFunction mouseFunc = mouse_select;

	int mouseX, mouseY;
	int buttons;



	for (;;)
	{
		//Events
		int wheel = 0;

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				goto exitfor;
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				wheel = e.wheel.y;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				const Uint8* key = SDL_GetKeyboardState(NULL);

				if (key[Keyboard_Pause])
				{
					Pause();
				}
				else if (key[Keyboard_SpawnRandoms])
				{
					field->SpawnControlGroup();
				}
				else if (key[Keyboard_NextFrame])
				{
					if (!simulate)
					{
						goto MakeStep;
					}
				}
				else if (key[Keyboard_RenderNatural])
				{
					renderType = natural;
				}
				else if (key[Keyboard_RenderPredators])
				{
					renderType = predators;
				}
				else if (key[Keyboard_RenderEnergy])
				{
					renderType = energy;
				}
				else if (key[Keyboard_NoRender])
				{
					renderType = noRender;
				}
			}
			else if (e.type == SDL_TEXTINPUT)
			{
				io.AddInputCharacter(*e.text.text);
			}

		}

		// Setup low-level inputs for imGUI
		buttons = SDL_GetMouseState(&mouseX, &mouseY);

		io.DeltaTime = 1.0f / 60.0f;
		io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		io.MouseWheel = static_cast<float>(wheel);

		/*
		io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
		io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
		io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
		io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
		*/

		//Mouse down event
		if ((io.MouseDown[0]) && (!io.WantCaptureMouse))
		{
			if ((nn_renderer.MouseClick({ mouseX, mouseY })) && showBrain)
			{
				//TODO
			}
			else if (field->IsInBoundsScreenCoords(mouseX, mouseY))
			{
				Point fieldCoords = field->ScreenCoordsToLocal(mouseX, mouseY);
				Object* obj = field->GetObjectLocalCoords(fieldCoords.x, fieldCoords.y);

				if (mouseFunc == mouse_select)
				{
					if (obj)
					{
						if (obj->type == bot)
						{
							selection = true;
							selectedObject = obj;
						}
					}
					else
					{
						Deselect();
					}
				}
				else if (mouseFunc == mouse_remove)
				{
					Deselect();

					for (int cx = -brushSize; cx < brushSize + 1; ++cx)
					{
						for (int cy = -brushSize; cy < brushSize + 1; ++cy)
						{
							if (field->IsInBounds(fieldCoords.x + cx, fieldCoords.y + cy))
								field->RemoveObject(fieldCoords.x + cx, fieldCoords.y + cy);
						}
					}
				}
				else if (mouseFunc == mouse_place_rock)
				{
					for (int cx = -brushSize; cx < brushSize + 1; ++cx)
					{
						for (int cy = -brushSize; cy < brushSize + 1; ++cy)
						{
							if (field->IsInBounds(fieldCoords.x + cx, fieldCoords.y + cy))
							{
								obj = field->GetObjectLocalCoords(fieldCoords.x + cx, fieldCoords.y + cy);

								if (!obj)
								{
									field->AddObject(new Rock(fieldCoords.x + cx, fieldCoords.y + cy));
								}
							}
						}
					}
				}
				else if (mouseFunc == mouse_from_file)
				{
					//Cell empty
					if (!obj)
					{
						if (selectedFilename != -1)
						{
							Bot* obj = new Bot(fieldCoords.x, fieldCoords.y);

							field->AddObject(obj);

							selection = true;
							selectedObject = obj;

							if (field->LoadObject(obj, filenamesFull[selectedFilename]))
								ConsoleInput("Object loaded\r\n");
							else
								ConsoleInput("Error while loading object\r\n");
						}
					}
				}
				else if (mouseFunc == mouse_force_mutation)
				{
					for (int cx = -brushSize; cx < brushSize + 1; ++cx)
					{
						for (int cy = -brushSize; cy < brushSize + 1; ++cy)
						{
							if (field->IsInBounds(fieldCoords.x + cx, fieldCoords.y + cy))
							{
								Bot* obj = (Bot*)field->GetObjectLocalCoords(fieldCoords.x + cx, fieldCoords.y + cy);

								if (obj)
								{
									if (obj->type == bot)
									{
										obj->Mutagen();
									}
								}
							}
						}
					}
				}
			}
		}

		//Simulation
		if (simulate)
		{
			//Simulation step
			currentTick = GetTickCount64();

			if (((currentTick - prevTick) >= interval) || (interval == 0))
			{
			MakeStep:
				prevTick = currentTick;

				field->tick(ticknum);

				++ticknum;
				++secondTickCount;

				#ifdef UseSeasons	
				if (++changeSeasonCounter >= ChangeSeasonAfter)
				{
					ChangeSeason();

					changeSeasonCounter = 0;
				}
				#endif

				//Add data to chart
				if (--timeBeforeNextDataToChart == 0)
				{
					AddToChart(field->GetNumObjects() * 1.0f);

					timeBeforeNextDataToChart = AddToChartEvery;
				}
			}

			//Calculate simulation speed
			if ((currentTick - lastSecond) >= 1000)
			{
				lastSecond = currentTick;

				simulationSpeed = secondTickCount;
				secondTickCount = 0;
			}

			//Skip drawing
			if (renderType != noRender)
			{
				if (skipFrames)
				{
					if (--skipping > 0)
					{
						continue;
					}
					else
					{
						skipping = skipFrames;
					}
				}
			}
			else
			{
				if (--skipping > 0)
				{
					continue;
				}
				else
				{
					skipping = SkipGUIFramesWhenNoRender;
				}
			}
		}

		//Begin frame

		//Clear background
		if (skippingRender == 0)
		{
			glClearColor(BackgroundColorFloat);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		//Render
		if (renderType != noRender)
		{
			if (simulate)
				field->draw(renderType);
			else
			{
				if (skippingRender-- == 0)
				{
					skippingRender = SkipRenderingFramesWhileOnPause;
					field->draw(renderType);
				}
			}
		}

		//Highlight selected object
		if (selection)
		{
			if (cursorShow)
				selectedObject->Object::draw();

			if (blink-- == 0)
			{
				blink = CursorBlinkRate;
				cursorShow = !cursorShow;
			}
		}

		ImGui_ImplSDLRenderer_NewFrame();		
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//Draw GUI
		//ImGUI demo window
		#ifdef ShowGUIDemoWindow
		{
			ImGui::SetNextWindowPos({ 20.0f,20.0f });
			ImGui::ShowDemoWindow();
		}
		#endif

		//Main window 
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 135.0f });
		ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f,InterfaceBorder * 1.0f });

		ImGui::Begin("Main", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		{
			//FPS text 
			ImGui::Text("steps: %i", ticknum);
			ImGui::Text("(interval %i, speed: %i ticks/sec)", interval, simulationSpeed);
			ImGui::Text("Objects total: %i", field->GetNumObjects());
			ImGui::Text("Bots total: %i", field->GetNumBots());

			//Show season name
			/*
			switch (season)
			{
			case summer:
				ImGui::Text("Season: Summer");
				break;
			case autumn:
				ImGui::Text("Season: Autumn");
				break;
			case winter:
				ImGui::Text("Season: Winter");
				break;
			case spring:
				ImGui::Text("Season: Spring");
				break;
			}

			ImGui::SameLine();
			ImGui::Text(" ( %i )", ChangeSeasonInterval - changeSeasonCounter);*/

			//Neural net params
			ImGui::Text("Layers: %i, Neurons in layer: %i", NumNeuronLayers, NeuronsInLayer);

			//Simulation seed and unique id
			ImGui::Text("Seed: %i, simulation id: %i", seed, id);

		}
		ImGui::End();


		//System window
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 70.0f });
		ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 140.0f });

		ImGui::Begin("System", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
			ImGui::SameLine();
			ImGui::Text(" %s", SDL_GetPlatform());

			ImGui::SameLine();

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "CPU cores: %d", SDL_GetCPUCount());

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "RAM: %.2f GB", SDL_GetSystemRAM() / 1024.0f);

			ImGui::SameLine();

			//Show number of threads
			#ifdef UseOneThread
			ImGui::Text(", 1 thread used");
			#endif

			#ifdef UseFourThreads
			ImGui::Text(", 4 threads used");
			#endif

			#ifdef UseEightThreads
			ImGui::Text(", 8 threads used");
			#endif
		}
		ImGui::End();


		//Controls window
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 160.0f });
		ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 220.0f });

		ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		{
			if (ImGui::Button((simulate) ? "stop" : "start", { 200, 25 }))
			{
				Pause();
			}

			//Sliders
			ImGui::PushItemWidth(200);
			ImGui::SliderInt("interval", &interval, GUI_Max_interval, 0, "%d", ImGuiSliderFlags_Logarithmic);
			ImGui::SliderInt("food", &(field->foodBase), GUI_Max_food, 0, "%d", ImGuiSliderFlags_None);
			ImGui::SliderInt("skip", &skipFrames, GUI_Max_skip, 0, "%d", ImGuiSliderFlags_None);
			ImGui::SliderInt("brush", &brushSize, GUI_Max_brush, 0, "%d", ImGuiSliderFlags_None);
		}
		ImGui::End();


		//Selection window
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 150.0f });
		ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 390.0f });

		ImGui::Begin("Selection", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		{
			if (selection)
			{				
				if (field->ValidateObjectExistance(selectedObject))
				{
					//Object info
					ImGui::Text("type: Bot	X: %i, Y: %i", selectedObject->x, selectedObject->y);
					ImGui::Text("lifetime: %i / %i", selectedObject->GetLifetime(), MaxBotLifetime);
					ImGui::Text("energy: %i (PS: %i, kills: %i)", ((Bot*)selectedObject)->GetEnergy(), ((Bot*)selectedObject)->GetEnergyFromPS(), ((Bot*)selectedObject)->GetEnergyFromKills());

					//Mutation markers
					int m[NumberOfMutationMarkers];
					memcpy(m, ((Bot*)selectedObject)->GetMarkers(), sizeof(m));
					ImGui::Text("markers: {");

					repeat(NumberOfMutationMarkers)
					{
						ImGui::SameLine();
						ImGui::Text("%i", m[i]);
					}

					ImGui::SameLine();
					ImGui::Text("}");

					//Color
					Uint8 c[3];
					memcpy(c, ((Bot*)selectedObject)->GetColor(), sizeof(c));
					ImGui::Text("color: {%i, %i, %i}", c[0], c[1], c[2]);

					ImGui::SameLine();
					ImGui::TextColored(ImVec4(((c[0] * 1.0f) / 255.0f), ((c[1] * 1.0f) / 255.0f), ((c[2] * 1.0f) / 255.0f), 1.0f), "*****");

					ImGui::SameLine();
					if (ImGui::Button("Repaint", { 50, 20 }))
					{
						field->RepaintBot((Bot*)selectedObject, Bot::GetRandomColor().rgb, 1);
					}

					if (ImGui::Button("Show summary", { 100, 30 }))
					{
						showSummary = !showSummary;
					}

					ImGui::SameLine();

					if (ImGui::Button("Show brain", { 100, 30 }))
					{
						showBrain = !showBrain;
					}
				}
				else
				{
					Deselect();
				}
			}
		}
		ImGui::End();


		//Display window
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 140.0f });
		ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 550.0f });

		ImGui::Begin("Display", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		{
			ImGui::BeginGroup();
			ImGui::Text("Render:");
			ImGui::RadioButton("Natural colors", (int*)&renderType, 0);
			ImGui::RadioButton("Predators", (int*)&renderType, 1);
			ImGui::RadioButton("Energy", (int*)&renderType, 2);
			ImGui::RadioButton("No render", (int*)&renderType, 3);
			ImGui::EndGroup();
		}
		ImGui::End();


		//Console window
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 120.0f });
		ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 700.0f });

		ImGui::Begin("Console", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		{
			ImGui::InputTextMultiline("##", consoleText, ConsoleCharLength, ImVec2(240, 90), ImGuiInputTextFlags_EnterReturnsTrue);
		}
		ImGui::End();


		//Mouse function window
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 130.0f });
		ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 830.0f });

		ImGui::Begin("Mouse function", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		{
			ImGui::BeginGroup();
			ImGui::RadioButton("Select", (int*)&mouseFunc, 0);
			ImGui::RadioButton("Remove", (int*)&mouseFunc, 1);
			ImGui::RadioButton("Place rock", (int*)&mouseFunc, 2);
			ImGui::RadioButton("Place creature from file", (int*)&mouseFunc, 3);
			ImGui::RadioButton("Force mutate", (int*)&mouseFunc, 4);
			ImGui::EndGroup();
		}
		ImGui::End();


		//Additional windows window
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 110.0f });
		ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 970.0f });

		ImGui::Begin("Additional windows", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		{

			if (ImGui::Button("Save/load", { 130, 30 }))
			{
				LoadFilenames();

				showSaveLoad = !showSaveLoad;
			}
			ImGui::SameLine();

			if (ImGui::Button("Dangerous commands", { 130, 30 }))
			{
				showDangerous = !showDangerous;
			}

			if (ImGui::Button("Adaptation", { 130, 30 }))
			{
				showAdaptation = !showAdaptation;
			}
			ImGui::SameLine();

			if (ImGui::Button("Chart", { 130, 30 }))
			{
				showChart = !showChart;
			}
		}
		ImGui::End();

		//Below windows that are hidden at startup

		if (showSaveLoad)
		{
			//Save/load window
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGui::SetNextWindowSize({ 330.0f, 200.0f });
			ImGui::SetNextWindowPos({ 100 * 1.0f, 100.0f }, ImGuiCond_Once);
			
			ImGui::Begin("Save/load", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			{
				//List of files
				ImGui::Text("Select filename");

				ImGui::ListBoxHeader("##", ImVec2(290, 110));

				for (int i = 0; i < numFilenames; ++i)
				{
					if (ImGui::Selectable(filenamesPartial[i], &selectedFilenames[i]))
					{
						memset(selectedFilenames, 0, sizeof(selectedFilenames));
						selectedFilenames[i] = true;
						selectedFilename = i;
					}
				}
				ImGui::ListBoxFooter();

				//Buttons
				if (ImGui::Button("Save", { 70, 30 }))
				{
					if (selection)
					{
						if (selectedFilename != -1)
						{
							if (field->SaveObject(selectedObject, filenamesFull[selectedFilename]))
								ConsoleInput("Object saved\r\n");
							else
								ConsoleInput("Error while saving object\r\n");
						}
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Load", { 70, 30 }))
				{
					if (selection)
					{
						if (selectedFilename != -1)
						{
							if (field->LoadObject(selectedObject, filenamesFull[selectedFilename]))
								ConsoleInput("Object loaded\r\n");
							else
								ConsoleInput("Error while loading object\r\n");
						}
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("New", { 70, 30 }))
				{
					CreateNewFile();
				}
			}
			ImGui::End();
		}

		//Dangerous commands window
		if (showDangerous)
		{
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGui::SetNextWindowSize({ 330.0f, 140.0f });
			ImGui::SetNextWindowPos({ 100 * 1.0f, 300.0f }, ImGuiCond_Once);

			ImGui::Begin("Use with caution!", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			{

				if (ImGui::Button("Wipe world", { 130, 30 }))
				{
					Deselect();

					for (int cx = 0; cx < FieldCellsWidth; ++cx)
					{
						for (int cy = 0; cy < FieldCellsHeight; ++cy)
						{
							field->RemoveObject(cx, cy);
						}
					}

					ticknum = 0;
				}

				ImGui::SameLine();

				if (ImGui::Button("Kill bots", { 130, 30 }))
				{
					Deselect();

					for (int cx = 0; cx < FieldCellsWidth; ++cx)
					{
						for (int cy = 0; cy < FieldCellsHeight; ++cy)
						{
							Object* o = field->GetObjectLocalCoords(cx, cy);

							if (o == NULL)
								continue;

							if(o->type == bot)
								field->RemoveObject(cx, cy);
						}
					}
				}

				if (ImGui::Button("CloseApp", { 130, 30 }))
				{
					goto exitfor;
				}

				ImGui::SameLine();

				if (ImGui::Button("Zero time", { 130, 30 }))
				{
					ticknum = 0;
				}
			}
			ImGui::End();
		}

		//Summary window
		if (showSummary)
		{
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGui::SetNextWindowSize({ 330.0f, 180.0f });
			ImGui::SetNextWindowPos({ 100 * 1.0f, 450.0f }, ImGuiCond_Once);

			ImGui::Begin("Bot summary", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			{
				if (selection)
				{
					if (field->ValidateObjectExistance(selectedObject))
					{
						auto summ = ((Bot*)selectedObject)->GetNeuronSummary();

						ImGui::Text("Neurons:");
						ImGui::Text("Simple: %i", summ.simple);
						ImGui::Text("Radial basis: %i", summ.radialBasis);
						ImGui::Text("Random: %i", summ.random);
						ImGui::Text("Memory: %i", summ.memory);

						ImGui::NewLine();

						ImGui::Text("Total neurons: %i, dead end neurons: %i", summ.neurons, summ.deadend);
						ImGui::Text("Total connections: %i", summ.connections);
					}
					else
						goto Nothing;

				}
				else
				{
					Nothing:
					ImGui::Text("Nothing is selected");
				}
								
			}
			ImGui::End();
		}

		//Adaptation window
		if (showAdaptation)
		{
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGui::SetNextWindowSize({ 450.0f, 240.0f });
			ImGui::SetNextWindowPos({ 100 * 1.0f, 250.0f }, ImGuiCond_Once);

			ImGui::Begin("Adaptation", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			{
				//Caption
				ImGui::Text("Divers");

				//Input box
				//ImGui::InputInt("Phase", &Bot::adaptationStep);

				//Sliders
				ImGui::SliderInt("Land birth penalty", &Bot::adaptationStep, 0, 1000);
				ImGui::SliderInt("Sea birth penalty", &Bot::adaptationStep2, 0, 1000);
				ImGui::SliderInt("New bot should wait penalty", &Bot::adaptationStep3, 0, 20);

				ImGui::NewLine();

				ImGui::SliderInt("Ocean PS chance", &Bot::adaptationStep4, 0, 1000, "%d", ImGuiSliderFlags_Logarithmic);
				ImGui::SliderInt("Ocean level", &Bot::adaptationStep5, 0, FieldCellsHeight);
				ImGui::SliderInt("Mud level", &Bot::adaptationStep7, 0, FieldCellsHeight);
				ImGui::SliderInt("Force move", &Bot::adaptationStep6, 0, 1000);
			}
			ImGui::End();
		}

		//Chart window
		if (showChart)
		{
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGui::SetNextWindowSize({ 900.0f, 600.0f });
			ImGui::SetNextWindowPos({ 300 * 1.0f, 250.0f }, ImGuiCond_Once);

			ImGui::Begin("Population chart", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			{
				ImGui::SetNextItemWidth(3.0f);
				ImGui::PlotLines("Objects", populationChartData, chart_numValues, chart_shift, (const char*)0, 0.0f, 26000.0f, {800, 550});
			}
			ImGui::End();
		}

		if (showBrain)
		{
			if (selection)
			{
				//Bot brain window
				ImGui::SetNextWindowBgAlpha(1.0f);
				ImGui::SetNextWindowSize({ 330.0f, 180.0f });
				ImGui::SetNextWindowPos({ 650 * 1.0f, 800.0f });

				ImGui::Begin("Bot brain data", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				{
					if (nn_renderer.selectedNeuron)
					{
						//Show neuron type
						ImGui::Text("Neuron type:");
						ImGui::SameLine();
						ImGui::Text(Neuron::GetTextFromType(nn_renderer.selectedNeuron->type));

						//Show bias
						ImGui::Text("Bias: %f", nn_renderer.selectedNeuron->bias);

						//Show connections
						repeat(nn_renderer.selectedNeuron->numConnections)
						{
							ImGui::Text("Connection to %i, weight: %f", nn_renderer.selectedNeuron->allConnections[i].num, nn_renderer.selectedNeuron->allConnections[i].weight);
						}
					}
					else
					{
						ImGui::Text("Nothing selected");
					}
				}
				ImGui::End();
			}
		}

		ImGui::EndFrame();

		if (showBrain)
		{
			if (selection)
			{
				nn_renderer.Render(((Bot*)selectedObject)->GetBrain());
			}

		}

		//Present scene
		ImGui::Render();
		
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());		
		SDL_RenderPresent(renderer);

		//Delay so it would not eat too many resourses while on pause
		if(!simulate)
			SDL_Delay(20);

	}
exitfor:

	//Clear memory
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}


//Pause/unpause
void Pause()
{
	simulate = !simulate;
}

void ClearChart()
{
	memset(populationChartData, 0, sizeof(populationChartData));

	chart_numValues = 0;
	chart_shift = 0;
	chart_currentPosition = 0;
}

void AddToChart(float newVal)
{

	populationChartData[chart_currentPosition] = newVal;

	if (chart_numValues < ChartNumValues)
	{
		++chart_numValues;
		++chart_currentPosition;
	}
	else
	{
		if (chart_currentPosition == ChartNumValues)
			chart_currentPosition = 0;
		else
			++chart_currentPosition;
	}

}

void Deselect()
{
	selection = false;
	selectedObject = NULL;
	showBrain = false;
}


void ClearConsole()
{
	consoleText[0] = '\0';
}

//This is temporary
void ConsoleInput(const char* str, bool newLine)
{
	if (strlen(consoleText) > (ConsoleCharLength/2 - 50))
		ClearConsole();

	strcpy_s(consoleText + strlen(consoleText), ConsoleCharLength / 2, str);

	if (newLine)
		ConsoleInput("\r\n");
}

void ConsoleInput(int num, bool newLine)
{

	if (strlen(consoleText) > (ConsoleCharLength / 2 - 50))
		ClearConsole();

	sprintf_s(consoleText + strlen(consoleText), ConsoleCharLength / 2, "%i", num);

	if(newLine)
		ConsoleInput("\r\n");
}

void LoadFilenames()
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
		strcpy_s(filenamesFull[numFilenames], FilenameMaxLen, entry.path().string().c_str());
		strcpy_s(filenamesPartial[numFilenames++], FilenameMaxLen, entry.path().filename().string().c_str());
	}
}




void CreateNewFile()
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
