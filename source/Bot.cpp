
#include "Bot.h"

SDL_Texture* Bot::sprite_head[8];
SDL_Texture* Bot::sprite_body;


void Bot::CreateImage()
{
	SDL_Surface* surf;
	SDL_Texture* tmpTexTarget;
	SDL_Texture* tmpTex2;

	//Create rotation sprites (outline and head positions)
	repeat(8)
	{
		//Target texture
		tmpTexTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, FieldCellSize, FieldCellSize);
		
		//Set render target
		SDL_SetRenderTarget(renderer, tmpTexTarget);

		//Clear texture
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		//Render outline
	#ifdef DrawBotOutline
			SDL_SetRenderDrawColor(renderer, BotOutlineColor);
			SDL_RenderDrawRect(renderer, &image_rect);
	#endif

		//Render head
	#ifdef DrawBotHead
			SDL_RenderDrawLine(renderer, 
				FieldCellSizeHalf,
				FieldCellSizeHalf,
				FieldCellSizeHalf + Rotations[i].x * FieldCellSizeHalf,
				FieldCellSizeHalf + Rotations[i].y * FieldCellSizeHalf);
	#endif

		//Create surface
		surf = SDL_CreateRGBSurface(0, FieldCellSize, FieldCellSize, 32,
			0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

		//Copy previously rendered texture on surface
		SDL_LockSurface(surf);

		int32_t pixels[FieldCellSize * FieldCellSize];
		
		SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, pixels, surf->pitch);

		SDL_memcpy(surf->pixels, pixels, surf->w * surf->h * 4);

		SDL_UnlockSurface(surf);

		//Create static texture
		tmpTex2 = SDL_CreateTextureFromSurface(renderer, surf);

		//Save texture
		sprite_head[i] = tmpTex2;

		//Free target texture and surface
		SDL_DestroyTexture(tmpTexTarget);
		SDL_FreeSurface(surf);
	}

	//Create bot 'body' image (white rectangle)
	
	//Another surface
	surf = SDL_CreateRGBSurface(0, FieldCellSize, FieldCellSize, 32,
		0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	//Clear surface
	SDL_LockSurface(surf);
	SDL_memset(surf->pixels, 255, surf->w* surf->h * 4);
	SDL_UnlockSurface(surf);

	//Create static texture
	sprite_body = SDL_CreateTextureFromSurface(renderer, surf);

	//Free surface
	SDL_FreeSurface(surf);
	
	//Free render target
	SDL_SetRenderTarget(renderer, NULL);
}

void Bot::DeleteImage()
{
	repeat(8)
		SDL_DestroyTexture(sprite_head[i]);

	SDL_DestroyTexture(sprite_body);
}

void Bot::CalculateLookAt()
{
	lookAt = Rotations[direction];

	lookAt.Shift(x, y);

	lookAt.x = pField->ValidateX(lookAt.x);

	lookAt_x = lookAt.x,
	lookAt_y = lookAt.y;
}


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

Color Bot::GetRandomColor()
{
	Color toRet;

#ifdef PresetRandomColors
	uint i = RandomVal(sizeof(presetColors) / (3 * sizeof(Uint8)));

	toRet.r = presetColors[i][0];
	toRet.g = presetColors[i][1];
	toRet.b = presetColors[i][2];
#else
	toRet.SetRandom();
#endif

	return toRet;
}

void Bot::RandomizeColor()
{
	color = GetRandomColor();
}

void Bot::RandomDirection()
{
	direction = RandomVal(8);
}


void Bot::TotalMutation()
{
	RandomizeMarkers();

	repeat(3)
		ChangeMutationMarker();

	activeBrain.MutateHarsh();

	RandomizeColor();
}


void Bot::ChangeColor(const int str)
{
	color.RandomChange(str);
}


void Bot::SlightlyMutate()
{
	activeBrain.MutateSlightly();

	#ifdef ChangeColorSlightly
	ChangeColor(BotColorChangeStrength);
	#endif
}



void Bot::Mutate()
{
	ChangeMutationMarker();

	//Mutate brain
	for (int i = 0; i < (1 + RandomVal(MutateNeuronsMaximum + 1)); ++i)
		initialBrain.Mutate();

	/*
	for (int s = 0; s < (1 + RandomVal(MutateNeuronsSlightlyMaximum + 1)); ++s)
		brain.MutateSlightly();
	*/

	//Change color
	#ifdef ChangeColorSlightly
	ChangeColor(20);
	#endif

	/*if (RandomPercentX10(RandomColorChancePercentX100))
	{
		RandomizeColor();
	}*/

}



void Bot::drawOutlineAndHead()
{
	SDL_RenderCopy(renderer, sprite_head[direction], &image_rect, &object_rect);
}

BrainInput Bot::FillBrainInput()
{
	BrainInput input;

input.energy=(float)energy/(float)MaxPossibleEnergyForABot;

	//If destination is out of bounds
	if (!pField->IsInBounds(lookAt_x, lookAt_y))
	{
		//1 if unpassable
		input.vision = 1.0f;
	}
	else
	{
		Object* tmpDest = (*pCells)[lookAt_x][lookAt_y];

		//Destination cell is empty
		if (!tmpDest)
		{
			//0 if empty
			input.vision = 0.0f;
		}
		else
		{
			//Destination not empty
			switch (tmpDest->type)
			{
			case bot:
				//0.5 if someone is in that cell
				input.vision = 1.0f;

				//Calculate how close they are as relatives, based on mutation markers
				input.vision += (1.0f - (FindKinship((Bot*)tmpDest) * 1.0f) / (NumberOfMutationMarkers * 1.0f));
				break;

			case rock:
				//0.5 if cell is unpassable
				input.vision = .5f;
				break;

			case organic_waste:
				//-.5 if cell contains organics
				input.vision = -.5f;
				break;

			case apple:
				//-1.0 if cell contains an apple
				input.vision = -1.0f;
				break;
			}
		}
	}

	input.age = (lifetime * 1.0f) / (pField->params.BotMaxLifePeriod * 1.0f);

	//input.rotation = (tmpOut.desired_rotation == (direction * .1f))?1.0f:0.0f;
	input.rotation = (direction * 1.0f) / 7.0f;

	return input;
}


void Bot::Multiply(int numChildren)
{
	if (ArtificialSelectionWatcher_OnDivide())
		return;

	for (int b = 0; b < numChildren; ++b)
	{
		if (energy <= 1 + GiveBirthCost)
			return;
		else
		{
			Point freeSpace;

			freeSpace = pField->FindFreeNeighbourCell(x, y);

			if (freeSpace.x != -1)
			{
			#ifndef NewbornGetsHalf
				TakeEnergy(EnergyPassedToAChild + GiveBirthCost);

				if ((!RandomPercentX10(pField->params.adaptation_botShouldBeOnLandOnceToMultiply)) || (wasOnLand))
					pField->AddObject(new Bot(freeSpace.x, freeSpace.y, EnergyPassedToAChild, this, RandomPercent(MutationChancePercent)));
			#else
				TakeEnergy(energy / 2 + GiveBirthCost);

				if ((!RandomPercentX10(pField->params.adaptation_botShouldBeOnLandOnceToMultiply)) || (wasOnLand))
					pField->AddObject(new Bot(freeSpace.x, freeSpace.y, energy, this, RandomPercent(MutationChancePercent)));
			#endif

				return;
			}
		}
	}
}

void Bot::Attack()
{
	if (pField->IsInBounds(lookAt_x, lookAt_y))
	{
		//If there is an object
		Object* obj = (*pCells)[lookAt_x][lookAt_y];

		if (obj)
		{
			if (obj->type == bot)
			{
				#ifdef BotCanEatBot
				//Eat a bot
				GiveEnergy(obj->energy, predation);
				pField->RemoveBot(lookAt_x, lookAt_y);
				#endif

				++numAttacks;
			}
			else if (obj->type == organic_waste)
			{
				//Eat organics
				GiveEnergy(obj->energy, organics);
				pField->RemoveObject(lookAt_x, lookAt_y);
			}
			else if (obj->type == apple)
			{
				//Eat apple
				GiveEnergy(obj->energy, organics);
				pField->RemoveObject(lookAt_x, lookAt_y);
			}
			#ifdef BotCanEatRock
			else if (obj->type == rock)
			{
				//Eat rock, it gives no energy
				RemoveObject(lookAt_x, lookAt_y);
			}
			#endif
		}
	}
}

void Bot::Photosynthesis()
{
	//Above water
	if (!pField->IsInWater(y))
	{
		int toGive;

		//Give energy depending on a season
		switch (pField->season)
		{
		case summer:
		#ifdef UseSeasons
			toGive = pField->params.SeasonReward_Summer; // PhotosynthesisReward_Summer;
		#else
			toGive = pField->photosynthesisReward;
			//toGive = FindHowManyFreeCellsAround(x, y) - 3;
			//if (toGive < 0) toGive = 0;
		#endif
			break;
		case autumn: case spring:
			toGive = pField->params.SeasonReward_Autumn; // PhotosynthesisReward_Autumn;
			break;
		case winter:
			toGive = pField->params.SeasonReward_Winter; // PhotosynthesisReward_Winter;
			//toGive = (ticknum % 5 == 0) ? 2 : 1;
			break;
		}

		GiveEnergy(toGive, PS);

		++numPSonLand;
	}
	//Below water
	else
	{
		#ifndef NoPhotosyntesisInOcean
		
		if (pField->IsInMud(y))
		{
			if (RandomPercentX10(pField->params.adaptation_PSInMudBlock))
				return;
		}
		else
		{
			if (RandomPercentX10(pField->params.adaptation_PSInOceanBlock))
				return;
		}

		GiveEnergy(pField->photosynthesisReward, PS);

		#endif
	}
}

void Bot::Mutagen()
{
	if (RandomPercent(10))
		Mutate();
}


BrainOutput Bot::think(BrainInput input)
{
	//Stunned means the creature can not act
	if (stunned)
	{
		--stunned;

		return BrainOutput::GetEmptyBrain();
	}

	//Clear all neuron values
	activeBrain.Clear();

	//Input data
	{
		//Energy
		activeBrain.allValues[NeuronInputLayerIndex][0] = ((energy * 1.0f) / (MaxPossibleEnergyForABot * 1.0f));

		//Sight
		activeBrain.allValues[NeuronInputLayerIndex][1] = input.vision;

		//Bot age
		activeBrain.allValues[NeuronInputLayerIndex][2] = input.age;

		//Rotation
		activeBrain.allValues[NeuronInputLayerIndex][3] = input.rotation;

		//Height
		activeBrain.allValues[NeuronInputLayerIndex][4] = (y * 1.0f) / (FieldCellsHeight * 1.0f);
	}

	//Compute
	activeBrain.Process();

	BrainOutput toRet = activeBrain.GetOutput();

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

	return toRet;
}



bool Bot::ArtificialSelectionWatcher_OnTick()
{
	//Winds
	if (pField->IsInWater(y))
	{
		if (addaptation_lastX < x)
			++adaptation_numRightSteps;

		addaptation_lastX = x;

		if (adaptationCounter++ == pField->params.adaptation_StepsNum_Winds)
		{
			if (adaptation_numRightSteps == 0)
			{
				if (RandomPercentX10(pField->params.adaptation_DeathChance_Winds))
					return true;
			}

			adaptation_numRightSteps = 0;
			adaptationCounter = 0;
		}
	}

	//Divers
	FieldDynamicParams& params = pField->params;	

	//Force movements Y
	if (lifetime > 3)
	{
		if (numMovesY * 3 < (lifetime - 3))
		{
			if (RandomPercentX10(params.adaptation_forceBotMovements))
				return true;
		}
	}

	return false;
}


bool Bot::ArtificialSelectionWatcher_OnDivide()
{
	FieldDynamicParams& params = pField->params;

	//Is on land
	if (y < FieldCellsHeight - params.oceanLevel)
	{
		if (RandomPercentX10(params.adaptation_landBirthBlock))
			return true;
	}
	//Is in ocean
	else if ((y >= FieldCellsHeight - params.oceanLevel) && (y < FieldCellsHeight - params.mudLevel))
	{
		if (RandomPercentX10(params.adaptation_seaBirthBlock))
			return true;
	}
	 
	//Force photosynthesis on land
	if (numPSonLand < 4)
	{
		if (RandomPercentX10(params.adaptation_botShouldDoPSOnLandOnceToMultiply))
			return true;
	}

	return false;
}



int Bot::tick()
{
	int ret = Object::tick();

	if (ret != 0)
		return ret;

	energy -= EveryTickEnergyPenalty;
#ifdef ArtificialSelection
	if (ArtificialSelectionWatcher_OnTick())
		return 1;
#endif
	if (((energy) <= 0) || (lifetime >= pField->params.BotMaxLifePeriod))
		return 1;

	BrainOutput tmpOut;
	
	CalculateLookAt();

	if (!pField->IsInWater(y))
		wasOnLand = true;

	//Fill brain input structure
	BrainInput input = FillBrainInput();

	//Bot brain does its stuff
	tmpOut = think(input);

	//Multiply first
	if (tmpOut.divide > 0)
	{
		Multiply(tmpOut.divide);

		if (energy <= 0)
			return 1;
	}

	//Then attack
	if (tmpOut.attack > 0)
	{
		//If dies of low energy
		if (TakeEnergy(AttackCost))
			return 1;
		else
		{
			Attack();
		}
	}
	else
	{
		//Rotate after
		if (tmpOut.desired_rotation != (direction * .1f))
		{
			//If dies of low energy
			if (TakeEnergy(RotateCost))
				return 1;

 			Rotate(int(tmpOut.desired_rotation * 10.0f));
		}

		//Move
		if (tmpOut.move > 0)
		{
			if (TakeEnergy(MoveCost))
				return 1;

			//Place object in a new place
			int tmpY = y;

			if (pField->MoveObject(this, lookAt_x, lookAt_y) == 0)
			{
				if(lookAt_y!=tmpY)
					++numMovesY;
			}

		}
		//Photosynthesis
		else if (tmpOut.photosynthesis > 0)
		{
			#ifdef NoPhotosynthesis
			return 0;
			#endif

			Photosynthesis();
		}
	}

	return 0;
}


void Bot::draw()
{
	CalcScreenX();
	CalcObjectRect();

	//Draw body
	SDL_SetTextureColorMod(sprite_body, color.r, color.g, color.b);
	SDL_RenderCopy(renderer, sprite_body, &image_rect, &object_rect);

	//Draw outlines
	drawOutlineAndHead();
}


void Bot::drawEnergy()
{
	CalcScreenX();
	CalcObjectRect();

	//Draw body
	SDL_SetTextureColorMod(sprite_body, 255, (1.0f - ((energy * 1.0f) / (MaxPossibleEnergyForABot * 1.0f))) * 255.0f, 0);
	SDL_RenderCopy(renderer, sprite_body, &image_rect, &object_rect);

	//Draw outlines
	drawOutlineAndHead();
}


void Bot::drawPredators()
{
	CalcScreenX();
	CalcObjectRect();

	//Draw body
	int energySumm = energyFromPredation + energyFromPS + energyFromOrganics;

	if (energySumm < 20)
		SDL_SetTextureColorMod(sprite_body, 180, 180, 180);
	else
		SDL_SetTextureColorMod(sprite_body, 255.0f * ((energyFromPredation * 1.0f) / (energySumm * 1.0f)),
			255.0f * ((energyFromPS * 1.0f) / (energySumm * 1.0f)), 255.0f * ((energyFromOrganics * 1.0f) / (energySumm * 1.0f)));

	SDL_RenderCopy(renderer, sprite_body, &image_rect, &object_rect);

	//Draw outlines
	drawOutlineAndHead();
}


void Bot::Rotate(int dir)
{
	dir = dir % 8;

	if (dir < 0)
		dir = 8 + dir;

	int delta = (direction - dir);	

	if (delta < 0)
	{
		if (delta > 4)
			--direction;
		else
			++direction;
	}
	else if (delta > 0)
	{
		if (delta > 4)
			++direction;
		else
			--direction;
	}
		
	direction = direction % 8;
}


void Bot::GiveEnergy(int num, EnergySource src)
{
	energy += num;

	if (energy > MaxPossibleEnergyForABot)
	{
	#ifdef BotDiesIfEnergyOverflow
		energy = 0;
		return;
	#else
		energy = MaxPossibleEnergyForABot;
	#endif
	}

	if (src == PS)
	{
		energyFromPS += num;
	}
	else if (src == predation)
	{
		energyFromPredation += num;
	}
	else if (src == organics)
	{
		energyFromOrganics += num;
	}
}

int Bot::GetEnergyFromPS()
{
	return energyFromPS;
}

int Bot::GetEnergyFromKills()
{
	return energyFromPredation;
}


int* Bot::GetMarkers()
{
	return mutationMarkers;
}


Color* Bot::GetColor()
{
	return &color;
}


bool Bot::TakeEnergy(int val)
{
	energy -= val;

	return energy <= 0;
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
			n = &activeBrain.allNeurons[xi][yi];

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

	if (numMarkers >= (NumberOfMutationMarkers - HowMuchDifferenseCantBeTold))
		numMarkers = NumberOfMutationMarkers;

	return numMarkers;
}

void Bot::SetColor(Color newColor)
{
	color = newColor;
}

void Bot::SetColor(Uint8 r, Uint8 g, Uint8 b)
{
	color.r = r;
	color.g = g;
	color.b = b;
}


Bot::Bot(int X, int Y, uint Energy, Bot* prototype, bool mutate) :Object(X, Y), initialBrain(&prototype->initialBrain)
{
	type = bot;

	energy = Energy;
	stunned = StunAfterBirth;
	fertilityDelay = FertilityDelay;
	energyFromPS = 0;
	energyFromPredation = 0;

	//Copy parent's markers and color
	memcpy(mutationMarkers, prototype->mutationMarkers, sizeof(mutationMarkers));
	nextMarker = prototype->nextMarker;

	color = prototype->color;

	//Random direction
	RandomDirection();

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

	//Create active brain
	activeBrain.Clone(&initialBrain);
	activeBrain.Optimize();

	addaptation_lastX = X;
}


Bot::Bot(int X, int Y, uint Energy) :Object(X, Y)
{
	type = bot;

	RandomizeMarkers();

	energy = Energy;
	stunned = StunAfterBirth;
	fertilityDelay = FertilityDelay;
	energyFromPS = 0;
	energyFromPredation = 0;

	//Randomize bot brain
	initialBrain.Randomize();
	activeBrain.Clone(&initialBrain);
	activeBrain.Optimize();

	//Set brain to dummy brain
	//brain.SetDummy();

	//Random color
	RandomizeColor();

	//Random direction
	RandomDirection();

	addaptation_lastX = X;

	//Temporary
	numMovesY = 1000;
}



BotNeuralNet* Bot::GetActiveBrain()
{
	return &activeBrain;
}

BotNeuralNet* Bot::GetInitialBrain()
{
	return &initialBrain;
}