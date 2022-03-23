#pragma once

#include "BotNeuralNet.h"



struct NeuralNetRenderer
{
	NeuralNetRenderer();
	~NeuralNetRenderer();

	void Render(BotNeuralNet* brain);

	bool MouseClick(Point);

	Neuron* selectedNeuron = NULL;

private:
	Point MouseXY;
	
	TTF_Font* Font1;
	SDL_Color White = { 255, 255, 255 };
	SDL_Rect TextRect;

	Rect bg_rect = { 100 - 20, 670 - 20, Render_LayerDistance * (NumNeuronLayers - 1) + Render_NeuronSize + 40, Render_VerticalDistance * (NeuronsInLayer - 1) + Render_NeuronSize + 40 };

	SDL_Surface* surfaceText;
	SDL_Texture* Text;

};