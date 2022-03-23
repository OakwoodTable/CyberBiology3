#include "NeuralNetRenderer.h"




NeuralNetRenderer::NeuralNetRenderer()
{
	Font1 = TTF_OpenFont("../ImGUI/misc/fonts/ProggyClean.tff", 24);
}


NeuralNetRenderer::~NeuralNetRenderer()
{
	//SDL_FreeSurface(surfaceMessage);
	//SDL_DestroyTexture(Message);
}

bool NeuralNetRenderer::MouseClick(Point p)
{
	MouseXY = p;

	return bg_rect.IsInBounds(p);
}

void NeuralNetRenderer::Render(BotNeuralNet* brain)
{ 
	//Draw background
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &bg_rect);

	//Draw connections
	Neuron* n;
	NeuronConnection* c;
	Point drawTo = { bg_rect.x + 20, bg_rect.y + 20};

	for (uint xi = 0; xi < NumNeuronLayers; ++xi)
	{
		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
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

				SDL_RenderDrawLine(renderer, drawTo.x + xi * Render_LayerDistance + (Render_NeuronSize/2), drawTo.y + yi * Render_VerticalDistance + (Render_NeuronSize/2), drawTo.x + xi * Render_LayerDistance + Render_LayerDistance + (Render_NeuronSize/2), drawTo.y + c->num * Render_VerticalDistance + (Render_NeuronSize/2));
			}
		}
	}

	//Draw neurons	
	Rect rect = {0,0,Render_NeuronSize,Render_NeuronSize };

	for (uint xi = 0; xi < NumNeuronLayers; ++xi)
	{
		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
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
				SDL_SetRenderDrawColor(renderer, 216, 191, 216, 255);
				break;

			}

			//Draw
			rect.x = drawTo.x + xi*Render_LayerDistance;
			rect.y = drawTo.y + yi*Render_VerticalDistance;

			SDL_RenderFillRect(renderer, &rect);

			//if selected
			if (rect.IsInBounds(MouseXY))
			{
				//Highlight
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				SDL_RenderDrawRect(renderer, &rect);

				selectedNeuron = n;
			}

		}
	}


}