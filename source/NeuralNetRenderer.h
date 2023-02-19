#pragma once

#include "BotNeuralNet.h"
#include "SDL.h"


struct NeuralNetRenderer final
{
	void DrawThickLine(int, int, int, int);
	void Render(BotNeuralNet* brain);
	bool MouseClick(Point);


	BotNeuralNet* selectedBrain = NULL;
	Neuron* selectedNeuron = NULL;
	const char* selectedNeuronCaption = "";
	float selectedNeuronValue = 0.0f;
	int8_t selectedNeuronMemory = 0;


private:

	Point MouseXY;

	Rect bg_rect = 
	{ 
		80, 
		350, 
		Render_LayerDistance * (NumNeuronLayers - 1) + Render_NeuronSize + 40,
		Render_VerticalDistance * (NumNeuronsInLayerMax - 1) + Render_NeuronSize + 40 
	};

};