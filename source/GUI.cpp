#include "GUI.h"

using namespace ImGui;


void InitImGUI()
{
	CreateContext();

	io = &GetIO();

	ImPlot::CreateContext();

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	//Setup Dear ImGui style
	StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
}

void DeInitImGUI()
{
	ImGui_ImplSDLRenderer_Shutdown();
	ImPlot::DestroyContext();
	DestroyContext();
}

void GUIStartFrame()
{
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	NewFrame();
}

void Main::DrawDemoWindow()
{
	SetNextWindowBgAlpha(1.0f);
	SetNextWindowPos({ 20.0f,20.0f });

	ShowDemoWindow();
}

void Main::DrawMainWindow()
{
	SetNextWindowBgAlpha(1.0f);
	SetNextWindowSize({ GUIWindowWidth * 1.0f, 175.0f });
	SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, InterfaceBorder * 1.0f });

	Begin("Main", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		//FPS text 
		Text("steps: %i", ticknum);
		Text("(interval %i, ticks/sec: %i, fps: %i)", limit_interval, realTPS, realFPS);
		Text("Objects total: %i", field->GetNumObjects());
		Text("Bots total: %i", field->GetNumBots());

		//Show season name
		if(field->params.useSeasons)
			Text("Season: %s ( %i/%i )", SeasonNames[field->GetSeason()], field->GetSeasonCounter(), field->params.seasonInterval);

		//Neural net params and FOV x
		Text("Layers: %i, Neurons: %i, Render_X: %i", NumNeuronLayers, NumNeuronsInLayerMax, field->renderX);

		//Simulation seed and unique id
		Text("Seed: %i, simulation id: %i", seed, id);

		//World size and avg lifetime
		Text("World size: %i (%i screens)", FieldCellsWidth, (FieldCellsWidth) / (ScreenCellsWidth));

		Text("Avg bot lifetime: %i (max: %i)", field->GetAverageLifetime(), field->params.botMaxLifetime);

	}
	End();
}


void Main::DrawSystemWindow()
{
	SetNextWindowBgAlpha(1.0f);
	SetNextWindowSize({ GUIWindowWidth * 1.0f, 70.0f });
	SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 195.0f });

	Begin("System", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	{
		TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
		SameLine();
		Text(" %s", SDL_GetPlatform());

		SameLine();

		TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "CPU cores: %d", SDL_GetCPUCount());
		TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "RAM: %.2f GB", SDL_GetSystemRAM() / 1024.0f);
		
		SameLine();

		#if NumThreads == 1
		{
			Text(", single thread");
	}
		#else
		{
			uint threads = NumThreads;
			Text(", %i threads used", threads);
		}
		#endif
	}
	End();
}

void Main::DrawControlsWindow()
{
	SetNextWindowBgAlpha(1.0f);
	SetNextWindowSize({ GUIWindowWidth * 1.0f, 160.0f });
	SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 275.0f });

	Begin("Controls", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		if (Button((simulate) ? "stop" : "start", { 200, 25 }))
		{
			SwitchPause();
		}

		PushItemWidth(200);
		SliderInt("limit TPS", &limit_ticks_per_second, 0, GUI_Max_tps, "%d");
		SliderInt("limit FPS", &limitFPS, 0, GUI_Max_fps, "%d");

		SliderInt("PS reward", &(field->params.PSreward), 0, GUI_Max_food);		
		SliderInt("brush", &brushSize, GUI_Max_brush, 0, "%d");
	}
	End();
}

void Main::DrawSelectionWindow()
{
	SetNextWindowBgAlpha(1.0f);
	SetNextWindowSize({ GUIWindowWidth * 1.0f, 150.0f });
	SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 445.0f });

	Begin("Selection", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		if (selectedObject)
		{
			if (field->ValidateObjectExistance(selectedObject))
			{
				//Object info
				Text("type: Bot	X: %i, Y: %i", selectedObject->x, selectedObject->y);
				Text("lifetime: %i / %i", selectedObject->GetLifetime(), field->params.botMaxLifetime);
				Text("energy: %i (PS: %i, predation: %i)", selectedObject->energy, 
					((Bot*)selectedObject)->GetEnergyFromPS(), ((Bot*)selectedObject)->GetEnergyFromKills());

				//Mutation markers
				int m[NumberOfMutationMarkers];

				memcpy(m, ((Bot*)selectedObject)->GetMarkers(), sizeof(m));

				Text("markers: {");

				repeat(NumberOfMutationMarkers)
				{
					SameLine();
					Text("%i", m[i]);
				}

				SameLine();
				Text("}");

				//Color
				Color& c = *((Bot*)selectedObject)->GetColor();

				Text("color: {%i, %i, %i}", c.c[0], c.c[1], c.c[2]);

				SameLine();
				TextColored(ImVec4(((c.c[0] * 1.0f) / 255.0f), ((c.c[1] * 1.0f) / 255.0f), 
					((c.c[2] * 1.0f) / 255.0f), 1.0f), "*****");
				
				if (Button("Show brain", { 100, 25 }))
				{
					showBrain = !showBrain;
				}

				SameLine();

				if (Button("Repaint", { 70, 25 }))
				{
					field->RepaintBot((Bot*)selectedObject, Color::GetRandomColor(), RepaintTolerance);
				}
			}
			else
			{
				Deselect();
			}
		}
	}
	End();
}

void Main::DrawDisplayWindow()
{
	SetNextWindowBgAlpha(1.0f);
	SetNextWindowSize({ GUIWindowWidth * 1.0f, 100.0f});
	SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 605.0f });

	Begin("Display", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		BeginGroup();

		Text("Render:");

		RadioButton("Natural colors", (int*)&renderType, 0);
		SameLine();
		RadioButton("Predators", (int*)&renderType, 1);

		RadioButton("Energy", (int*)&renderType, 2);
		SameLine();
		RadioButton("No render", (int*)&renderType, 3);

		EndGroup();
	}
	End();
}

void Main::DrawLogWindow()
{
	SetNextWindowBgAlpha(1.0f);
	SetNextWindowSize({ GUIWindowWidth * 1.0f, 120.0f });
	SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 715.0f});

	Begin("Log", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		PushStyleColor(ImGuiCol_ChildBg, ImVec4(LogBackgroundColor));

		BeginChild("scrolling", ImVec2(265, 85), true);
		{
			TextUnformatted(logText.Buf.Data);

			if (GetScrollY() >= GetScrollMaxY())
				SetScrollHereY(1.0f);
		}
		EndChild();

		PopStyleColor();
	}
	End();
}

void Main::DrawMouseFunctionWindow()
{
	SetNextWindowBgAlpha(1.0f);
	SetNextWindowSize({ GUIWindowWidth * 1.0f, 100.0f});
	SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 845.0f });

	Begin("Mouse function", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		BeginGroup();

		RadioButton("Select", (int*)&mouseFunc, 0);
		SameLine();
		RadioButton("Remove", (int*)&mouseFunc, 1);

		RadioButton("Place rock", (int*)&mouseFunc, 2);
		SameLine();
		RadioButton("From file", (int*)&mouseFunc, 3);

		RadioButton("Mutate", (int*)&mouseFunc, 4);

		EndGroup();
	}
	End();
}

void Main::DrawAdditionalsWindow()
{
	SetNextWindowBgAlpha(1.0f);
	SetNextWindowSize({ GUIWindowWidth * 1.0f, 100.0f });
	SetNextWindowPos({ (2 * FieldX + FieldWidth) * 1.0f, 955.0f });

	Begin("Additional windows", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{

		if (Button("Save/load", { 85, 30 }))
		{
			LoadFilenames();

			showSaveLoad = !showSaveLoad;
		}
		SameLine();

		if (Button("Dangerous", { 85, 30 }))
		{
			showDangerous = !showDangerous;
		}
		SameLine();

		if (Button("Adaptation", { 85, 30 }))
		{
			showAdaptation = !showAdaptation;
		}

		if (Button("Chart", { 85, 30 }))
		{
			showChart = !showChart;
		}
		SameLine();

		if (Button("Auto adapt", { 85, 30 }))
		{
			showAutomaticAdaptation = !showAutomaticAdaptation;
		}

	}
	End();
}

void Main::DrawSaveLoadWindow()
{
	if (showSaveLoad)
	{
		//Save/load window
		SetNextWindowBgAlpha(1.0f);
		SetNextWindowSize({ 400.0f, 230.0f });
		SetNextWindowPos({ 100 * 1.0f, 100.0f }, ImGuiCond_Once);

		Begin("Save/load", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			//List of files
			Text("Select file");

			ListBoxHeader("##", ImVec2(380, 110));

			for (int i = 0; i < allFilenames.size(); ++i)
			{
				if (Selectable(allFilenames[i].fullCaption.c_str(), &allFilenames[i].isSelected))
				{
					for (int b = 0; b < allFilenames.size(); ++b)
						allFilenames[b].isSelected = false;

					allFilenames[i].isSelected = true;
					selectedFile = &allFilenames[i];
				}
			}

			ListBoxFooter();

			//Buttons

			if (Button("Load", { 50, 30 }))
			{
				if (selectedFile)
				{
					if (selectedFile->isWorld)
					{
						ObjectSaver::WorldParams ret = saver.LoadWorld(field, (char*)selectedFile->nameFull.c_str());

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
					else
					{
						if (selectedObject)
							delete selectedObject;

						selectedObject = saver.LoadObject((char*)selectedFile->nameFull.c_str());

						if (selectedObject)
							LogPrint("Object loaded\r\n");
						else
							LogPrint("Error while loading object\r\n");
					}
				}
			}			

			SameLine();

			if (Button("Save bot", { 100, 30 }))
			{
				if (selectedObject)
				{
					if (selectedFile)
					{
						if (saver.SaveObject(selectedObject, (char*)selectedFile->nameFull.c_str()))
						{
							LogPrint("Object saved\r\n");

							LoadFilenames();
						}
						else
						{
							LogPrint("Error while saving object\r\n");
						}
					}
				}
			}

			SameLine();

			if (Button("Save world", { 100, 30 }))
			{
				if (selectedFile)
				{
					if (saver.SaveWorld(field, (char*)selectedFile->nameFull.c_str(), id, ticknum))
					{
						LogPrint("World saved\r\n");

						LoadFilenames();
					}
					else
					{
						LogPrint("Error while saving world\r\n");
					}
				}
			}

			SameLine();

			if (Button("New file", { 100, 30 }))
			{
				CreateNewFile();

				LoadFilenames();
			}

			if (selectedFile)
			{
				if (selectedFile->isWorld)
				{
					if (Button("Load landscape keep bots", { 150, 30 }))
					{
						ObjectSaver::WorldParams ret = saver.LoadWorld(field, (char*)selectedFile->nameFull.c_str(),
							false, false, true, false);

						if (ret.id != -1)
						{
							if (ret.width != FieldCellsWidth)
								LogPrint("Landscape loaded(width mismatch)\r\n");
							else
								LogPrint("Landscape loaded\r\n");
						}
						else
							LogPrint("Error while loading Landscape\r\n");
					}

					SameLine();

					if (Button("Load only bots", { 150, 30 }))
					{
						ObjectSaver::WorldParams ret = saver.LoadWorld(field, (char*)selectedFile->nameFull.c_str(),
							false, false, false, true);

						if (ret.id != -1)
						{
							if (ret.width != FieldCellsWidth)
								LogPrint("Landscape loaded(width mismatch)\r\n");
							else
								LogPrint("Landscape loaded\r\n");
						}
						else
							LogPrint("Error while loading Landscape\r\n");
					}
				}
			}
		}
		End();
	}
}

void Main::DrawDangerousWindow()
{
	if (showDangerous)
	{
		SetNextWindowBgAlpha(1.0f);
		SetNextWindowSize({ 290.0f, 100.0f });
		SetNextWindowPos({ 100 * 1.0f, 300.0f }, ImGuiCond_Once);

		Begin("Use with caution!", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			if (Button("Wipe world", { 130, 30 }))
			{
				ClearWorld();
				field->params.Reset();
				simulation.chart.ClearChart();
			}

			SameLine();

			if (Button("Kill bots", { 130, 30 }))
			{
				Deselect();

				for (int cx = 0; cx < FieldCellsWidth; ++cx)
				{
					for (int cy = 0; cy < FieldCellsHeight; ++cy)
					{
						Object* o = field->GetObjectLocalCoords(cx, cy);

						if (o == NULL)
							continue;

						if (o->type() == bot)
							field->RemoveObject(cx, cy);
					}
				}
			}

			if (Button("CloseApp", { 130, 30 }))
			{
				terminate = true;
			}

			SameLine();

			if (Button("Zero time", { 130, 30 }))
			{
				ticknum = 0;
			}
		}
		End();
	}
}

void Main::DrawSummaryWindow()
{
	if (showBrain)
	{
		SetNextWindowBgAlpha(1.0f);
		SetNextWindowSize({ 330.0f, 180.0f });
		SetNextWindowPos({ 100 * 1.0f, 150.0f }, ImGuiCond_Once);

		Begin("Bot summary", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			if (selectedObject)
			{
				if (field->ValidateObjectExistance(selectedObject))
				{
					auto summ = ((Bot*)selectedObject)->GetNeuronSummary();

					Text("Neurons:");
					Text("Simple: %i", summ.simple);
					Text("Radial basis: %i", summ.radialBasis);
					Text("Random: %i", summ.random);
					Text("Memory: %i", summ.memory);

					NewLine();

					Text("Total neurons: %i, dead end neurons: %i", summ.neurons, summ.deadend);
					Text("Total connections: %i", summ.connections);
				}
				else
					goto Nothing;

			}
			else
			{
			Nothing:
				Text("Nothing is selected");
			}

		}
		End();
	}
}

void Main::DrawAdaptationWindow()
{
	if (showAdaptation)
	{
		SetNextWindowBgAlpha(1.0f);
		SetNextWindowSize({ 600.0f, 650.0f });
		SetNextWindowPos({ 100 * 1.0f, 250.0f }, ImGuiCond_Once);

		Begin("Adaptation", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			if(CollapsingHeader("Winds"))			
			{
				SliderInt("X steps to divide", &field->params.adaptation_StepsNumToDivide_Winds, 0, 200);
				SliderInt("Force movements X", &field->params.adaptation_forceBotMovementsX, 0, 1000);
			}

			NewLine();
			
			if (CollapsingHeader("Divers", ImGuiTreeNodeFlags_DefaultOpen))
			{
				SliderInt("Land mulpitly block", &field->params.adaptation_landBirthBlock, 0, 1000);
				SliderInt("Sea mulpitly block", &field->params.adaptation_seaBirthBlock, 0, 1000);

				NewLine();

				SliderInt("no PS in ocean", &field->params.adaptation_PSInOceanBlock, 0, 1000, "%d");
				SliderInt("no PS in mud", &field->params.adaptation_PSInMudBlock, 0, 1000, "%d");
				SliderInt("On land at least once", &field->params.adaptation_botShouldBeOnLandOnceToMultiply, 0, 1000, "%d");
				SliderInt("On land PS at least once", &field->params.adaptation_botShouldDoPSOnLandOnceToMultiply, 0, 1000, "%d");				
				SliderInt("Force movements Y", &field->params.adaptation_forceBotMovementsY, 0, 1000);

				NewLine();

				SliderInt("Ocean level", &field->params.oceanLevel, 0, FieldCellsHeight);
				SliderInt("Mud level", &field->params.mudLevel, 0, FieldCellsHeight);
			}

			NewLine();

			if (CollapsingHeader("Organics"))
			{
				SliderInt("Organics spawn rate", &field->params.adaptation_organicSpawnRate, 0, 1000);
			}

			NewLine();

			if (CollapsingHeader("Apples"))
			{
				SliderInt("Apple energy", &field->params.appleEnergy, 1, 200);
				Checkbox("Spawn apples", &field->params.spawnApples);
			}

			NewLine();

			if (CollapsingHeader("More"))
			{
				SliderInt("Bot max lifetime", &field->params.botMaxLifetime, MaxBotLifetime_Min, MaxBotLifetime_Max);
				SliderInt("Fertility delay", &field->params.fertility_delay, 0, field->params.botMaxLifetime);

				Checkbox("No mutations", &field->params.noMutations);
				Checkbox("No predators", &field->params.noPredators);
			}

			NewLine();

			if (CollapsingHeader("Seasons"))
			{	
				Checkbox("Use seasons", &field->params.useSeasons);
				SliderInt("Season change interval", &field->params.seasonInterval, 200, 4000);
			}

			NewLine();

			if (Button("Reset", { 70, 20 }))
			{
				field->params.Reset();
			}
		}
		End();
	}
}

void Main::DrawChartWindow()
{
	if (showChart)
	{
		SetNextWindowBgAlpha(1.0f);
		SetNextWindowSize({ 920.0f, 600.0f });
		SetNextWindowPos({ 700.0f, 250.0f }, ImGuiCond_Once);

		Begin("Population chart", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			chart.Plot();
		}
		End();
	}
}

void Main::DrawBotBrainWindow()
{	
	if (showBrain)
	{
		if (selectedObject)
		{
			SetNextWindowBgAlpha(1.0f);
			SetNextWindowSize({ 330.0f, 280.0f });
			SetNextWindowPos({ 650 * 1.0f, 350.0f });

			Begin("Bot brain data", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			{
				BeginGroup();

				RadioButton("Active brain", &brainToShow, 0);
				RadioButton("Initial brain", &brainToShow, 1);

				EndGroup();

				if (nn_renderer.selectedNeuron)
				{
					//Show neuron type
					Text("Neuron type:");
					SameLine();
					Text(NeuronTypeNames[nn_renderer.selectedNeuron->type]);

					if (nn_renderer.selectedNeuronCaption != "")
					{
						SameLine();
						Text(" (%s)", nn_renderer.selectedNeuronCaption);
					}

					//Show value 
					Text("Value: %f", nn_renderer.selectedNeuronValue);

					//Show memory
					if(nn_renderer.selectedNeuron->type == memory)
						Text("Memory: %f", nn_renderer.selectedNeuronMemory);

					//Show bias
					Text("Bias: %f", nn_renderer.selectedNeuron->bias);

					//Show connections
					repeat(nn_renderer.selectedNeuron->numConnections)
					{
						Text("Connection to l: %i, n: %i, weight: %f", nn_renderer.selectedNeuron->allConnections[i].dest_layer,
							nn_renderer.selectedNeuron->allConnections[i].dest_neuron, nn_renderer.selectedNeuron->allConnections[i].weight);
					}

					//Show memory data somehow(TODO)
					//Text("Memory data: %i", nn_renderer.selectedBrain->allMemory[][]);

					//TODO
					if((nn_renderer.selectedNeuron->type!=input) and (nn_renderer.selectedNeuron->type != output))
					{
						if (Button("set random", { 100,30 }))
						{
							nn_renderer.selectedNeuron->SetRandom();
						}

						SameLine();

						if (Button("set zero", { 100,30 }))
						{
							nn_renderer.selectedNeuron->SetZero();
						}
					}
				}
				else
				{
					Text("Nothing selected");
				}

				NewLine();

				if (Button("Mutate brain", { 100,30 }))
				{
					if(nn_renderer.selectedBrain)
						nn_renderer.selectedBrain->Mutate();
				}
			}

			End();

			if(brainToShow == 0)
				nn_renderer.Render(((Bot*)selectedObject)->GetActiveBrain());
			else
				nn_renderer.Render(((Bot*)selectedObject)->GetInitialBrain());
		}
	}
}

void Main::DrawAAWindow()
{
	if (showAutomaticAdaptation)
	{
		auto_adapt->DrawWindow();
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
	DrawDisplayWindow();
	DrawLogWindow();
	DrawMouseFunctionWindow();
	DrawAdditionalsWindow();

	//Below windows that are hidden at startup

	DrawSaveLoadWindow();
	DrawDangerousWindow();
	DrawSummaryWindow();
	DrawAdaptationWindow();
	DrawChartWindow();
	DrawBotBrainWindow();
	DrawAAWindow();
}

void Main::MouseClick()
{
	if (!(nn_renderer.MouseClick({ mouseState.mouseX, mouseState.mouseY }) and (showBrain)))
	{
		if (field->IsInBoundsScreenCoords(mouseState.mouseX, mouseState.mouseY))
		{
			Point fieldCoords = field->ScreenCoordsToLocal(mouseState.mouseX, mouseState.mouseY);

			if (fieldCoords.x < 0)
			{
				fieldCoords.x = 0;
			}
			else if (fieldCoords.x >= FieldCellsWidth)
			{
				fieldCoords.x = FieldCellsWidth - 1;
			}

			Object* obj = field->GetObjectLocalCoords(fieldCoords.x, fieldCoords.y);

			if (mouseFunc == mouse_select)
			{
				if (obj)
				{
					if (obj->type() == bot)
					{
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
							field->ObjectAddOrReplace(new Rock(fieldCoords.x + cx, fieldCoords.y + cy));
						}
					}
				}
			}
			else if (mouseFunc == mouse_from_file)
			{
				//Cell is empty				
				if (!obj)
				{
					if (selectedFile)
					{
						obj = saver.LoadObject((char*)selectedFile->nameFull.c_str());

						if (obj)
						{
							obj->x = fieldCoords.x;
							obj->y = fieldCoords.y;
							obj->energy = MaxPossibleEnergyForABot;

							if (field->AddObject(obj))
							{
								LogPrint("Object loaded\r\n");
							}
						}
						else
						{
							LogPrint("Error while loading object\r\n");
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
								if (obj->type() == bot)
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
	//Limit FPS
	TimePoint currentTickFps = clock.now();

	if (renderType == noRender)
	{
		fpsInterval = 1000 / GUI_FPSWhenNoRender;
	}
	else
	{
		if (limitFPS > 0)
		{
			fpsInterval = 1000 / limitFPS;
		}
		else
		{
			fpsInterval = 0;
		}
	}

	if (TimeMSBetween(currentTickFps, lastTickFps) < fpsInterval)
	{
		return;
	}
	else
	{
		lastTickFps = currentTickFps;
	}

	//Begin frame

	//Clear background
	glClearColor(BackgroundColorFloat);
	glClear(GL_COLOR_BUFFER_BIT);

	//Render
	if (renderType != noRender)
	{
		field->draw(renderType);

		//Highlight selected object
		HighlightSelection();

		++fpsCounter;
	}

	SelectionShadowScreen();

	//Draw GUI
	GUIStartFrame();
	
	DrawWindows();

	EndFrame();

	//Present scene
	SDLPresentScene();

	//Calculate fps
	if (TimeMSBetween(currentTickFps, lastSecondTickFps) >= 1000)
	{
		lastSecondTickFps = currentTickFps;

		realFPS = fpsCounter;
		fpsCounter = 0;
	}
}

void Main::RunWithNoRender()
{
	limit_ticks_per_second = 0;
	renderType = noRender;

	Start();
}

void Main::RunWithMinimumRender()
{
	limit_ticks_per_second = 0;
	limitFPS = 25;
	renderType = natural;

	Start();
}

void Main::Print(string s)
{
	LogPrint(s.c_str());
}


void Main::ClearWorld()
{
	Deselect();

	field->RemoveAllObjects();

	ticknum = 0;
}
