#pragma once

#include "Settings.h"
#include "Utils.h"


struct NeuronConnection final
{
	byte dest_layer : 4;
	byte dest_neuron : 4;

	int8_t weight : 8;

	void ChangeWeight();
	void SetRandomWeight();
};


enum NeuronType : byte
{
	basic,
	input,
	output,
	random,
	radialbasis,
	memory
};

constexpr int MaxConnectionsPerNeuronType[] = 
{ 
	MaxConnections_Basic, 
	MaxConnections_Input, 
	0,
	MaxConnections_Random,
	MaxConnections_RadialBasis,
	MaxConnections_Memory 
};

//Neuron description by type (for GUI)
constexpr const char* NeuronTypeNames[] =
{
	"basic",
	"input",
	"output",
	"random",
	"radial basis",
	"memory"
};


struct Neuron final
{
	NeuronType type : 4;
	byte layer : 4;

	int8_t bias : 8;
	
	byte numConnections : 8;
	NeuronConnection allConnections[NumNeuronsInLayerMax];
	

	Neuron();

	void Clone(Neuron* source);

	void AddConnection(uint DEST_LAYER, uint DEST, int8_t WEIGHT);
	void AddConnection(NeuronConnection* prototype);
	bool AddRandomConnection();
	void RemoveConnection(uint index);

	uint GetRandomConnectionIndex();

	//Does neuron have a connection,
	//returns connection index or -1
	int IsConnected(uint LAYER, uint index);

	void SetRandomBias();
	void SetRandomType();
	void SetRandomConnections();
	void SetRandom();

	void MakeFullyConnected();	

	//Clear bias and connections
	void SetZero();

	void ClearConnections();	

	//Tunnel neuron - one with no bias and only 1 connection to same neuron in next layer with weight = 1.0f
	void SetTunnel(int num);

	bool IsInactive();

	
	//Mutation functions
	void mutate_ChangeType();
	void mutate_ChangeBias();
	void mutate_ChangeConnection(uint index);
	void mutate_DeleteNeuron();
	
};
