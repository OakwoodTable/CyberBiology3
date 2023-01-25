#pragma once

#include "Settings.h"
#include "SDL.h"



class Chart final
{
	float chartData_bots[ChartNumValues];
	float chartData_apples[ChartNumValues];
	float chartData_organics[ChartNumValues];
	float chartData_predators[ChartNumValues];
	float chartData_avg_lifetime[ChartNumValues];

	int chart_numValues = 0;
	int chart_currentPosition = 0;

	bool chartShow_apples = false;
	bool chartShow_organics = false;
	bool chartShow_predators = false;
	bool chartShow_avg_lifetime = false;

	int timeBeforeNextDataToChart = AddToChartEvery;

public:

	bool Tick();

	void ClearChart();
	void AddToChart(float, float, float, float, float);

	void Plot();
};