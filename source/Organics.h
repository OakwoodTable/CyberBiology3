#pragma once
#include "Object.h"



//Organics class
class Organics :public Object
{

public:

	bool doneFalling = false;
	int energy;


	//Draw
	void draw() override;

	//Draw energy
	void drawEnergy();

	//New organic waste
	Organics(int, int, int);

};

