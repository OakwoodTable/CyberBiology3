#pragma once
#include "Object.h"



//Apple class
class Apple :public Object
{

public:

	int energy;


	//Draw
	void draw() override;

	//Draw energy
	//void drawEnergy();

	//New apple
	Apple(int, int, int = 0);

};