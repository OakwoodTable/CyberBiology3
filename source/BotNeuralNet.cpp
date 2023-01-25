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
	
	return 0;
}

float BotNeuralNet::RadialBasisActivation(float value)
{
	return ((value >=0.45f) and (value <= 0.55f)) ? 1.0f : 0.0f;
}

float BotNeuralNet::LinearActivation(float value)
{
	if (value > 1.0f)
		value = 1.0f;
	else if (value < 0)
		value = 0.0f;

	return value;
}

void BotNeuralNet::ClearMemory()
{
	memset(allMemory, 0, sizeof(allMemory));
}


BotNeuralNet::BotNeuralNet()
{
	repeat(NumInputNeurons)
		allNeurons[NeuronInputLayerIndex][i].type = input;

	repeat(NumOutputNeurons)
		allNeurons[NeuronOutputLayerIndex][i].type = output;

	for (uint nx = 0; nx < NumNeuronLayers; ++nx)
	{
		for (uint ny = 0; ny < neuronsInLayer[nx]; ++ny)
		{
			allNeurons[nx][ny].layer = nx;
		}
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

void BotNeuralNet::ClearValues()
{
	memset(allValues, 0, sizeof(allValues));
}

void BotNeuralNet::Process(BrainInput input)
{
	//Clear all neuron values
	ClearValues();

	//Copy input data
	repeat(NumInputNeurons)
	{
		allValues[NeuronInputLayerIndex][i] = input.fields[i];
	}

	float* m;
	Neuron* n;
	float tmpVal;


	//Parse input layer
	for (uint yi = 0; yi < NumInputNeurons; ++yi)
	{
		n = &allNeurons[NeuronInputLayerIndex][yi];

		tmpVal = allValues[NeuronInputLayerIndex][yi] + n->bias;		

		NeuronConnection* tmpC;

		for (uint i = 0; i < n->numConnections; ++i)
		{
			tmpC = &n->allConnections[i];

			allValues[tmpC->dest_layer][tmpC->dest_neuron] += tmpVal * tmpC->weight;
		}
	}

	//Parse hidden layers
	for (uint xi = 1; xi < NumNeuronLayers-1; ++xi)
	{
		for (uint yi = 0; yi < NumHiddenNeurons; ++yi)
		{
			m = &allMemory[xi][yi];
			n = &allNeurons[xi][yi];
			tmpVal = allValues[xi][yi] + n->bias;			

			//Skip calculation if neuron has no further connections
			if (n->numConnections == 0)
				continue;

			switch (n->type)
			{
			case basic:
				tmpVal = ActivationSimple(tmpVal);
				//tmpVal = Linear(tmpVal);
				break;

			case random:
				tmpVal = (RandomVal(1000) <= ((int)(tmpVal * 1000.0f))) ? 1.0f : 0.0f;
				break;

			case radialbasis:
				tmpVal = RadialBasisActivation(tmpVal);
				break;

			case memory:

				if (tmpVal < -1000)
				{
					//Wipe memory
					*m = 0;
				}
				else if (tmpVal > 1000)
				{
					//Write in memory
					*m = 1000;
				}

				tmpVal = *m;

				break;

			}

			//Feed forward
			NeuronConnection* tmpC;

			for (uint i = 0; i < n->numConnections; ++i)
			{
				tmpC = &n->allConnections[i];

				allValues[tmpC->dest_layer][tmpC->dest_neuron] += tmpVal * tmpC->weight;
			}
		}
	}


	//Output layer

	//Rotation
	repeat(2)
	{
		n = &allNeurons[NeuronOutputLayerIndex][i];
		tmpVal = allValues[NeuronOutputLayerIndex][i] + n->bias;		

		tmpVal = PlusMinusActivation(tmpVal);
	}

	//All the rest
	for (uint oi = 2; oi < NumOutputNeurons; ++oi)
	{
		n = &allNeurons[NeuronOutputLayerIndex][oi];
		tmpVal = allValues[NeuronOutputLayerIndex][oi] + n->bias;

		tmpVal = ActivationSimple(tmpVal);
	}
}



void BotNeuralNet::Mutate()
{
	uint randomNeuronIndex = RandomVal(NumNeuronLayers * NumNeuronsInLayerMax);
	uint counter = 0;
	Neuron* n;
	uint roll;

	for (uint xi = 0; xi < NumNeuronLayers; ++xi)
	{
		for (uint yi = 0; yi < neuronsInLayer[xi]; ++yi)
		{
			if (counter++ == randomNeuronIndex)
			{
				n = &allNeurons[xi][yi];
				roll = RandomVal(101);
								
				if(roll <= 5)
				{
					//5% - set neuron to random
					n->SetRandom();
				}
				else if (roll <= 7)
				{
					//2% - clear neuron
					n->mutate_DeleteNeuron();
				}
				else if (roll <= 12)
				{
					//5% - change neuron type
					n->mutate_ChangeType();
				}
				else if (roll <= 30)
				{
					//18% - change bias
					n->mutate_ChangeBias();
				}
				else if (roll <= 35)
				{
					//5% - 1 connection jump to next layer 
					//TODO
					if (n->numConnections > 0)
					{
						uint connection = n->GetRandomConnectionIndex();

						byte lx = n->allConnections[connection].dest_layer;
						byte nx = n->allConnections[connection].dest_neuron;

						Neuron* n2 = &allNeurons[lx][nx];

						if (n2->numConnections > 0)
						{
							uint connection_dest = n2->GetRandomConnectionIndex();

							n->allConnections[connection].dest_layer = n2->allConnections[connection_dest].dest_layer;
							n->allConnections[connection].dest_neuron = n2->allConnections[connection_dest].dest_neuron;
						}
						else
						{
							goto def;
						}
					}
					else
					{
						goto def;
					}
				}
				else
				{
				def:
					//change 1 connection
					if (n->numConnections == 0)
						return;

					uint connection = n->GetRandomConnectionIndex();
					NeuronConnection* c = &n->allConnections[connection];
					byte d_layer = c->dest_layer;

					if ((roll <= 85) and ((d_layer - xi) > 1))
					{
						//Split connection	
						for (int i = xi + 1; i < d_layer; ++i)
						{
							//Find unused neuron
							for (int b = 0; b < NumHiddenNeurons; ++b)
							{
								Neuron& n2 = allNeurons[i][b];

								if (n2.IsInactive())
								{
									//Copy neuron connection
									n2.bias = 0.0f;
									n2.AddConnection(c);

									//Retarget old connection
									n->RemoveConnection(connection);

									c->weight = 1.0f;
									c->dest_layer = i;
									c->dest_neuron = b;

									//We are done
									return;
								}
							}
						}
					}
					else
					{
						//Other mutations
						n->mutate_ChangeConnection(connection);
					}
				}
			}
		}
	}
}


void BotNeuralNet::Optimize()
{
	Neuron* n;
	
	for (uint x = NeuronOutputLayerIndex - 1; x > 0; --x)
	{
		for (uint y = 0; y < neuronsInLayer[x]; ++y)
		{
			n = &allNeurons[x][y];

			//If neuron is a dead-end
			if (n->numConnections == 0)
			{
				//Clear bias
				n->SetZero();

				//Clear all connections to this neuron in prev. layers
				for (uint cx = 0; cx < x; ++cx)
				{
					for (uint cy = 0; cy < NumNeuronsInLayerMax; ++cy)
					{
						int ind = allNeurons[cx][cy].IsConnected(x, y);

						if (ind != -1)
						{
							allNeurons[cx][cy].RemoveConnection(ind);
						}
					}
				}
			}
		}
	}
}


void BotNeuralNet::Randomize()
{
	//Input layer
	for (uint y = 0; y < NumInputNeurons; ++y)
	{
		allNeurons[NeuronInputLayerIndex][y].SetRandomConnections();
		allNeurons[NeuronInputLayerIndex][y].SetRandomBias();
	}

	//Hidden layers
	for (uint xi = 1; xi < NumNeuronLayers - 1; ++xi)
	{
		for (uint yi = 0; yi < NumHiddenNeurons; ++yi)
		{
			allNeurons[xi][yi].SetRandom();
		}
	}

	//Output layer
	for (uint y = 0; y < NumOutputNeurons; ++y)
	{
		allNeurons[NeuronOutputLayerIndex][y].SetRandomBias();
	}
}


BrainOutput BotNeuralNet::GetOutput()
{
	BrainOutput toRet;

	repeat(NumNeuronsInLayerMax)
		toRet.fields[i] = static_cast<int>(allValues[NeuronOutputLayerIndex][i]);

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
