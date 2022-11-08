#pragma once


#include "BotNeuralNet.h"
#include "SDL.h"



struct NeuralNetRenderer final
{
	NeuralNetRenderer() {};

	void DrawThickLine(int, int, int, int);

	void Render(BotNeuralNet* brain);

	bool MouseClick(Point);


	Neuron* selectedNeuron = NULL;
	BotNeuralNet* selectedBrain = NULL;

private:

	Point MouseXY;

	Rect bg_rect = { 80, 350, Render_LayerDistance * (NumNeuronLayers - 1) + Render_NeuronSize + 40,
		Render_VerticalDistance * (NeuronsInLayer - 1) + Render_NeuronSize + 40 };

};