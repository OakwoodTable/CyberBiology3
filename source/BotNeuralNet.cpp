
#include "BotNeuralNet.h"



float BotNeuralNet::ActivationSimple(float value)
{
	return (value >= 0.5f) ? 1.0f : 0.0f;
}

float BotNeuralNet::PlusMinusActivation(float value)
{
	if (value >= 0.5f)
		return 1.0f;
	else if (value <= -0.5f)
		return -1.0f;
	
	return 0.0f;
}

float BotNeuralNet::RadialBasisActivation(float value)
{
	return ((value >= 0.45f) && (value <= 0.55f)) ? 1.0f : 0.0f;
}

float BotNeuralNet::LinearActivation(float value)
{
	if (value > 1.0f)
		value = 1.0f;
	else if (value < 0.0f)
		value = 0.0f;

	return value;
}

void BotNeuralNet::ClearMemory()
{
	memset(allMemory, 0, sizeof(allMemory));
}


BotNeuralNet::BotNeuralNet()
{
	for (uint yi = 0; yi < NeuronsInLayer; ++yi)
	{
		allNeurons[NeuronOutputLayerIndex][yi].type = output;
		allNeurons[NeuronInputLayerIndex][yi].type = input;
	}

	ClearMemory();
}

void BotNeuralNet::Clone(BotNeuralNet* prototype)
{
	ClearMemory();
	memcpy(allNeurons, prototype->allNeurons, sizeof(allNeurons));
}

BotNeuralNet::BotNeuralNet(BotNeuralNet* prototype)
{
	BotNeuralNet();

	Clone(prototype);
}

void BotNeuralNet::Clear()
{
	memset(allValues, 0, sizeof(allValues));
}

void BotNeuralNet::Process()
{
	float* value;
	float* m;
	Neuron* n;

	//Parse all neurons, except output layer
	for (uint xi = 0; xi < NumNeuronLayers-1; ++xi)
	{
		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
		{
			value = &allValues[xi][yi];
			m = &allMemory[xi][yi];
			n = &allNeurons[xi][yi];

			//Skip calculation if neuron has no further connections
			if (n->numConnections == 0)
				continue;

			switch (n->type)
			{
			case basic:
				*value = ActivationSimple(*value + n->bias);
				//*value = Linear(*value + n->bias);
				break;

			case random:
				*value = (RandomVal(1000) <= ((int)((*value + n->bias) * 1000.0f))) ? 1.0f : 0.0f;
				break;

			case input:
				*value = *value + n->bias;
				break;

			case radialbasis:
				*value = RadialBasisActivation(*value + n->bias);
				break;

			case memory:

				if ((*value + n->bias) < .5f)
				{
					//Wipe memory
					*m = 0.0f;
				}
				else if ((*value + n->bias) >= .5f)
				{
					//Write in memory
					*m = 1.0f;
				}

				*value = *m;

				break;

			}

			//Feed forward

			NeuronConnection* tmpConnection;

			for (uint i = 0; i < n->numConnections; ++i)
			{
				tmpConnection = &n->allConnections[i];

				allValues[xi + 1][tmpConnection->dest] += *value * tmpConnection->weight;
			}
		}
	}


	//Output layer

	//Rotation
	value = &allValues[NeuronOutputLayerIndex][0];
	n = &allNeurons[NeuronOutputLayerIndex][0];

	*value = PlusMinusActivation(*value + n->bias);

	//All the rest
	for (uint oi = 1; oi < NeuronsInLayer; ++oi)
	{
		value = &allValues[NeuronOutputLayerIndex][oi];
		n = &allNeurons[NeuronOutputLayerIndex][oi];

		*value = ActivationSimple(*value + n->bias);
	}
}


void BotNeuralNet::MutateSlightly()
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


void BotNeuralNet::Mutate()
{
	uint randomNeuronIndex = RandomVal(NumNeuronLayers * NeuronsInLayer);
	uint counter = 0;
	Neuron* n;
	uint roll;

	for (uint xi = 0; xi < NumNeuronLayers; ++xi)
	{
		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
		{
			if (counter++ == randomNeuronIndex)
			{
				n = &allNeurons[xi][yi];
				roll = RandomVal(101);
								
				if(roll <= 5)
					n->SetRandom();
				else if (roll <= 7)
					n->mutate_DeleteNeuron();
				else if (roll <= 12)
					n->mutate_ChangeType();
				else if (roll <= 30)
					n->mutate_ChangeBias();
				else
					n->mutate_ChangeOneConnection();

				//IGNORE THE FOLLOWING!
				//Some other variants of mutate function 
				//Sorry!

				/*
				if (roll <= 5)
					n->SetRandom();
				else if (roll <= 20)
					n->mutate_ChangeBias();
				else
					n->mutate_ChangeOneConnection();*/
					
				/*	
				if (roll <= 5)
					n->SetRandom();
				else if (roll <= 20)
					n->mutate_ChangeBias();
				else
					n->mutate_ChangeOneConnection();*/

				/*
					n->SetRandom();
				*/
			}
		}
	}
}


void BotNeuralNet::MutateHarsh()
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


void BotNeuralNet::Optimize()
{
	Neuron* n;
	
	//TODO
	for (int x = NeuronOutputLayerIndex - 1; x > 0; --x)
	{
		for (int y = 0; y < NeuronsInLayer; ++y)
		{
			n = &allNeurons[x][y];

			//If neuron is a dead-end
			if (n->numConnections == 0)
			{
				//Clear bias
				n->SetZero();

				//Clear all connections to this neuron in prev. layer
				for (int cy = 0; cy < NeuronsInLayer; ++cy)
				{
					int ind = allNeurons[x - 1][cy].IsConnected(y);

					if (ind != -1)
					{
						allNeurons[x - 1][cy].RemoveConnection(ind);
					}
				}
			}
		}
	}
}


void BotNeuralNet::Randomize()
{
	//Input layer
	for (uint y = 0; y < NeuronsInLayer; ++y)
	{
		allNeurons[NeuronInputLayerIndex][y].SetRandomConnections();
		allNeurons[NeuronInputLayerIndex][y].SetRandomBias();
	}

	//Hidden layers
	for (uint xi = 1; xi < NumNeuronLayers - 1; ++xi)
	{
		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
		{
			allNeurons[xi][yi].SetRandom();
		}
	}

	//Output layer
	for (uint y = 0; y < NeuronsInLayer; ++y)
	{
		allNeurons[NeuronOutputLayerIndex][y].SetRandomBias();
	}
}


BrainOutput BotNeuralNet::GetOutput()
{
	BrainOutput toRet;

	repeat(NeuronsInLayer)
		toRet.fields[i] = (int)allValues[NeuronOutputLayerIndex][i];

	return toRet;
}


void BotNeuralNet::SetDummy()
{
	//Preset brain

	/*
	Example:
	
	allNeurons[1][3].SetTunnel(3);
	...
	allNeurons[4][3].bias = 0.0f;
	allNeurons[4][3].AddConnection(3, 0.5f);
	*/
}

BrainOutput BrainOutput::GetEmptyBrain()
{
	return { 0, 0, 0, 0, 0 };
};
