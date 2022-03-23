#include "Neuron.h"


void Neuron::AddConnection(uint NUM, float WEIGHT)
{
	allConnections[numConnections].num = NUM;
	allConnections[numConnections++].weight = WEIGHT;
}

bool Neuron::IsConnected(uint index)
{
	for (uint i = 0; i < numConnections; ++i)
	{
		if (allConnections[i].num == index)
			return true;
	}

	return false;
}

void Neuron::ClearConnections()
{
	numConnections = 0;
}

void Neuron::SetRandomBias()
{
	bias = (RandomVal(40001) * 0.0001f) - 2.0f;
}

void Neuron::SetRandomType()
{
	if ((type != input) && (type != output))
	{
		int refVal = RandomVal(14);

		if (refVal <= 8)
			type = basic;
		else if (refVal <= 13)
			type = radialbasis;
		else
			type = memory;

		if (RandomPercent(2))
			type = random;
	}
}

void Neuron::SetRandomConnections()
{

	ClearConnections();

	if (type == output)
		return;

	uint connections = RandomVal(MaxConnectionsPerNeuron + 1);

	if (connections > 0)
	{
		uint connectionIndex;

		for (;;)
		{
			connectionIndex = RandomVal(NeuronsInLayer);

			if (!IsConnected(connectionIndex))
			{
				AddConnection(connectionIndex, (RandomVal(40001) * 0.0001f) - 2.0f);

				if (--connections == 0)
					return;
			}
		}
	}
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
	allConnections[0].num = num;
}

void Neuron::SlightlyChange()
{
	bias += (RandomVal(1001) * 0.0001f) - .05f;

	for (uint i = 0; i < numConnections; ++i)
	{
		allConnections[i].weight += (RandomVal(2001) * 0.0001f) - .1f;
	}
}

void Neuron::SortConnections()
{
	//TODO

}