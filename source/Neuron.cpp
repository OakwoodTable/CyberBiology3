#include "Neuron.h"


Neuron::Neuron()
{
	SetRandomType();

	type = basic;
	numConnections = 0u;
}

void Neuron::Clone(Neuron* source)
{
	memcpy(this, source, sizeof(Neuron));
}

void Neuron::AddConnection(uint DEST_LAYER, uint DEST, int8_t WEIGHT = 120)
{
	NeuronConnection& nConnection = allConnections[numConnections];

	nConnection.dest_neuron = DEST;
	nConnection.dest_layer = DEST_LAYER;

	if (WEIGHT < 120)
		nConnection.weight = WEIGHT;
	else
		nConnection.SetRandomWeight();

	++numConnections;
}

void Neuron::AddConnection(NeuronConnection* prototype)
{
	AddConnection(prototype->dest_layer, prototype->dest_neuron, prototype->weight);
}

bool Neuron::AddRandomConnection()
{
	if (numConnections >= MaxConnectionsPerNeuronType[type])
		return false;

	int c_Index, c_Layer;

	for (;;)
	{
		if (layer == NumNeuronLayers - 2)
			c_Layer = NeuronOutputLayerIndex;
		else
			c_Layer = RandomVal(NumNeuronLayers - (layer + 1)) + layer + 1;

		if(c_Layer == NeuronOutputLayerIndex)
		{
			c_Index = RandomVal(NumOutputNeurons);
			c_Layer = NeuronOutputLayerIndex;
		}
		else
			c_Index = RandomVal(NumHiddenNeurons);				

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
	bias = RandomValRange(NeuronBiasMin, NeuronBiasMax);
}

void Neuron::SetRandomType()
{
	if ((type != input) && (type != output))
	{
		#ifdef UseRandomNeuron
		{
			if (RandomPercent(2))
			{
				type = random;

				return;
			}
		}
		#endif

		#ifdef UseMemoryNeuron
		int roll = RandomVal(12);
		#else
		int roll = RandomVal(11);
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
	{
		MakeFullyConnected();
		return;
	}
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
	repeat(NumNeuronsInLayerMax)
	{
		allConnections[i].dest_layer = layer + 1;
		allConnections[i].dest_neuron = i;

		allConnections[i].SetRandomWeight();
	}

	numConnections = NumNeuronsInLayerMax;
}

void Neuron::SetRandom()
{
	SetRandomBias();
	SetRandomType();
	SetRandomConnections();
}

bool Neuron::IsInactive()
{
	return numConnections == 0;
}

void Neuron::SetZero()
{
	ClearConnections();
	bias = 0;
}

void Neuron::SetTunnel(int num)
{
	SetZero();

	allConnections[0].weight = WeightValueCorrespondingTo_1;
	allConnections[0].dest_neuron = num;
	allConnections[0].dest_layer = layer + 1;
}

void Neuron::mutate_ChangeType()
{
	SetRandomType();
}

void Neuron::mutate_ChangeBias()
{
	char change = RandomValRange(ChangeBiasMin, ChangeBiasMax);

	if (RandomPercent(50))
		change *= -1;

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

void Neuron::mutate_ChangeConnection(uint index)
{	
	int roll = RandomVal(101);
 	int connection = index;

	if (roll <= 15)
	{
		//15% - remove connection
		RemoveConnection(connection);
	}
	else if (roll <= 30)
	{
		//15% - add random connection
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

	if((type!=input) and (type!=output))
		type = basic;
}

void NeuronConnection::ChangeWeight()
{
	char change = RandomValRange(ChangeConnectionWeightMin, ChangeConnectionWeightMax);

	if (RandomPercent(50))
		change *= -1;

	weight += change * WeightValueCorrespondingTo_1;

	if (weight > ConnectionWeightMax)
		weight = ConnectionWeightMax;
	else if (weight < ConnectionWeightMin)
		weight = ConnectionWeightMin;
}

void NeuronConnection::SetRandomWeight()
{
	weight = (RandomValRange(ConnectionWeightMin, ConnectionWeightMax) * WeightValueCorrespondingTo_1);
}
