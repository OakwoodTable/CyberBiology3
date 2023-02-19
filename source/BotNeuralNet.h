#pragma once

#include "Neuron.h"


union BrainInput
{
	struct
	{
		float
		energy,
		age,
		rotation,
		height,	//Y coordinate
		area,	//is in ocean/mud?
		eye1,	//Sight
		eye1_isrelative,
		eye_islookingatme;
		//eye2;	//Second sight cell
	};

	float fields[NumInputNeurons] = {};
};



union BrainOutput
{
	struct
	{
		int desired_rotation_x;
		int desired_rotation_y;

		int move;
		int photosynthesis;
		int attack;
		int digestOrganics;

		int divide_num;
		int divide_energy;		
	};

	int fields[NumOutputNeurons];
};



class BotNeuralNet
{
private:

	//Activation functions
	float ActivationSimple(float value);
	float PlusMinusActivation(float value);
	float RadialBasisActivation(float value);
	float LinearActivation(float value);
		
	void ClearMemory();
	void ClearValues();	

	void SetDummy();

public:

	Neuron allNeurons[NumNeuronLayers][NumNeuronsInLayerMax];

	//Neuron values and memory
	float allValues[NumNeuronLayers][NumNeuronsInLayerMax];
	int8_t allMemory[NumNeuronLayers][NumNeuronsInLayerMax];

	void Clone(BotNeuralNet* prototype);

	BotNeuralNet();
	BotNeuralNet(BotNeuralNet* prototype);

	void Process(BrainInput input);

	void Mutate();
	void Randomize();
	void Optimize();
	
	BrainOutput GetOutput();
};
