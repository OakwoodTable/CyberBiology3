
#include "Bot.h"

SDL_Texture* Bot::sprite_head[8] = {};
SDL_Texture* Bot::sprite_body = NULL;



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


void Bot::SetBodyImage(SDL_Texture* img)
{
	sprite_body = img;
}

void Bot::SetHeadImages(SDL_Texture* img[8])
{
	repeat(8)
		sprite_head[i] = img[i];
}

void Bot::RandomizeColor()
{
	color.SetRandom();
}

void Bot::RandomDirection()
{
	direction = RandomVal(8);
}


void Bot::ChangeColor(const int str)
{
	color.RandomChange(str);
}



void Bot::Mutate()
{
	ChangeMutationMarker();

	//Mutate brain
	constexpr uint MaxMutate = MutateNeuronsMaximum;

	for (uint i = 0; i < (1 + RandomVal(MaxMutate + 1)); ++i)
		initialBrain.Mutate();

	//Change color
	#ifdef ChangeColorSlightly
	ChangeColor(BotColorChangeStrength);
	#endif

	#ifdef UseRandomColorOccasionaly
	{
		if (RandomPercentX10(RandomColorChancePercentX10))
		{
			RandomizeColor();
		}
	}
	#endif
}



void Bot::drawOutlineAndHead()
{
	SDL_RenderCopy(renderer, sprite_head[direction], &image_rect, &object_rect);
}

BrainInput Bot::FillBrainInput()
{
	BrainInput input;

	input.energy = ((energy * 1.0f) / (MaxPossibleEnergyForABot * 1.0f));
	input.age = (lifetime * 1.0f) / (pField->params.botMaxLifetime * 1.0f);

	//input.rotation = (tmpOut.desired_rotation == (direction * .1f))?1.0f:0.0f;
	input.rotation = (direction * 1.0f) / 7.0f;
	input.height = (y * 1.0f) / (FieldCellsHeight * 1.0f);

	if(pField->IsInMud(y))
		input.area = 1.0f;
	else if(pField->IsInWater(y))
		input.area = 0.5f;
	else
		input.area = 0.0f;


	input.eye1 = FillSightNeuron(lookAt);

	//Second sight sensor
	Point p = lookAt + Rotations[direction];

	p.x = pField->ValidateX(p.x);

	input.eye2 = FillSightNeuron(p);

	return input;
}

float Bot::FillSightNeuron(Point at)
{
	//If destination is out of bounds
	if (!pField->IsInBounds(at))
	{
		//-1 if unpassable
		return -1.0f;
	}
	else
	{
		Object* tmpDest = (*pCells)[at.x][at.y];

		//Destination cell is empty
		if (tmpDest)
		{
			float toRet;

			//Destination not empty
			switch (tmpDest->type())
			{
			case bot:
				//1.5 if someone is in that cell
				toRet = 1.5f;

				//Calculate how close they are as relatives, based on mutation markers
				toRet += (1.0f - (FindKinship((Bot*)tmpDest) * 1.0f) / (NumberOfMutationMarkers * 1.0f));

				return toRet;

			case rock:
				//-0.5 if cell is unpassable
				return -.5f;

			case organic_waste:
				//.5 if cell contains organics
				return .5f;

			case apple:
				//1.0 if cell contains an apple
				return 1.0f;
			}
		}

		//0 if empty
		return 0.0f;
	}
}


void Bot::Multiply(int numChildren, float energy_to_pass)
{
	if (ArtificialSelectionWatcher_OnDivide())
	{
		return;
	}

	int toGive = static_cast<int>((energy - GiveBirthCost*numChildren) * energy_to_pass);

	if (toGive < 0)
	{
		//Bot dies
		energy = 0;

		return;
	}


	for (int b = 0; b < numChildren; ++b)
	{
		if (energy <= 1 + GiveBirthCost)
		{
			return;
		}
		else
		{
			Point freeSpace;

			freeSpace = pField->FindFreeNeighbourCell(x, y);

			if (freeSpace.x != -1)
			{
				TakeEnergy(GiveBirthCost + toGive);

				//Adaptation check
				if ((!RandomPercentX10(pField->params.adaptation_botShouldBeOnLandOnceToMultiply)) or (wasOnLand))
				{
					pField->AddObject(new Bot(freeSpace.x, freeSpace.y, toGive, this, RandomPercent(MutationChancePercent)));
				}

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
			if (obj->type() == bot)
			{
				#ifdef BotCanEatBot
				{
					//Eat a bot
					GiveEnergy(obj->energy, predation);
					pField->RemoveBot(lookAt_x, lookAt_y);

					++numAttacks;
				}
				#endif
			}
			else if (obj->type() == apple)
			{
				//Eat apple
				GiveEnergy(obj->energy, organics);
				pField->RemoveObject(lookAt_x, lookAt_y);
			}
			#ifdef BotCanEatRock
			else if (obj->type() == rock)
			{
				//Eat rock, it gives no energy
				RemoveObject(lookAt_x, lookAt_y);
			}
			#endif
		}
	}
}

void Bot::EatOrganics()
{
	if (pField->IsInBounds(lookAt_x, lookAt_y))
	{		
		Object* obj = (*pCells)[lookAt_x][lookAt_y];

		//If there is an object
		if (obj)
		{
			if (obj->type() == organic_waste)
			{
				//Eat organics
				GiveEnergy(obj->energy, organics);
				pField->RemoveObject(lookAt_x, lookAt_y);
			}
		}
	}
}

void Bot::Photosynthesis()
{
	//Above water
	if (!pField->IsInWater(y))
	{
		int toGive = pField->params.PSreward;

		//Give energy depending on a season
		if (pField->params.useSeasons)
		{
			switch (pField->GetSeason())
			{
			case summer:
				toGive = static_cast<int>(toGive * PS_Multiplier_Summer);
				break;
			case autumn: case spring:
				toGive = static_cast<int>(toGive * PS_Multiplier_AutumnSpring);
				break;
			case winter:
				toGive = static_cast<int>(toGive * PS_Multiplier_Winter);
				break;
			}

			if (toGive < 1)
				toGive = 1;
		}

		GiveEnergy(toGive, PS);

		++numPSonLand;
	}
	//Below water
	else
	{		
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

		GiveEnergy(pField->params.PSreward, PS);
	}
}

constexpr ObjectTypes Bot::type()
{
	return bot;
}

bool Bot::isPredator()
{
	return (energyFromPredation > 0);
}

void Bot::Mutagen()
{
	if (RandomPercent(10))
		Mutate();
}


BrainOutput Bot::think(BrainInput input)
{
	//Compute
	activeBrain.Process(input);

	//Return output
	return activeBrain.GetOutput();
}



bool Bot::ArtificialSelectionWatcher_OnTick()
{	
	FieldDynamicParams& params = pField->params;	

	//Divers
	if (lifetime > 3)
	{
		if (numMovesX * 3 < (lifetime - 3))
		{
			if (RandomPercentX10(params.adaptation_forceBotMovementsX))
				return true;
		}

		if (numMovesY * 3 < (lifetime - 3))
		{
			if (RandomPercentX10(params.adaptation_forceBotMovementsY))
				return true;
		}
	}

	return false;
}


bool Bot::ArtificialSelectionWatcher_OnDivide()
{
	FieldDynamicParams& params = pField->params;

	//Winds
	if (pField->IsInWater(y))
	{
		if (pField->FindDistanceX(x, addaptation_birthX) < params.adaptation_StepsNumToDivide_Winds)
		{
			return true;
		}
	}

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

	if (ArtificialSelectionWatcher_OnTick())
		return 1;

	//Stunned means the creature can not act
	if (stunned)
	{
		--stunned;
		return 0;
	}

	if (energy <= 0)	
	{
		return 1;
	}
	#ifdef BotDiesOfOldAge
	else if (lifetime >= static_cast<uint>(pField->params.botMaxLifetime))
	{
		return 1;
	}
	#endif

	BrainOutput tmpOut;
	
	CalculateLookAt();

	if (!pField->IsInWater(y))
		wasOnLand = true;

	//Fill brain input structure
	BrainInput input = FillBrainInput();

	//Bot brain does its stuff
	tmpOut = think(input);

	//Multiply first
	if (tmpOut.divide_num > 0)
	{
		if(lifetime >= fertilityDelay)
		{
			Multiply(tmpOut.divide_num);

			if (energy <= 0)
				return 1;
		}
	}

	//Then attack or eat organics
	if (tmpOut.attack > 0)
	{
		//If dies of low energy
		if (TakeEnergy(AttackCost))
			return 1;
		else
		{
			if (pField->params.noPredators)
				return 1;

			Attack();
		}
	}
	else if (tmpOut.digestOrganics > 0)
	{
		//If dies of low energy
		if (TakeEnergy(EatOrganicsCost))
			return 1;
		else
		{
			EatOrganics();
		}
	}
	else
	{
		//Rotate after
		uint desired_rotation = RotationsReverse[tmpOut.desired_rotation_x][tmpOut.desired_rotation_y];

		if (desired_rotation != direction)
		{
			//If dies of low energy
			if (TakeEnergy(RotateCost))
				return 1;

 			Rotate(desired_rotation);
		}

		//Move
		if (tmpOut.move > 0)
		{
			if (TakeEnergy(MoveCost))
				return 1;

			//Move object to a new place
			int tmpY = y;
			int tmpX = x;

			if (pField->MoveObject(this, lookAt_x, lookAt_y) == 0)
			{
				if(lookAt_y!=tmpY)
				{
					++numMovesY;
				}
				if (lookAt_x != tmpX)
				{
					++numMovesX;
				}				
			}

		}
		//Photosynthesis
		else if (tmpOut.photosynthesis > 0)
		{
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
	SDL_SetTextureColorMod(sprite_body, color.c[0], color.c[1], color.c[2]);
	SDL_RenderCopy(renderer, sprite_body, &image_rect, &object_rect);

	//Draw outlines
	drawOutlineAndHead();
}


void Bot::drawEnergy()
{
	CalcScreenX();
	CalcObjectRect();

	//Draw body
	SDL_SetTextureColorMod(sprite_body, 255, static_cast<int>((1.0f - ((energy * 1.0f) / (MaxPossibleEnergyForABot * 1.0f))) * 255.0f), 0);
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
		SDL_SetTextureColorMod(sprite_body, static_cast<int>(255.0f * ((energyFromPredation * 1.0f) / (energySumm * 1.0f))),
			static_cast<int>(255.0f * ((energyFromPS * 1.0f) / (energySumm * 1.0f))),
			static_cast<int>(255.0f * ((energyFromOrganics * 1.0f) / (energySumm * 1.0f))));

	SDL_RenderCopy(renderer, sprite_body, &image_rect, &object_rect);

	//Draw outlines
	drawOutlineAndHead();
}


void Bot::Rotate(int dir)
{
	if (direction == dir)
		return;

	int delta = (direction - dir);	

	if (delta < 0)
	{
		if (delta < -4)
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

	if (direction < 0)
	{
		direction += 8;
	}
	else if (direction > 7)
	{
		direction -= 8;
	}
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

		if (energyFromPS>100)
		{
			energyFromPS = 100;
		}
	}
	else if (src == predation)
	{
		energyFromPredation += num;

		if (energyFromPredation > 100)
		{
			energyFromPredation = 100;
		}
	}
	else if (src == organics)
	{
		energyFromOrganics += num;

		if (energyFromOrganics > 100)
		{
			energyFromOrganics = 100;
		}
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

int Bot::GetDirection()
{
	return direction;
}

void Bot::SetDirection(int D)
{
	direction = D;
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


Bot::summary_return Bot::GetNeuronSummary()
{
	int toRet[6] = { 0,0,0,0,0,0 };
	Neuron* n;

	for (uint xi = 1; xi < NumNeuronLayers; ++xi)
	{
		for (uint yi = 0; yi < NumNeuronsInLayerMax; ++yi)
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

	return { toRet[0], toRet[1], toRet[2], toRet[3], toRet[4], toRet[5], NumNeuronLayers * NumNeuronsInLayerMax };
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
	color.Set(r, g, b);
}


Bot::Bot(int X, int Y, uint Energy, Bot* prototype, bool mutate) :Object(X, Y), initialBrain(&prototype->initialBrain)
{
	energy = Energy;
	stunned = StunAfterBirth;
	fertilityDelay = pParams->fertility_delay;
	energyFromPS = 0;
	energyFromPredation = 0;

	//Copy parent's markers and color
	memcpy(mutationMarkers, prototype->mutationMarkers, sizeof(mutationMarkers));
	nextMarker = prototype->nextMarker;

	color = prototype->color;

	//Random direction
	RandomDirection();

	//Now mutate
	if(!pField->params.noMutations)
	{
		if (mutate)
			Mutate();
	}

	//Create active brain
	activeBrain.Clone(&initialBrain);
	activeBrain.Optimize();

	addaptation_birthX = X;
}


Bot::Bot(int X, int Y, uint Energy) :Object(X, Y)
{
	RandomizeMarkers();

	energy = Energy;
	stunned = StunAfterBirth;
	fertilityDelay = pParams->fertility_delay;
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

	addaptation_birthX = X;

	//Temporary
	numMovesX = 1000;
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