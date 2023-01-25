#pragma once

#include "Settings.h"
#include "MyTypes.h"


struct NeuronConnection
{
	byte dest_layer;
	byte dest_neuron;

	float weight;

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
	float bias;
	NeuronType type = basic;	

	byte layer;

	byte numConnections = 0;
	NeuronConnection allConnections[NumNeuronsInLayerMax];
	

	Neuron();

	void Clone(Neuron* source);

	void AddConnection(uint DEST_LAYER, uint DEST, float WEIGHT);
	void AddConnection(NeuronConnection* prototype);
	bool AddRandomConnection();
	void RemoveConnection(uint index);

	uint GetRandomConnectionIndex();

	//Sort connections by index
	void SortConnections();

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
