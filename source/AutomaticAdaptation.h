#pragma once

class AutomaticAdaptation;
class Field;
class Main;

#include "Main.h"


struct ParameterSweep
{
private:

	uint currentTick;
	uint beginTick;

	int* toSweep = NULL;

	int sweepIncrement;
	int sweepGoal;		

	uint addToChartRate;
	uint chartValPrev;

	void AddToChart();

public:	

	uint sweep_counter;
	uint sweep_rate;
	uint waitForPopulation;

	void SetTick(uint tick);
	void WaitPopulation(int pop, int count);
	void BeginSweep(int* p, int begin, int inc, int goal, int pop, int count, int addToChartEvery = 0);
	bool CheckSweep(uint numBots);
	void Clear();


	vector<uint>* history;
};


class AutomaticAdaptation final
{
private:

	//const bool makeAutosave = false;

	uint currentTick;	

	bool inProgress = false;

	uint adaptationBeginTick;
	uint phaseBeginTick;
	uint phase;
	uint numPhases;

	uint iteration;
	uint numIterations;
	
	const char* phaseDesc = NULL;

	vector<vector<uint>> history;
	vector<uint> avg_history;

	void CalculateHistoryAverage();

	void NextPhase(const char* desc = NULL);
	void NewEpoch();
	void Finish();

	ParameterSweep sweep;		

	void AutoAdaptDivers();
	void AutoAdaptWinds();
	void (AutomaticAdaptation::*AA_func)();
	void (AutomaticAdaptation::*Reset_func)();

	ImGuiTextBuffer AALog;

	void PrintInLog(string s);
	void Plot();

	void BeginDivers();
	void BeginWinds();
	void Stop();
	void Reset();

	Field* field;
	FieldDynamicParams* params;
	Main* sim;


public:

	void DrawWindow();
	void AdaptationStep(uint frame);

	AutomaticAdaptation(Field*, Main*);
	
};