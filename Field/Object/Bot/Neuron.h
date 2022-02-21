#pragma once

#include "../../../Settings.h"

//Sorry, no comments here atm :(


struct NeuronConnection
{
	uint num;
	float weight;
};


enum NeuronType
{
	basic,
	input,
	output,
	random,
	radialbasis,
	memory
};


struct Neuron
{
	NeuronType type = basic;

	float bias = 0.0f;

	uint numConnections = 0;
	NeuronConnection allConnections[NeuronsInLayer];	



	void AddConnection(uint NUM, float WEIGHT)
	{
		allConnections[numConnections].num = NUM;
		allConnections[numConnections++].weight = WEIGHT;
	}

	bool IsConnected(uint index)
	{
		for (uint i = 0; i < numConnections; ++i)
		{
			if (allConnections[i].num == index)
				return true;
		}

		return false;
	}

	void ClearConnections()
	{
		numConnections = 0;
	}

	void SetRandomBias()
	{
		bias = (RandomVal(40001) * 0.0001f) - 2.0f;
	}

	void SetRandomType()
	{
		if ((type != input) && (type != output))
		{
			int refVal = RandomVal(14);
			
			if(refVal <= 8)
				type = basic;				
			else if(refVal <= 13)
				type = radialbasis;
			else
				type = memory;			

			if (RandomPercent(2))
				type = random;
		}
	}

	void SetRandomConnections()
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

	void SetRandom()
	{
		SetRandomBias();
		SetRandomType();
		SetRandomConnections();
	}

	void SetZero()
	{
		ClearConnections();
		bias = 0.0f;
	}

	void SetTunnel(int num)
	{
		bias = 0.0f;
		numConnections = 1;
		allConnections[0].weight = 1.0f;
		allConnections[0].num = num;
	}

	void SlightlyChange()
	{
		bias += (RandomVal(1001) * 0.0001f) - .05f;

		for (uint i = 0; i < numConnections; ++i)
		{
			allConnections[i].weight += (RandomVal(2001) * 0.0001f) - .1f;
		}
	}

};