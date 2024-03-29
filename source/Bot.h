#pragma once
//#pragma message("	Bot_h")

#include "Field.h"
#include "BotNeuralNet.h"



enum EnergySource
{
	PS,
	predation,
	organics,
	unknown
};


//Preset colors
const Uint8 presetColors[][3] =
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

//Rotations array, contains where a bot would look with every
//position of its head
const Point Rotations[8] =
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

const int RotationsReverse[3][3] =
{
	{7, 6, 5},
	{0, -1, 4},
	{1, 2, 3}
};



class Bot : public Object
{

protected:

	//Rotation, see Rotations[]
	int direction=0;

	//That is what a bot is looking at
	Point lookAt;
	int lookAt_x, lookAt_y;

	void CalculateLookAt();


	//Active brain - one that is used during simulation, may be changed or optimized
	BotNeuralNet activeBrain;
	//Bot gets initial brain from his parent, it is his original genes
	BotNeuralNet initialBrain;

	//if this is not 0, bot does nothing at his turn
	uint stunned;

	//How long a bot should wait before multiply
	uint fertilityDelay;

	Color color;

	//Energy from different sources
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

	void Mutate();

	void UpdateActiveBrain();

	void drawOutlineAndHead();


	//Create brain input data
	BrainInput FillBrainInput();
	void FillSightNeurons(Point at, float& n1, float& n2);

	void Multiply(int numChildren, float energy_to_pass = 0.5f);
	void Attack();
	void EatOrganics();
	void Photosynthesis();
	void Rotate(int dir = 1);

	//Take away bot energy, return true if 0 or below (bot dies)
	bool TakeEnergy(int val);
	void GiveEnergy(int num, EnergySource src = unknown);



	//----------------------------------------------------------------------------------------------
	//These functions are used for experiments such as adaptation,
	//you are supposed to call them in tick() function, or do not use

	int addaptation_birthX;

	//How many times bot used attack, move and PS commands
	uint numAttacks = 0;
	uint numMovesX = 0;
	uint numMovesY = 0;
	uint numPSonLand = 0;


	bool ArtificialSelectionWatcher_OnTick();
	bool ArtificialSelectionWatcher_OnDivide();
		
	//----------------------------------------------------------------------------------------------

public:

	constexpr ObjectTypes type() override;
	constexpr float image_sensor_val() override;
	
	bool isPredator();

	//Experimental
	void Mutagen();

	//Use neural network (feed forward)
	BrainOutput think(BrainInput input);

	//Bot tick function, it should always call parents tick function first
	int tick() override;	

	void draw() override;
	void drawEnergy() override;
	void drawPredators() override;	

	//Return current energy amount from different sources
	int GetEnergyFromPS();
	int GetEnergyFromKills();
	int GetDirection();
	void SetDirection(int);

	int* GetMarkers();
	Color* GetColor();

	BotNeuralNet* GetActiveBrain();
	BotNeuralNet* GetInitialBrain();	

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
	};

	summary_return GetNeuronSummary();

	/*Find out how close these two are as relatives,
	returns number of matching mutation markers*/
	int FindKinship(Bot* stranger);

	void SetColor(Color);


	//Inherit from a parent
	Bot(int X, int Y, uint Energy, Bot* prototype, bool mutate = false);

	//New bot
	Bot(int X, int Y, uint Energy = BotMaxEnergyInitial);


	static void SetBodyImage(SDL_Texture* img);
	static void SetHeadImages(SDL_Texture* img[8]);


private:

	static SDL_Texture* sprite_head[8];
	static SDL_Texture* sprite_body;

};

