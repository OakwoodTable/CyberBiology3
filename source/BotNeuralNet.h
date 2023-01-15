#pragma once


#include "Neuron.h"


struct BrainInput
{
	float
	energy = 0.0f,
	vision = 0.0f,
	vision2 = 0.0f,
	age = 0.0f,
	rotation = 0.0f,
	height = 0.0f;
};



union BrainOutput
{
	struct
	{
		int desired_rotation;
		int move;
		int photosynthesis;
		int divide;
		int attack;
	};

	int fields[NeuronsInLayer];

	static BrainOutput GetEmptyBrain();
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


public:

	Neuron allNeurons[NumNeuronLayers][NeuronsInLayer];

	//Neuron values and memory
	float allValues[NumNeuronLayers][NeuronsInLayer];
	float allMemory[NumNeuronLayers][NeuronsInLayer];

	void Clone(BotNeuralNet* prototype);

	BotNeuralNet();
	BotNeuralNet(BotNeuralNet* prototype);

	//Clear all values
	void Clear();

	void Process();

	/*Very little change, no new connections,
	  experimental*/
	void MutateSlightly(); 

	void Mutate();
	void MutateHarsh();

	void Randomize();

	void Optimize();
	
	BrainOutput GetOutput();

	void SetDummy();

};
