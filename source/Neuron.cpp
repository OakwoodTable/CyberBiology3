#include "Neuron.h"


Neuron::Neuron()
{
	SetRandomType();
}

void Neuron::Clone(Neuron* source)
{
	memcpy(this, source, sizeof(Neuron));
}

void Neuron::AddConnection(uint DEST, float WEIGHT=1000.0f)
{
	allConnections[numConnections].dest = DEST;

	if (WEIGHT < 1000.0f)
		allConnections[numConnections].weight = WEIGHT;
	else
		allConnections[numConnections].SetRandomWeight();

	++numConnections;
}

bool Neuron::AddRandomConnection()
{
	if (numConnections >= MaxConnectionsPerNeuronType[type])
		return false;

	int connectionIndex;

	for (;;)
	{
		connectionIndex = RandomVal(NeuronsInLayer);

		if (IsConnected(connectionIndex) == -1)
		{
			AddConnection(connectionIndex);

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

int Neuron::IsConnected(uint index)
{
	for (uint i = 0; i < numConnections; ++i)
	{
		if (allConnections[i].dest == index)
			return i;
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
		allConnections[i].dest = i;
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
	bias = 0.0f;
	numConnections = 1;
	allConnections[0].weight = 1.0f;
	allConnections[0].dest = num;
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

void Neuron::mutate_ChangeOneConnection()
{	
	if (numConnections == 0)
		return;

	int roll = RandomVal(101);
 	int connection = RandomVal(numConnections);

	if (roll <= 10)
		RemoveConnection(connection);
	else if (roll <= 20)
		AddRandomConnection();
	else
		allConnections[connection].ChangeWeight();

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
