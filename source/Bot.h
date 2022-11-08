#pragma once
//#pragma message("	Bot_h")


#include "Field.h"
#include "BotNeuralNet.h"



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
	predation,
	organics,
	unknown
};


//Preset colors
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



class Bot final:public Object
{
	//Rotation, see Rotations[]
	uint direction=0;

	//That is what a bot is looking at
	Point lookAt;
	int lookAt_x, lookAt_y;

	void CalculateLookAt();


	//Active brain - one that is used during simulation, may be changed or optimized
	BotNeuralNet activeBrain;
	//Bot gets initial brain from his parent, it is his original genes
	BotNeuralNet initialBrain;

	//if this is not 0, bot does nothing at his turn
	int stunned;

	//How long a bot should wait before multiply
	int fertilityDelay;

	Color color;

	//Energy acquired from different sources
	int energyFromPS = 0;
	int energyFromPredation = 0;
	int energyFromOrganics = 0;

	//Mutation markers used to decide how close two bots are to each other as relatives
	int mutationMarkers[NumberOfMutationMarkers];
	uint nextMarker;

	void ChangeMutationMarker();

	void RandomizeMarkers();
	void RandomizeColor();
	void RandomDirection();

	//Severe mutation function - Experimental
	void TotalMutation();

	//Shift color a little (-10 to +10)
	void ChangeColor(const int str = 10);

	//Experimental
	void SlightlyMutate();

	void Mutate();


	void drawOutlineAndHead();


	//Create brain input data
	BrainInput FillBrainInput();

	void Multiply(int numChildren);
	void Attack();
	void Photosynthesis();


	//----------------------------------------------------------------------------------------------
	//These functions are used for experiments such as adaptation,
	//you are supposed to call them at the end of tick() function, or do not use

	int adaptation_numTicks = 0;
	int adaptation_numRightSteps = 0;
	int addaptation_lastX;

	int adaptationCounter = 0;

	//How many times bot used attack and move commands
	int numAttacks = 0;
	int numMoves = 0;

	//Bot visited land
	bool wasOnLand = false;

	bool ArtificialSelectionWatcher_Winds();
	bool ArtificialSelectionWatcher_Divers();
		
	//----------------------------------------------------------------------------------------------


public:
	
	//Experimental
	void Mutagen();

	//Use neural network (feed forward)
	BrainOutput think(BrainInput input);

	//Bot tick function, it should always call parents tick function first
	int tick() override;
	

	void draw() override;
	void drawEnergy() override;
	void drawPredators() override;
		

	void Rotate(int dir = 1);
	

	void GiveEnergy(int num, EnergySource src = unknown);

	//Return current energy amount from different sources
	int GetEnergyFromPS();
	int GetEnergyFromKills();

	int* GetMarkers();
	Color* GetColor();

	BotNeuralNet* GetActiveBrain();
	BotNeuralNet* GetInitialBrain();

	//Take away bot energy, return true if 0 or below (bot dies)
	bool TakeEnergy(int val);

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
	struct summary_return
	{ 
		int simple, radialBasis, random, memory, connections, deadend, neurons; 
	} 
	GetNeuronSummary();

	/*Find out how close these two are as relatives,
	returns number of matching mutation markers*/
	int FindKinship(Bot* stranger);

	void SetColor(Color);
	void SetColor(Uint8, Uint8, Uint8);


	//Inherit from a parent
	Bot(int X, int Y, uint Energy, Bot* prototype, bool mutate = false);

	//New bot
	Bot(int X, int Y, uint Energy = MaxPossibleEnergyForABot);


	static Color GetRandomColor();

};

