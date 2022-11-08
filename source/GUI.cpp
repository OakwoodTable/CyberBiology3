
#include "GUI.h"



void InitImGUI()
{
	ImGui::CreateContext();

	io = &ImGui::GetIO();

	ImPlot::CreateContext();

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	//Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
}

void DeInitImGUI()
{
	ImGui_ImplSDLRenderer_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}



void GUIStartFrame()
{
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}



void Main::DrawDemoWindow()
{
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::SetNextWindowPos({ 20.0f,20.0f });

	ImGui::ShowDemoWindow();
}

void Main::DrawMainWindow()
{
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 135.0f });
	ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, InterfaceBorder * 1.0f });

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

		//Neural net params and FOV x
		ImGui::Text("Layers: %i, Neurons: %i, Render_X: %i", NumNeuronLayers, NeuronsInLayer, field->renderX);

		//Simulation seed and unique id
		ImGui::Text("Seed: %i, simulation id: %i", seed, id);

	}
	ImGui::End();
}


void Main::DrawSystemWindow()
{
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
}

void Main::DrawControlsWindow()
{
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
		ImGui::InputInt("PS reward", &(field->foodBase), 0, 5);

		if (field->foodBase > GUI_Max_food)
			field->foodBase = GUI_Max_food;

		ImGui::SliderInt("skip", &skipFrames, GUI_Max_skip, 0, "%d", ImGuiSliderFlags_None);
		ImGui::SliderInt("brush", &brushSize, GUI_Max_brush, 0, "%d", ImGuiSliderFlags_None);
	}
	ImGui::End();
}

void Main::DrawSelectionWindow()
{
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
				ImGui::Text("energy: %i (PS: %i, predation: %i)", selectedObject->energy, ((Bot*)selectedObject)->GetEnergyFromPS(), ((Bot*)selectedObject)->GetEnergyFromKills());

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
					field->RepaintBot((Bot*)selectedObject, Bot::GetRandomColor(), 1);
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
}

void Main::DrawRenderWindow()
{
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 140.0f});
	ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 550.0f });

	ImGui::Begin("Display", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		ImGui::BeginGroup();

		ImGui::Text("Render:");

		ImGui::RadioButton("Natural colors", (int*)&renderType, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Predators", (int*)&renderType, 1);

		ImGui::RadioButton("Energy", (int*)&renderType, 2);
		ImGui::SameLine();
		ImGui::RadioButton("No render", (int*)&renderType, 3);

		ImGui::EndGroup();
	}
	ImGui::End();
}

void Main::DrawConsoleWindow()
{
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 120.0f });
	ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 700.0f});

	ImGui::Begin("Console", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		ImGui::InputTextMultiline("##", consoleText, ConsoleCharLength, ImVec2(240, 90), ImGuiInputTextFlags_EnterReturnsTrue);
	}
	ImGui::End();
}

void Main::DrawMouseFunctionWindow()
{
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 130.0f});
	ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 830.0f });

	ImGui::Begin("Mouse function", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		ImGui::BeginGroup();

		ImGui::RadioButton("Select", (int*)&mouseFunc, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Remove", (int*)&mouseFunc, 1);

		ImGui::RadioButton("Place rock", (int*)&mouseFunc, 2);
		ImGui::SameLine();
		ImGui::RadioButton("From file", (int*)&mouseFunc, 3);

		ImGui::RadioButton("Mutate", (int*)&mouseFunc, 4);

		ImGui::EndGroup();
	}
	ImGui::End();
}

void Main::DrawAdditionalsWindow()
{
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::SetNextWindowSize({ GUIWindowWidth * 1.0f, 110.0f });
	ImGui::SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 970.0f });

	ImGui::Begin("Additional windows", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{

		if (ImGui::Button("Save/load", { 75, 30 }))
		{
			LoadFilenames();

			showSaveLoad = !showSaveLoad;
		}
		ImGui::SameLine();

		if (ImGui::Button("Dangerous", { 75, 30 }))
		{
			showDangerous = !showDangerous;
		}
		ImGui::SameLine();

		if (ImGui::Button("Adaptation", { 75, 30 }))
		{
			showAdaptation = !showAdaptation;
		}

		if (ImGui::Button("Chart", { 75, 30 }))
		{
			showChart = !showChart;
		}

	}
	ImGui::End();
}

void Main::DrawSaveLoadWindow()
{
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
				if (ImGui::Selectable(filenamesPartial[i].c_str(), &selectedFilenames[i]))
				{
					ZeroMemory(selectedFilenames, sizeof(selectedFilenames));

					selectedFilenames[i] = true;
					selectedFilename = i;
				}
			}
			ImGui::ListBoxFooter();

			//Buttons
			if (ImGui::Button("Save", { 50, 30 }))
			{
				if (selection)
				{
					if (selectedFilename != -1)
					{
						if (field->saver.SaveObject(selectedObject, (char*)filenamesFull[selectedFilename].c_str()))
							ConsoleInput("Object saved\r\n");
						else
							ConsoleInput("Error while saving object\r\n");
					}
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Load", { 50, 30 }))
			{
				if (selection)
				{
					if (selectedFilename != -1)
					{
						if (selectedObject)
						{
							delete selectedObject;
						}

						selectedObject = field->saver.LoadObject((char*)filenamesFull[selectedFilename].c_str());

						if (selectedObject)
							ConsoleInput("Object loaded\r\n");
						else
							ConsoleInput("Error while loading object\r\n");
					}
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("New", { 50, 30 }))
			{
				CreateNewFile();
			}

			ImGui::SameLine();

			if (ImGui::Button("Save world", { 70, 30 }))
			{
				if (selectedFilename != -1)
				{
					if (field->saver.SaveWorld(field, (char*)filenamesFull[selectedFilename].c_str(), id, ticknum))
						ConsoleInput("World saved\r\n");
					else
						ConsoleInput("Error while saving world\r\n");
				}
			}
			
			ImGui::SameLine();

			if (ImGui::Button("Load world", { 70, 30 }))
			{
				if (selectedFilename != -1)
				{
					ObjectSaver::WorldParams ret = field->saver.LoadWorld(field, (char*)filenamesFull[selectedFilename].c_str());

					if (ret.id != -1)
					{
						if(ret.width != FieldCellsWidth)
							ConsoleInput("World loaded(width mismatch)\r\n");
						else
							ConsoleInput("World loaded\r\n");

						seed = ret.seed;
						ticknum = ret.tick;
						id = ret.id;

						field->seed = seed;
					}
					else
						ConsoleInput("Error while loading world\r\n");
				}
			}
		}
		ImGui::End();
	}
}

void Main::DrawDangerousWindow()
{
	if (showDangerous)
	{
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ 290.0f, 100.0f });
		ImGui::SetNextWindowPos({ 100 * 1.0f, 300.0f }, ImGuiCond_Once);

		ImGui::Begin("Use with caution!", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{

			if (ImGui::Button("Wipe world", { 130, 30 }))
			{
				ClearWorld();
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

						if (o->type == bot)
							field->RemoveObject(cx, cy);
					}
				}
			}

			if (ImGui::Button("CloseApp", { 130, 30 }))
			{
				//TODO - rework this
				terminate = true;

				SDL_Quit();
				exit(0);
			}

			ImGui::SameLine();

			if (ImGui::Button("Zero time", { 130, 30 }))
			{
				ticknum = 0;
			}
		}
		ImGui::End();
	}
}

void Main::DrawSummaryWindow()
{
	if (showSummary)
	{
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ 330.0f, 180.0f });
		ImGui::SetNextWindowPos({ 100 * 1.0f, 150.0f }, ImGuiCond_Once);

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
}

void Main::DrawAdaptationWindow()
{
	if (showAdaptation)
	{
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ 470.0f, 400.0f });
		ImGui::SetNextWindowPos({ 100 * 1.0f, 250.0f }, ImGuiCond_Once);

		ImGui::Begin("Adaptation", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{

			ImGui::Text("Winds");

			ImGui::SliderInt("Phase", &field->params.adaptation_DeathChance_Winds, 0, 1000);
			ImGui::SliderInt("Steps", &field->params.adaptation_StepsNum_Winds, 0, 20);

			ImGui::NewLine();

			ImGui::Text("Divers");

			ImGui::SliderInt("Land mulpitly block", &field->params.adaptation_landBirthBlock, 0, 1000);
			ImGui::SliderInt("Sea mulpitly block", &field->params.adaptation_seaBirthBlock, 0, 1000);

			ImGui::NewLine();

			ImGui::SliderInt("no PS in ocean", &field->params.adaptation_PSInOceanBlock, 0, 1000, "%d");
			ImGui::SliderInt("no PS in mud", &field->params.adaptation_PSInMudBlock, 0, 1000, "%d");
			ImGui::SliderInt("On land at least once", &field->params.adaptation_botShouldBeOnLandOnceToMultiply, 0, 1000, "%d");
			ImGui::SliderInt("Force movements", &field->params.adaptation_forceBotMovements, 0, 1000);

			ImGui::NewLine();

			ImGui::SliderInt("Ocean level", &field->params.oceanLevel, 0, FieldCellsHeight);
			ImGui::SliderInt("Mud level", &field->params.mudLevel, 0, FieldCellsHeight);			

			ImGui::SliderInt("Apple energy", &field->params.appleEnergy, 1, 200);
		}
		ImGui::End();
	}
}

void Main::DrawChartWindow()
{
	if (showChart)
	{
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize({ 900.0f, 600.0f });
		ImGui::SetNextWindowPos({ 300 * 1.0f, 250.0f }, ImGuiCond_Once);

		ImGui::Begin("Population chart", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			if (ImPlot::BeginPlot("Objects", { 800, 550 }))
			{

				//Axes
				ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 250.0, ImPlotCond_Always);
				ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 26000.0);

				ImPlot::SetupAxis(ImAxis_X1, "Time");
				ImPlot::SetupAxis(ImAxis_Y1, "Number");

				//Bots number
				ImPlot::SetNextLineStyle({ 1, 0, 0, 1 }, ChartLineThickness);

				ImPlot::PlotLine("Bots", chartData_bots, chart_numValues, 1.0f, 0.0f, ImPlotLineFlags_None);

				//Apples number
#ifdef UseApples
				ImPlot::SetNextLineStyle({ 0, 1, 0, 1 }, 1.5f);

				ImPlot::PlotLine("Apples", chartData_apples, chart_numValues, 1.0f, 0.0f, ImPlotLineFlags_None);
#endif

				//Organics number
#ifdef SpawnOrganicWasteWhenBotDies
				ImPlot::SetNextLineStyle({ 0, 0, 1, 1 }, 1.5f);

				ImPlot::PlotLine("Organics", chartData_organics, chart_numValues, 1.0f, 0.0f, ImPlotLineFlags_None);
#endif

				ImPlot::EndPlot();
			}

			//Buttons
			ImGui::SameLine();

			if (ImGui::Button("Clear", { 70.0f, 30.0f }))
				ClearChart();
		}
		ImGui::End();
	}
}

void Main::DrawBotBrainWindow()
{	
	if (showBrain)
	{
		if (selection)
		{
			//Bot brain window
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGui::SetNextWindowSize({ 330.0f, 240.0f });
			ImGui::SetNextWindowPos({ 650 * 1.0f, 500.0f });

			ImGui::Begin("Bot brain data", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			{
				ImGui::BeginGroup();

				ImGui::RadioButton("Active brain", &brainToShow, 0);
				ImGui::RadioButton("Initial brain", &brainToShow, 1);

				ImGui::EndGroup();

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
						ImGui::Text("Connection to %i, weight: %f", nn_renderer.selectedNeuron->allConnections[i].dest, nn_renderer.selectedNeuron->allConnections[i].weight);
					}

					//Show memory data
					//ImGui::Text("Memory data: %i", nn_renderer.selectedBrain->allMemory[][]);

					//TODO
					if((nn_renderer.selectedNeuron->type!=input) && (nn_renderer.selectedNeuron->type != output))
					{
						if (ImGui::Button("set random", { 100,30 }))
						{
							nn_renderer.selectedNeuron->SetRandom();
						}
						else if (ImGui::Button("set zero", { 100,30 }))
						{
							nn_renderer.selectedNeuron->SetZero();
						}
					}
				}
				else
				{
					ImGui::Text("Nothing selected");
				}
			}
			ImGui::End();

			if(brainToShow == 0)
				nn_renderer.Render(((Bot*)selectedObject)->GetActiveBrain());
			else
				nn_renderer.Render(((Bot*)selectedObject)->GetInitialBrain());
		}
	}
}

void Main::DrawWindows()
{
	//ImGUI demo window
	#ifdef ShowGUIDemoWindow
		DrawDemoWindow();
	#endif

	DrawMainWindow();
	DrawSystemWindow();
	DrawControlsWindow();
	DrawSelectionWindow();
	DrawRenderWindow();
	DrawConsoleWindow();
	DrawMouseFunctionWindow();
	DrawAdditionalsWindow();

	//Below windows that are hidden at startup

	DrawSaveLoadWindow();
	DrawDangerousWindow();
	DrawSummaryWindow();
	DrawAdaptationWindow();
	DrawChartWindow();
	DrawBotBrainWindow();
}

void Main::MouseClick()
{
	if (!(nn_renderer.MouseClick({ mouseState.mouseX, mouseState.mouseY }) && (showBrain)))
	{
		if (field->IsInBoundsScreenCoords(mouseState.mouseX, mouseState.mouseY))
		{
			Point fieldCoords = field->ScreenCoordsToLocal(mouseState.mouseX, mouseState.mouseY);
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
				//Cell is empty
				if (!obj)
				{
					if (selectedFilename != -1)
					{
						obj = field->saver.LoadObject((char*)filenamesFull[selectedFilename].c_str());

						if (obj)
						{
							obj->x = fieldCoords.x;
							obj->y = fieldCoords.y;
							obj->energy = MaxPossibleEnergyForABot;

							if (field->AddObject(obj))
							{
								ConsoleInput("Object loaded\r\n");

								selection = true;
								selectedObject = obj;
							}
						}
						else
						{
							ConsoleInput("Error while loading object\r\n");
						}
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
}

void Main::Render()
{
	//Skip drawing
	if (simulation.renderType != noRender)
	{
		if (simulation.skipFrames)
		{
			if (--skipping > 0)
			{
				return;
			}
			else
			{
				skipping = simulation.skipFrames;
			}
		}
	}
	else
	{
		if (--skipping > 0)
		{
			return;
		}
		else
		{
			skipping = SkipGUIFramesWhenNoRender;
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
		{
			field->draw(renderType);

			//Highlight selected object
			HighlightSelection();
		}
		else
		{
			if (skippingRender-- == 0)
			{
				skippingRender = SkipRenderingFramesWhileOnPause;
				field->draw(renderType);

				//Highlight selected object
				HighlightSelection();
			}
		}
	}

	SelectionShadowScreen();

	//Draw GUI
	GUIStartFrame();
	
	DrawWindows();

	ImGui::EndFrame();

	//Present scene
	SDLPresentScene();

	//Delay so it would not eat too many resourses while on pause
	if (!simulate)
		SDL_Delay(20);
}


void Main::ClearWorld()
{
	Deselect();

	field->RemoveAllObjects();

	ticknum = 0;
}
