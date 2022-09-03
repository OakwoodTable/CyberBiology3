#pragma once

#include "BotNeuralNet.h"



struct NeuralNetRenderer
{
	NeuralNetRenderer() {};

	void DrawThickLine(int, int, int, int);

	void Render(BotNeuralNet* brain);

	bool MouseClick(Point);

	Neuron* selectedNeuron = NULL;

private:
	Point MouseXY;

	Rect bg_rect = { 100 - 20, 670 - 20, Render_LayerDistance * (NumNeuronLayers - 1) + Render_NeuronSize + 40, Render_VerticalDistance * (NeuronsInLayer - 1) + Render_NeuronSize + 40 };

};