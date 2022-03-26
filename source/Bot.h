#pragma once


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



	void ChangeMutationMarker();

	//Set random mutation markers
	void RandomizeMarkers();

	//Set random color
	void RandomizeColor();

	//Experimental
	//Total mutation function - rarely used
	void TotalMutation();

	//Shift color a little (-10 to +10)
	void ChangeColor(const int str = 10);

	//Experimental
	void SlightlyMutate();

	//Main mutate function
	void Mutate();

	//Draw bot outline and his head
	void drawOutlineAndHead(SDL_Rect rect);


	//----------------------------------------------------------------------------------------------
	//Функции для осуществления селекции, вызывается в конце функции tick(), либо не использовать

	int selection_numTicks = 0;
	int selection_numRightSteps = 0;
	int selection_lastX;

	bool SelectionWatcher();

	public: static int selectionStep;
	//----------------------------------------------------------------------------------------------


public:

	//Experimental
	//This function is used to simulate mutagen
	void Radiation();

	//Use neural network
	BrainOutput think(BrainInput input);

	//Bot tick function, it should always call parents tick function first
	int tick() override;

	//Bot main draw function
	void draw() override;

	//Bot draw function is energy mode
	void drawEnergy();

	//Bot draw function is predators mode
	void drawPredators();

	//Change rotation function
	void Rotate(int dir = 1);

	//Give a bot some energy
	void GiveEnergy(int num, EnergySource src = unknown);

	//Return current energy amount from different sources
	int GetEnergy();
	int GetEnergyFromPS();
	int GetEnergyFromKills();

	//Returns a pointer to mutation markers array
	int* GetMarkers();

	//Get bot color
	Uint8* GetColor();

	//Get neural net pointer
	Neuron* GetNeuralNet();

	//Get brain
	BotNeuralNet* GetBrain();

	//Get rotation
	Point GetDirection();

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
	struct summary_return{ int simple, radialBasis, random, memory, connections, deadend, neurons; } GetNeuronSummary();

	/*Find out how close these two are as relatives,
	returns number of matching mutation markers*/
	int FindKinship(Bot* stranger);

	//Change bot color
	void Repaint(Uint8 [3]);

	//Inherit from a parent
	Bot(int X, int Y, uint Energy, Bot* prototype, bool mutate = false);

	//New bot
	Bot(int X, int Y, uint Energy = 100);



	//This function is used only after a bot was loaded from file!!
	void GiveInitialEnergyAndMarkers();


	
	static struct s_Color{ Uint8 rgb[3]; } GetRandomColor();


};

