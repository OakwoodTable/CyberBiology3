#include "Neuron.h"


Neuron::Neuron()
{
	SetRandomType();
}

void Neuron::Clone(Neuron* source)
{
	memcpy(this, source, sizeof(Neuron));
}

void Neuron::AddConnection(uint DEST_LAYER, uint DEST, float WEIGHT=1000.0f)
{
	NeuronConnection* nConnection = &allConnections[numConnections];

	nConnection->dest_neuron = DEST;
	nConnection->dest_layer = DEST_LAYER;

	if (WEIGHT < 1000.0f)
		nConnection->weight = WEIGHT;
	else
		nConnection->SetRandomWeight();

	++numConnections;
}

bool Neuron::AddRandomConnection()
{
	if (numConnections >= MaxConnectionsPerNeuronType[type])
		return false;

	int c_Index, c_Layer;

	for (;;)
	{
		c_Index = RandomVal(NeuronsInLayer);

		if (layer == NumNeuronLayers - 2)
			c_Layer = NumNeuronLayers - 1;
		else
			c_Layer = RandomVal(NumNeuronLayers - (layer + 1)) + layer + 1;

		//If not connected
		if (IsConnected(c_Layer, c_Index) == -1)
		{
			AddConnection(c_Layer, c_Index);

			return true;
		}
	}	
}

void Neuron::RemoveConnection(uint index)
{
	--numConnections;

	for (int i = index; i < numConnections; ++i)
	{
		allConnections[i] = allConnections[i + 1];
	}
}

int Neuron::IsConnected(uint LAYER, uint index)
{
	NeuronConnection* c;

	for (uint i = 0; i < numConnections; ++i)
	{
		c = &allConnections[i];

		if (c->dest_layer == LAYER)
		{
			if(c->dest_neuron == index)
				return i;
		}			
	}

	return -1;
}

void Neuron::ClearConnections()
{
	numConnections = 0;
}

void Neuron::SetRandomBias()
{
	bias = RandomFloatInRange(NeuronBiasMin, NeuronBiasMax);
}

void Neuron::SetRandomType()
{
	if ((type != input) && (type != output))
	{
	#ifdef UseRandomNeuron
		if (RandomPercent(2))
		{
			type = random;

			return;
		}
	#endif

	#ifdef UseMemoryNeuron
		int roll = RandomVal(12);
	#else
		int roll = RandomVal(14);
	#endif

		if (roll <= 5)
			type = basic;
		else if (roll <= 10)
			type = radialbasis;
		else
			type = memory;
				
	}
}

void Neuron::SetRandomConnections()
{
	ClearConnections();

	if (type == output)
		return;

	#ifdef FullyConnected
		MakeFullyConnected();
		return;
	#endif

	uint connections = RandomVal(MaxConnectionsPerNeuronType[type] + 1);

	for (; connections > 0; --connections)
	{
		if (connections > 0)
		{
			if (!AddRandomConnection())
				return;

		}
	}
}

void Neuron::MakeFullyConnected()
{
	repeat(NeuronsInLayer)
	{
		allConnections[i].dest_layer = layer + 1;
		allConnections[i].dest_neuron = i;

		allConnections[i].SetRandomWeight();
	}

	numConnections = NeuronsInLayer;
}

void Neuron::SetRandom()
{
	SetRandomBias();
	SetRandomType();
	SetRandomConnections();
}

void Neuron::SetZero()
{
	ClearConnections();
	bias = 0.0f;
}

void Neuron::SetTunnel(int num)
{
	SetZero();

	allConnections[0].weight = 1.0f;

	allConnections[0].dest_neuron = num;
	allConnections[0].dest_layer = layer + 1;
}

void Neuron::SlightlyChange()
{
	bias += (RandomVal(1001) * 0.0001f) - .05f;

	for (uint i = 0; i < numConnections; ++i)
	{
		allConnections[i].weight += (RandomVal(2001) * 0.0001f) - .1f;
	}
}

void Neuron::mutate_ChangeType()
{
	SetRandomType();
}

void Neuron::mutate_ChangeBias()
{
	float change = RandomFloatInRange(ChangeBiasMin, ChangeBiasMax);

	if (RandomPercent(50))
		change *= -1.0f;

	bias += change;

	if (bias > NeuronBiasMax)
		bias = NeuronBiasMax;
	else if (bias < NeuronBiasMin)
		bias = NeuronBiasMin;
}

uint Neuron::GetRandomConnectionIndex()
{
	return RandomVal(numConnections);
}

void Neuron::mutate_ChangeOneConnection()
{	
	if (numConnections == 0)
		return;

	int roll = RandomVal(101);
 	int connection = GetRandomConnectionIndex();

	if (roll <= 10)
	{
		//10% - remove connection
		RemoveConnection(connection);
	}
	else if (roll <= 20)
	{
		//10% - add random connection
		AddRandomConnection();
	}
	else
	{
		//Change connection weight
		allConnections[connection].ChangeWeight();
	}

}

void Neuron::mutate_DeleteNeuron()
{
	SetZero();

	if((type!=input)&&(type!=output))
		type = basic;
}

char* Neuron::GetTextFromType(NeuronType t)
{
	switch (t)
	{
		case basic:
			return (char*)"basic";
		case input:
			return (char*)"input";
		case output:
			return (char*)"output";
		case radialbasis:
			return (char*)"radial basis";
		case memory:
			return (char*)"memory";
		case random:
			return (char*)"random";
		default:
			return (char*)"other";
	}
}

void Neuron::SortConnections()
{
	//TODO
}

void NeuronConnection::ChangeWeight()
{
	float change = RandomFloatInRange(ChangeConnectionWeightMin, ChangeConnectionWeightMax);

	if (RandomPercent(50))
		change *= -1.0f;

	weight += change;

	if (weight > ConnectionWeightMax)
		weight = ConnectionWeightMax;
	else if (weight < ConnectionWeightMin)
		weight = ConnectionWeightMin;
}

void NeuronConnection::SetRandomWeight()
{
	weight = RandomFloatInRange(ConnectionWeightMin, ConnectionWeightMax);
}
