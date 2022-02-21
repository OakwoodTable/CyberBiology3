#pragma once

#include "Neurons.h"
#include "../Object.h"


//Rotations array, contains where a bot would look with every
//position of its head
const Point Rotations[] =
{
	{0,-1},
	{1,-1},
	{1,0},
	{1,1},
	{0,1},
	{-1,1},
	{-1,0},
	{-1,-1}
};

enum EnergySource
{
	PS,
	kills,
	mineral,
	unknown
};


//Preselected colors for bots
const Uint8 presetColors[][4] =
{
	{255, 0, 0},
	{0, 255, 0},
	{0, 0, 255},
	{223, 52, 210},
	{200, 14, 84},
	{60, 60, 163},
	{160, 160, 200},
	{0, 255, 255},
	{100, 170, 170},
	{80, 90, 90},
	{70, 200, 160},
	{0, 130, 86},
	{0, 133, 0},
	{140, 255, 0},
	{136, 160, 103},
	{200, 255, 0},
	{160, 180, 100},
	{255, 255, 0},
	{255, 190, 0},
	{170, 150, 85},
	{255, 120, 0},
	{240, 200, 200}
};



//Bot class
class Bot:public Object
{
	//Rotation, see Rotations[]
	uint direction=0;

	BotNeuralNet brain;

	//Bot energy, if this is 0 bot dies
	int energy;

	//if this is not 0, bot does nothing at his turn
	int stunned;

	//How long a bot should wait to give birth after his own birth 
	int fertilityDelay;

	Uint8 color[3] = {0, 111, 0};

	//Energy acquired from different sources
	int energyFromPS = 0;
	int energyFromKills = 0;
	int energyFromMinerals = 0;


	//Mutation markers used to decide how close two bots are to each other as relatives
	int mutationMarkers[NumberOfMutationMarkers];
	uint nextMarker;

	void ChangeMutationMarker()
	{
		mutationMarkers[nextMarker++] = rand();

		if (nextMarker >= NumberOfMutationMarkers)
		{
			nextMarker = 0;

			#ifdef RandomColorEveryNewMarkersSet
				RandomizeColor();
			#endif
		}
	}

	//Set random mutation markers
	void RandomizeMarkers()
	{
		for (uint i = 0; i < NumberOfMutationMarkers; ++i)
		{
			mutationMarkers[i] = rand();
		}
		nextMarker = 0;
	}

	//Set random color
	void RandomizeColor()
	{
		#ifdef PresetRandomColors
			uint i = RandomVal(sizeof(presetColors)/(3 * sizeof(Uint8)));

			color[0] = presetColors[i][0];
			color[1] = presetColors[i][1];
			color[2] = presetColors[i][2];
		#else
			color[0] = RandomVal(256);
			color[1] = RandomVal(256);
			color[2] = RandomVal(256);
		#endif
	}

	//Experimental
	//Total mutation function - rarely used
	void TotalMutation()
	{
		//RandomizeMarkers();
		repeat(3)
			ChangeMutationMarker();

		brain.MutateHarsh();

		RandomizeColor();
	}


	//Shift color a little (-10 to +10)
	void ChangeColor(const int str = 10)
	{
		uint i = RandomVal(3);

		int changeColor = (RandomVal((2 * str) + 1) - str);
		color[i] += changeColor;

		if (color[i] < 0)
			color[i] = 0;
		else if (color[i] > 255)
			color[i] = 255;
	}


	//Experimental
	void SlightlyMutate()
	{
		brain.MutateSlightly();

	#ifdef ChangeColorSlightly
		ChangeColor(10);
	#endif
	}

	//Main mutate function
	void Mutate()
	{
		//Change next mutation marker
		ChangeMutationMarker();

		//Mutate brain
		for (int i = 0; i < (1 + RandomVal(MutateNeuronsMaximum + 1)); ++i)
			brain.Mutate();

		for (int s = 0; s < (1 + RandomVal(MutateNeuronsSlightlyMaximum + 1)); ++s)
			brain.MutateSlightly();

		//Change color
		#ifdef ChangeColorSlightly
			ChangeColor(20);
		#endif

		/*if (RandomPercentX10(RandomColorChancePercentX100))
		{
			RandomizeColor();
		}*/

	}

	//Draw bot outline and his head
	void drawOutlineAndHead(SDL_Rect rect)
	{
		//Draw outline
		#ifdef DrawBotOutline
			SDL_SetRenderDrawColor(renderer, BotOutlineColor);
			SDL_RenderDrawRect(renderer, &rect);
		#endif

		//Draw direction marker
		#ifdef DrawBotHead
			SDL_RenderDrawLine(renderer, FieldX + x * FieldCellSize + FieldCellSizeHalf, FieldY + y * FieldCellSize + FieldCellSizeHalf, FieldX + x * FieldCellSize + FieldCellSizeHalf + Rotations[direction].x * FieldCellSizeHalf, FieldY + y * FieldCellSize + FieldCellSizeHalf + Rotations[direction].y * FieldCellSizeHalf);
		#endif
	}

public:

	//Experimental
	//This function is used to simulate mutagen
	void Radiation()
	{
		Mutate();
	}

	//Use neural network
	BrainOutput think(BrainInput input)
	{
		//Stunned means the creature can not act
		if (stunned)
		{
			--stunned;
			return {0, 0, 0, 0};
		}

		//Clear all neuron values
		brain.Clear();

		//Input data
		{
			//Energy
			brain.allValues[0][0] = ((energy * 1.0f)/(MaxPossibleEnergyForABot * 1.0f));

			//Sight
			brain.allValues[0][1] = input.vision;

			//Is relative
			brain.allValues[0][2] = input.isRelative;

			//Rotation
			brain.allValues[0][3] = (direction*1.0f) / 7.0f;

			//Height
			brain.allValues[0][4] = (y * 1.0f) / (FieldCellsHeight * 0.5f);
		}

		//Compute
		brain.Process();

		BrainOutput toRet = brain.GetOutput();

		//Cannot multipy if not ready
		if (fertilityDelay)
		{
			--fertilityDelay;
			toRet.divide = 0;
		}
		else if (toRet.divide)
		{
			fertilityDelay = FertilityDelay;
		}

		//if(toRet.attack == 1)
			//return toRet;

		return toRet;
	}

	//Bot tick function, it should always call parents tick function first
	bool tick() override
	{
		Object::tick();

		energy -= EveryTickEnergyPenalty;

		return ((energy)<=0) || (lifetime >= MaxBotLifetime);
	}

	//Bot main draw function
	void draw() override
	{
		SDL_Rect rect = { FieldX + x * FieldCellSize, FieldY + y * FieldCellSize, FieldCellSize, FieldCellSize };

		//Draw body
		SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255);
		SDL_RenderFillRect(renderer, &rect); 

		//Draw outlines
		drawOutlineAndHead(rect);
	}

	//Bot draw function is energy mode
	void drawEnergy()
	{
		SDL_Rect rect = { FieldX + x * FieldCellSize, FieldY + y * FieldCellSize, FieldCellSize, FieldCellSize };

		//Draw body
		SDL_SetRenderDrawColor(renderer, 255, (1.0f - ((energy * 1.0f) / (MaxPossibleEnergyForABot * 1.0f))) * 255.0f, 0, 255);

		SDL_RenderFillRect(renderer, &rect);

		//Draw outlines
		drawOutlineAndHead(rect);
	}

	//Bot draw function is predators mode
	void drawPredators()
	{
		SDL_Rect rect = { FieldX + x * FieldCellSize, FieldY + y * FieldCellSize, FieldCellSize, FieldCellSize };

		//Draw body
		int energySumm = energyFromKills + energyFromPS;

		if (energyFromMinerals > 0)
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		else if(energySumm<20)
			SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
		else
			SDL_SetRenderDrawColor(renderer, 255.0f * ((energyFromKills * 1.0f)/(energySumm*1.0f)), 255.0f * ((energyFromPS * 1.0f) / (energySumm * 1.0f)), 0, 255);

		SDL_RenderFillRect(renderer, &rect);

		//Draw outlines
		drawOutlineAndHead(rect);
	}

	//Change rotation function
	void Rotate(int dir=1)
	{
		if (dir > 0)
			++direction;
		else
			--direction;

		direction = direction % 8;
	}

	//Give a bot some energy
	void GiveEnergy(int num, EnergySource src = unknown)
	{
		energy += num;

		if (energy > MaxPossibleEnergyForABot)
		{
			energy = MaxPossibleEnergyForABot;
		}

		if (src == PS)
		{
			energyFromPS += num;
		}
		else if (src == kills)
		{
			energyFromKills += num;
		}
		else if (src == mineral)
		{
			energyFromMinerals += num;
		}
	}

	//Return current energy
	int GetEnergy()
	{
		return energy;
	}

	int GetEnergyFromPS()
	{
		return energyFromPS;
	}

	int GetEnergyFromKills()
	{
		return energyFromKills;
	}

	//Returns a pointer to mutation markers array
	int* GetMarkers()
	{
		return mutationMarkers;
	}

	//Get bot color
	Uint8* GetColor()
	{
		return color;
	}

	//Take away bot energy, return true if 0 or below (bot dies)
	bool TakeEnergy(int val)
	{
		energy -= val;

		return energy <= 0;
	}

	//Get rotation
	Point GetDirection()
	{
		return Rotations[direction];
	}

	/*Find out how close these two are as relatives,
	returns number of matching mutation markers*/
	int FindKinship(Bot* stranger)
	{
		int numMarkers = 0;

		for (uint i = 0; i < NumberOfMutationMarkers; ++i)
		{
			if (mutationMarkers[i] == stranger->mutationMarkers[i])
				++numMarkers;
		}
		
		#ifdef OneMarkerDifferenceCantBeTold
			if(numMarkers == NumberOfMutationMarkers - 1)
				numMarkers = NumberOfMutationMarkers;
		#endif

		return numMarkers;
	}


	//Inherit from a parent
	Bot(int X, int Y, uint Energy, Bot* prototype, bool mutate = false) :Object(X, Y), brain(&prototype->brain)
	{
		type = bot;

		energy = Energy;
		stunned = StunAfterBirth;
		fertilityDelay = FertilityDelay;
		energyFromPS = 0;
		energyFromKills = 0;

		//Copy parent's markers and color
		memcpy(mutationMarkers, prototype->mutationMarkers, sizeof(mutationMarkers));
		nextMarker = prototype->nextMarker;

		memcpy(color, prototype->color, sizeof(color));
		
		//Now mutate
		#ifndef ForbidMutations
		if (mutate)
		{
			#ifdef UseTotalMutation
			if (RandomPercentX10(TotalMutationChancePercentX10))
				TotalMutation();
			else
			#endif
			{
				Mutate();
			}
		}
		#endif
	}

	//New bot
	Bot(int X, int Y, uint Energy = 100):Object(X, Y)
	{
		type = bot;

		//Ignore
		//brain.allNeurons[1][1].type = random;
		//brain.allNeurons[1][1].bias = 0.5f;
		//brain.allNeurons[1][1].AddConnection(2, -1.0f);

		//brain.allNeurons[2][2].bias = 1.0f;

		//brain.allNeurons[4][1].bias = 1.0f;

		RandomizeMarkers();

		energy = Energy;
		stunned = StunAfterBirth;
		fertilityDelay = FertilityDelay;
		energyFromPS = 0;
		energyFromKills = 0;

		//Randomize bot brain
		brain.Randomize();

		//Set brain to dummy brain
		//brain.SetDummy();

		//Random color
		RandomizeColor();

		//for (int m = 0; m < NeuronsInLayer*NumNeuronLayers; ++m)
			//Mutate();
	}



	//This function is used only after a bot was loaded from file!!
	void GiveInitialEnergyAndMarkers()
	{
		RandomizeMarkers();

		energy = MaxPossibleEnergyForABot;
		stunned = StunAfterBirth;
		fertilityDelay = FertilityDelay;
		energyFromPS = 0;
		energyFromKills = 0;

		direction = 0;
		RandomizeColor(); // Temporary
	}


	//Get neural net pointer
	Neuron* GetNeuralNet()
	{
		return (Neuron*)brain.allNeurons;
	}

};

