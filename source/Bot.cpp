#include "Bot.h"

void Bot::ChangeMutationMarker()
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


void Bot::RandomizeMarkers()
{
	for (uint i = 0; i < NumberOfMutationMarkers; ++i)
	{
		mutationMarkers[i] = rand();
	}
	nextMarker = 0;
}

Bot::s_Color Bot::GetRandomColor()
{
	s_Color toRet;

#ifdef PresetRandomColors
	uint i = RandomVal(sizeof(presetColors) / (3 * sizeof(Uint8)));

	toRet.rgb[0] = presetColors[i][0];
	toRet.rgb[1] = presetColors[i][1];
	toRet.rgb[2] = presetColors[i][2];
#else
	toRet.rgb[0] = RandomVal(256);
	toRet.rgb[1] = RandomVal(256);
	toRet.rgb[2] = RandomVal(256);
#endif

	return toRet;
}

void Bot::RandomizeColor()
{
	s_Color c = GetRandomColor();

	color[0] = c.rgb[0];
	color[1] = c.rgb[1];
	color[2] = c.rgb[2];
}


void Bot::TotalMutation()
{
	//RandomizeMarkers();
	repeat(3)
		ChangeMutationMarker();

	brain.MutateHarsh();

	RandomizeColor();
}



void Bot::ChangeColor(const int str)
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
void Bot::SlightlyMutate()
{
	brain.MutateSlightly();

#ifdef ChangeColorSlightly
	ChangeColor(10);
#endif
}



void Bot::Mutate()
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



void Bot::drawOutlineAndHead(SDL_Rect rect)
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



void Bot::Radiation()
{
	Mutate();
}


BrainOutput Bot::think(BrainInput input)
{
	//Stunned means the creature can not act
	if (stunned)
	{
		--stunned;
		return { 0, 0, 0, 0 };
	}

	//Clear all neuron values
	brain.Clear();

	//Input data
	{
		//Energy
		brain.allValues[0][0] = ((energy * 1.0f) / (MaxPossibleEnergyForABot * 1.0f));

		//Sight
		brain.allValues[0][1] = input.vision;

		//Is relative
		brain.allValues[0][2] = input.isRelative;

		//Rotation
		brain.allValues[0][3] = (direction * 1.0f) / 7.0f;

		//Height
		brain.allValues[0][4] = (y * 1.0f) / (FieldCellsHeight * 1.0f);
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

int Bot::selectionStep = 0;

bool Bot::SelectionWatcher()
{
	if (y >= FieldCellsHeight - OceanHeight)
	{
		if (selection_lastX < x)
		{
			++selection_numRightSteps;	
			
		}

		selection_lastX = x;

		if (selection_numTicks++ == 2)
		{
			if (selection_numRightSteps == 0)
			{
				if(RandomPercent(selectionStep))
					return true;
			}

			selection_numRightSteps = 0;
			selection_numTicks = 0;
		}
	}

	return false;
}


int Bot::tick()
{
	if (Object::tick() == 2)
		return 2;

	energy -= EveryTickEnergyPenalty;

	if (SelectionWatcher())
		energy = 0;

	if(((energy) <= 0) || (lifetime >= MaxBotLifetime))
		return 1;

	return 0;
}


void Bot::draw()
{
	SDL_Rect rect = { FieldX + x * FieldCellSize, FieldY + y * FieldCellSize, FieldCellSize, FieldCellSize };

	//Draw body
	SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255);
	SDL_RenderFillRect(renderer, &rect);

	//Draw outlines
	drawOutlineAndHead(rect);
}


void Bot::drawEnergy()
{
	SDL_Rect rect = { FieldX + x * FieldCellSize, FieldY + y * FieldCellSize, FieldCellSize, FieldCellSize };

	//Draw body
	SDL_SetRenderDrawColor(renderer, 255, (1.0f - ((energy * 1.0f) / (MaxPossibleEnergyForABot * 1.0f))) * 255.0f, 0, 255);

	SDL_RenderFillRect(renderer, &rect);

	//Draw outlines
	drawOutlineAndHead(rect);
}


void Bot::drawPredators()
{
	SDL_Rect rect = { FieldX + x * FieldCellSize, FieldY + y * FieldCellSize, FieldCellSize, FieldCellSize };

	//Draw body
	int energySumm = energyFromKills + energyFromPS;

	if (energyFromMinerals > 0)
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	else if (energySumm < 20)
		SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
	else
		SDL_SetRenderDrawColor(renderer, 255.0f * ((energyFromKills * 1.0f) / (energySumm * 1.0f)), 255.0f * ((energyFromPS * 1.0f) / (energySumm * 1.0f)), 0, 255);

	SDL_RenderFillRect(renderer, &rect);

	//Draw outlines
	drawOutlineAndHead(rect);
}


void Bot::Rotate(int dir)
{
	if (dir > 0)
		++direction;
	else
		--direction;

	direction = direction % 8;
}


void Bot::GiveEnergy(int num, EnergySource src)
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


int Bot::GetEnergy()
{
	return energy;
}

int Bot::GetEnergyFromPS()
{
	return energyFromPS;
}

int Bot::GetEnergyFromKills()
{
	return energyFromKills;
}


int* Bot::GetMarkers()
{
	return mutationMarkers;
}


Uint8* Bot::GetColor()
{
	return color;
}


bool Bot::TakeEnergy(int val)
{
	energy -= val;

	return energy <= 0;
}


Point Bot::GetDirection()
{
	return Rotations[direction];
}

/*Get neuron summary(info)
Format: (all integers)
-simple neurons
-radial basis neurons
-random neurons
-memory neurons (if any)
-total connections
-dead end neurons
-total neurons
*/
Bot::summary_return Bot::GetNeuronSummary()
{
	int toRet[6] = { 0,0,0,0,0,0 };
	Neuron* n;

	for (uint xi = 1; xi < NumNeuronLayers; ++xi)
	{
		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
		{
			n = &brain.allNeurons[xi][yi];

			switch (n->type)
			{
			case basic:
				toRet[0]++;
				break;
			case radialbasis:
				toRet[1]++;
				break;
			case random:
				toRet[2]++;
				break;
			case memory:
				toRet[3]++;
				break;
			}

			toRet[4] += n->numConnections;

			if (n->numConnections == 0)
				toRet[5]++;
		}
	}

	return { toRet[0], toRet[1], toRet[2], toRet[3], toRet[4], toRet[5], NumNeuronLayers * NeuronsInLayer };
}


int Bot::FindKinship(Bot* stranger)
{
	int numMarkers = 0;

	for (uint i = 0; i < NumberOfMutationMarkers; ++i)
	{
		if (mutationMarkers[i] == stranger->mutationMarkers[i])
			++numMarkers;
	}

#ifdef OneMarkerDifferenceCantBeTold
	if (numMarkers == NumberOfMutationMarkers - 1)
		numMarkers = NumberOfMutationMarkers;
#endif

	return numMarkers;
}

void Bot::Repaint(Uint8 newColor[3])
{
	memcpy(color, newColor, sizeof(color));
}



Bot::Bot(int X, int Y, uint Energy, Bot* prototype, bool mutate) :Object(X, Y), brain(&prototype->brain)
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

	selection_lastX = X;
}


Bot::Bot(int X, int Y, uint Energy) :Object(X, Y)
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

	selection_lastX = X;
}




void Bot::GiveInitialEnergyAndMarkers()
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



Neuron* Bot::GetNeuralNet()
{
	return (Neuron*)brain.allNeurons;
}

BotNeuralNet* Bot::GetBrain()
{
	return &brain;
}