#pragma once


#include "../../../Settings.h"


//Sorry, no comments here atm :(


struct BrainInput
{
	float
	energy = 0.0f,
	vision = 0.0f,
	isRelative = 0.0f,
	rotation = 0.0f,
	height = 0.0f;
};



union BrainOutput
{
	static const int numFields = 5;

	struct
	{
		int rotate;
		int move;
		int photosynthesis;
		int divide;
		int attack;
	};

	int fields[numFields];
};


#include "Neuron.h"



class BotNeuralNet
{

private:

	float Activation(float value)
	{
		return (value >= 0.5f) ? 1.0f : 0.0f;
	}

	float RadialBasisActivation(float value)
	{
		return ((value >= 0.45f) && (value <= 0.55f)) ? 1.0f : 0.0f;
	}

	void ClearMemory()
	{
		memset(allMemory, 0, sizeof(allMemory));
	}


public:

	//All neurons
	Neuron allNeurons[NumNeuronLayers][NeuronsInLayer];
	float allValues[NumNeuronLayers][NeuronsInLayer];
	float allMemory[NumNeuronLayers][NeuronsInLayer];



	BotNeuralNet()
	{
		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
		{
			allNeurons[NeuronOutputLayerIndex][yi].type = output;
			allNeurons[NeuronInputLayerIndex][yi].type = input;
		}

		ClearMemory();
	}

	BotNeuralNet(BotNeuralNet* prototype)
	{
		BotNeuralNet();

		ClearMemory();
		memcpy(allNeurons, prototype->allNeurons, sizeof(allNeurons));

	}

	void Clear()
	{
		memset(allValues, 0, sizeof(allValues));
	}

	void Process()
	{
		float* value;
		float* m;
		Neuron* n;
		int t, v;

		for (uint xi = 0; xi < NumNeuronLayers; ++xi)
		{
			for (uint yi = 0; yi < NeuronsInLayer; ++yi)
			{
				value = &allValues[xi][yi];
				m = &allMemory[xi][yi];
				n = &allNeurons[xi][yi];

				//Temporary - костыль
				if ((n->numConnections == 0) && (n->type != output))
				{
					continue;
				}

				switch (n->type)
				{ 
				case basic: 
					*value = Activation(*value + n->bias);
					break;

				case random:
					t = RandomVal(1000);
					v = (int)((*value + n->bias) * 1000.0f);
					*value = (t <= v)?1.0f:0.0f;
					break;

				case output:
					*value = Activation(*value + n->bias);
					continue;

				case input:
					*value = *value + n->bias;
					break;

				case radialbasis:
					*value = RadialBasisActivation(*value + n->bias);
					break;

				case memory:					
					if ((*value + n->bias) <= -.5f)
					{
						//Wipe memory
						*m = 0.0f;
					}
					else if((*value + n->bias) >= .5f)
					{
						//Write in memory
						*m += Activation(*value + n->bias);

						//Maximal value
						if (*m > 5.0f)
							*m = 5.0f;
					}
					*value = *m;
					break;
				}
				
				for (uint i = 0; i < n->numConnections; ++i)
				{
					allValues[xi+1][n->allConnections[i].num] += *value * n->allConnections[i].weight;
				}
			}
		}
	}


	/*Very little change, no new connections,
	  experimental*/
	void MutateSlightly()
	{
		uint randomNeuronIndex = RandomVal(NumNeuronLayers * NeuronsInLayer);
		uint counter = 0;

		for (uint xi = 0; xi < NumNeuronLayers; ++xi)
		{
			for (uint yi = 0; yi < NeuronsInLayer; ++yi)
			{

				if (counter++ == randomNeuronIndex)
				{
					Neuron* n = &allNeurons[xi][yi];

					n->SlightlyChange();
				}

			}
		}
	}


	//Change one neuron
	void Mutate()
	{
		uint randomNeuronIndex = RandomVal(NumNeuronLayers*NeuronsInLayer);
		uint counter = 0;

		for (uint xi = 0; xi < NumNeuronLayers; ++xi)
		{
			for (uint yi = 0; yi < NeuronsInLayer; ++yi)
			{
				if (counter++ == randomNeuronIndex)
				{
					Neuron* n = &allNeurons[xi][yi];

					n->SetRandom();
				}
			}
		}
	}

	//Change half neurons a lot
	void MutateHarsh()
	{
		for (uint xi = 0; xi < NumNeuronLayers; ++xi)
		{
			for (uint yi = 0; yi < NeuronsInLayer; ++yi)
			{

				if (RandomPercent(50))
					continue;

				Neuron* n = &allNeurons[xi][yi];

				n->SetRandom();

			}
		}
	}

	//Make completely random
	void Randomize()
	{
		for (uint y = 0; y < NeuronsInLayer; ++y)
		{
			allNeurons[NeuronInputLayerIndex][y].SetRandomConnections();
			allNeurons[NeuronInputLayerIndex][y].SetRandomBias();
		}

		for (uint xi = 1; xi < NumNeuronLayers-1; ++xi)
		{
			for (uint yi = 0; yi < NeuronsInLayer; ++yi)
			{
				allNeurons[xi][yi].SetRandom();
			}
		}

		for (uint y = 0; y < NeuronsInLayer; ++y)
		{
			allNeurons[NeuronOutputLayerIndex][y].SetRandomBias();
		}
	}

	
	BrainOutput GetOutput()
	{
		BrainOutput toRet;

		repeat(BrainOutput::numFields)
			toRet.fields[i] = (int)allValues[NeuronOutputLayerIndex][i];

		return toRet;
	}


	void SetDummy()
	{
		allNeurons[NeuronOutputLayerIndex][2].bias = 1.0f;
		allNeurons[NeuronInputLayerIndex][0].SetTunnel(3);
		allNeurons[1][3].SetTunnel(3);
		allNeurons[2][3].SetTunnel(3);
		allNeurons[3][3].SetTunnel(3);
		allNeurons[4][3].SetTunnel(3);
	}

};