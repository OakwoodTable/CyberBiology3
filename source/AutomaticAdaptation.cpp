#include "AutomaticAdaptation.h"


vector<uint>* ParameterSweep::history;


void AutomaticAdaptation::NextPhase(const char* desc)
{
	++phase;
	phaseDesc = desc;

	string s = "Phase " + to_string(phase);
	s += "/" + to_string(numPhases);
	s += " ticks: " + to_string(currentTick - phaseBeginTick);
	PrintInLog(s);
}

void AutomaticAdaptation::Finish()
{
	phaseDesc = NULL;
	PrintInLog("done!");

	inProgress = false;
	sim->Pause();
}

void ParameterSweep::SetTick(uint tick)
{
	currentTick = tick;
}

void ParameterSweep::WaitPopulation(int pop, int count)
{
	waitForPopulation = pop;
	sweep_counter = count;
	beginTick = currentTick;
}

void ParameterSweep::BeginSweep(int* p, int begin, int inc, int goal, int pop, int count, int addToChartEvery)
{
	toSweep = p;
	sweepIncrement = inc;
	sweepGoal = goal;
	waitForPopulation = pop;
	chartValPrev = 0;
	addToChartRate = addToChartEvery;

	*toSweep = begin;
	sweep_rate = count;
	sweep_counter = sweep_rate;

	beginTick = currentTick;
}

bool ParameterSweep::CheckSweep(uint numBots)
{
	if (toSweep)
	{
		if ((sweep_counter <= 0) or (numBots > 20000))//(waitForPopulation * 2)))
		{
			if (numBots >= waitForPopulation)
			{
				if (*toSweep == sweepGoal)
				{
					if (addToChartRate == 0)
					{
						AddToChart();
					}

					toSweep = NULL;

					return true;
				}

				*toSweep += sweepIncrement;

				if (addToChartRate > 0)
				{
					if (chartValPrev == addToChartRate)
					{
						chartValPrev = 0;
						AddToChart();
					}
					else
					{
						++chartValPrev;
					}
				}

				sweep_counter = sweep_rate;
			}
		}
		else
		{
			--sweep_counter;
		}

		return false;
	}
	else
	{
		if (numBots >= waitForPopulation)
		{
			AddToChart();
			return true;
		}
	}

	return false;
}

void ParameterSweep::Clear()
{
	currentTick = 0u;
	beginTick = 0u;
	toSweep = NULL;
}

void ParameterSweep::AddToChart()
{
	//TODO: fix
	if ((currentTick - beginTick) > 1)
		history->push_back(currentTick - beginTick);
}

void AutomaticAdaptation::AutoAdaptDivers()
{
	switch (phase)
	{
	case 1:
		sweep.BeginSweep(&params->adaptation_forceBotMovementsY, 
			0, 100, 1000, 10000, 30);

		NextPhase("forceMovementsY");
		break;

	case 2:
		params->adaptation_landBirthBlock = 1000;

		sweep.BeginSweep(&params->adaptation_botShouldBeOnLandOnceToMultiply,
			0, 100, 1000, 15000, 30);

		NextPhase("on land once");
		break;

	case 3:
		params->adaptation_seaBirthBlock = 1000;

		sweep.BeginSweep(&params->mudLevel, params->oceanLevel - 1,
			-1, 24, 15000, 60);

		NextPhase("mud level");
		break;

	case 4:
		sweep.BeginSweep(&params->adaptation_PSInMudBlock,
			0, 50, 1000, 15000, 150);

		NextPhase("no PS in mud");
		break;

	case 5:
		sweep.BeginSweep(&params->adaptation_PSInOceanBlock,
			0, 50, 1000, 12000, 150);

		NextPhase("no PS in water");
		break;

	case 6:
		params->adaptation_botShouldBeOnLandOnceToMultiply = 0;

		sweep.BeginSweep(&params->adaptation_forceBotMovementsY,
			1000, -100, 0, 12000, 200);

		NextPhase("force move Y disable");
		break;

	default:
		Finish();
	}
}

void AutomaticAdaptation::AutoAdaptWinds()
{
	switch (phase)
	{
	case 1:
		sweep.BeginSweep(&params->adaptation_forceBotMovementsX,
				0, 100, 1000, 8000, 50);

		NextPhase("forceMovementsX");
		break;

	case 2:
		sweep.BeginSweep(&params->adaptation_StepsNumToDivide_Winds,
				0, 5, 165, 5000, 150, 2);

		NextPhase("X dist. to divide");
		break;


	case 4:
		params->adaptation_forceBotMovementsX = 0;

		sweep.WaitPopulation(5000, 200);
		NextPhase();
		break;

	default:
		Finish();
	}
}

void AutomaticAdaptation::PrintInLog(string s)
{
	s += "\r\n";

	AALog.append(s.c_str());
	sim->Print(s);
}

void AutomaticAdaptation::Plot()
{
	using namespace ImPlot;

	if (BeginPlot("Results", { 500, 350 }))
	{
		//Axes
		SetupAxisLimits(ImAxis_X1, 0.0, 1.0f * history.size(), ImPlotCond_Always);
		SetupAxisLimits(ImAxis_Y1, 0.0, 2000.0f);

		SetupAxis(ImAxis_X1, "Step");
		SetupAxis(ImAxis_Y1, "Time");

		SetAxis(ImAxis_Y1);
		SetNextLineStyle({ 1, 1, 1, 1 }, ChartLineThickness);

		PlotLine("1", history.data(), static_cast<int>(history.size()),
			1.0f, 0.0f, ImPlotLineFlags_None);

		EndPlot();
	}
}

void AutomaticAdaptation::DrawWindow()
{
	using namespace ImGui;

	static bool visualize = true;
	static int iterations = 1;

	SetNextWindowBgAlpha(1.0f);
	SetNextWindowSize({ 600.0f, 750.0f });
	SetNextWindowPos({ 700.0f, 250.0f }, ImGuiCond_Once);


	Begin("Automatic adaptation", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	{
		PushStyleColor(ImGuiCol_ChildBg, ImVec4(LogBackgroundColor));

		BeginChild("AA log", { 565, 200 });
		{
			TextUnformatted(AALog.Buf.Data);

			if (GetScrollY() >= GetScrollMaxY())
				SetScrollHereY(1.0f);
		}
		EndChild();

		PopStyleColor();

		NewLine();

		Text("Current step progress: %i / %i", (sweep.sweep_rate - sweep.sweep_counter), sweep.sweep_rate);
		if (sweep.sweep_counter == 0)
		{
			SameLine();
			Text(" [waiting population: %i / %i]", field->GetNumBots(), sweep.waitForPopulation);
		}		

		if(phaseDesc)
		{
			Text("This phase: %s", phaseDesc);
		}
		else
		{
			NewLine();
		}

		NewLine();

		if (inProgress)
		{
			if (Button("Stop", { 100, 30 }))
			{
				Stop();
			}
		}
		else
		{
			if (Button("Make winds", { 100, 30 }))
			{	
				BeginWinds();

				if (visualize)
				{
					sim->RunWithMinimumRender();
				}
				else
				{
					sim->RunWithNoRender();
				}
			}

			SameLine();

			if (Button("Make divers", { 100, 30 }))
			{
				BeginDivers();

				if (visualize)
				{
					sim->RunWithMinimumRender();
				}
				else
				{
					sim->RunWithNoRender();
				}
			}

			SameLine();

			if (Button("Clear log", { 100, 30 }))
			{
				AALog.clear();
			}

			SetNextItemWidth(100);
			SameLine();
			//DragInt("iterations", &iterations, 1.0f, 1, 10);

			SameLine();
			Checkbox("Visualize", &visualize);
		}

		NewLine();

		if(history.size() > 1)
		{
			Plot();
		}
	}
	End();
}

void AutomaticAdaptation::AdaptationStep(uint frame)
{
	currentTick = frame;

	if(inProgress)
	{
		if (field->GetNumBots() == 0)
		{
			PrintInLog("Failed attempt. Starting over...");
			(*this.*Reset_func)();
		}
		else
		{
			sweep.SetTick(frame);

			if(sweep.CheckSweep(field->GetNumBots()))
				(*this.*AA_func)();
		}
	}
}

void AutomaticAdaptation::BeginDivers()
{
	Reset();

	PrintInLog("[Divers] Started...");

	inProgress = true;
	numPhases = 8u;
	AA_func = &AutomaticAdaptation::AutoAdaptDivers;
	Reset_func = &AutomaticAdaptation::BeginDivers;
	params->PSreward = 25;

	sweep.WaitPopulation(10000, 100);

	NextPhase();
}

void AutomaticAdaptation::BeginWinds()
{
	Reset();

	PrintInLog("[Winds] Started...");

	params->mudLevel = 0;
	params->oceanLevel = FieldCellsHeight;
	params->noPredators = true;

	inProgress = true;
	numPhases = 4u;
	AA_func = &AutomaticAdaptation::AutoAdaptWinds;
	Reset_func = &AutomaticAdaptation::BeginWinds;

	NextPhase();
}

void AutomaticAdaptation::Stop()
{
	inProgress = false;
}

void AutomaticAdaptation::Reset()
{		
	sweep.Clear();
	history.clear();
	sim->ClearWorld();
	params->Reset();
	field->RemoveAllObjects();
	field->SpawnControlGroup();

	phase = 0u;
	phaseBeginTick = 0u;
	adaptationBeginTick = 0u;
}

AutomaticAdaptation::AutomaticAdaptation(Field* f, Main* m)
{
	field = f;
	params = &f->params;
	sim = m;
	ParameterSweep::history = &history;
}
