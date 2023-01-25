#include "NeuralNetRenderer.h"



bool NeuralNetRenderer::MouseClick(Point p)
{
	MouseXY = p;

	return bg_rect.IsInBounds(p);
}

void NeuralNetRenderer::DrawThickLine(int x1, int y1, int x2, int y2)
{
	repeat(Render_LineThickness)
		SDL_RenderDrawLine(renderer, x1, y1 + i - (Render_LineThickness/2), x2, y2 + i - (Render_LineThickness/2));
}

void NeuralNetRenderer::Render(BotNeuralNet* brain)
{ 
	selectedBrain = brain;

	//Draw background
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &bg_rect);

	//Draw connections
	Neuron* n;
	NeuronConnection* c;
	Point drawTo = { bg_rect.x + 20, bg_rect.y + 20};

	int shifts[NumNeuronLayers];

	repeat(NumNeuronLayers)
		shifts[i] = ((NumNeuronsInLayerMax - neuronsInLayer[i]) * Render_VerticalDistance) / 2;

	for (uint xi = 0; xi < NumNeuronLayers; ++xi)
	{
		for (uint yi = 0; yi < neuronsInLayer[xi]; ++yi)
		{
			n = &brain->allNeurons[xi][yi];

			for (uint ci = 0; ci < n->numConnections; ++ci)
			{
				c = &n->allConnections[ci];

				if (c->weight > Render_GreyThreshold)
				{
					SDL_SetRenderDrawColor(renderer, Render_PositiveWeightColor, 255);
				}
				else if (c->weight < -Render_GreyThreshold)
				{
					SDL_SetRenderDrawColor(renderer, Render_NegativeWeightColor, 255);
				}
				else
				{
					SDL_SetRenderDrawColor(renderer, Render_NeutralWeightColor, 255);
				}

				DrawThickLine(drawTo.x + xi * Render_LayerDistance + (Render_NeuronSize/2),
					drawTo.y + yi * Render_VerticalDistance + (Render_NeuronSize/2) + shifts[xi],
					drawTo.x + (c->dest_layer) * Render_LayerDistance + (Render_NeuronSize/2),
					drawTo.y + c->dest_neuron * Render_VerticalDistance + (Render_NeuronSize/2) + shifts[c->dest_layer]);
			}
		}
	}

	//Draw neurons	
	Rect rect = {0,0,Render_NeuronSize,Render_NeuronSize };

	bool select = false;

	for (uint xi = 0; xi < NumNeuronLayers; ++xi)
	{
		for (uint yi = 0; yi < neuronsInLayer[xi]; ++yi)
		{
			n = &brain->allNeurons[xi][yi];

			//Choose color
			switch (n->type)
			{
			//Yellow
			case input: case output:
				SDL_SetRenderDrawColor(renderer, 235, 235, 0, 255);
				break;

			//Grey
			case basic:
				SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
				break;

			//White
			case radialbasis:
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				break;

			//Blue
			case random:
				SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);
				break;

			//Purple
			case memory:
				SDL_SetRenderDrawColor(renderer, 155, 0, 155, 255);
				break;

			}

			//Draw
			rect.x = drawTo.x + xi*Render_LayerDistance;
			rect.y = drawTo.y + yi*Render_VerticalDistance + shifts[xi];

			SDL_RenderFillRect(renderer, &rect);

			//if selected
			if (rect.IsInBounds(MouseXY))
			{
				//Highlight
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				SDL_RenderDrawRect(renderer, &rect);

				selectedNeuron = n;
				
				if (n->type == input)
				{
					selectedNeuronCaption = inputLayerCaptions[yi];
				}
				else if (n->type == output)
				{
					selectedNeuronCaption = outputLayerCaptions[yi];
				}

				selectedNeuronValue = brain->allValues[xi][yi];
				selectedNeuronMemory = brain->allMemory[xi][yi];

				select = true;
			}
		}
	}

	if (!select)
	{
		selectedNeuronCaption = "";
		selectedNeuron = NULL;
	}
}