#pragma once
//#pragma message("	Organics_h")


#include "Field.h"


class Organics :public Object
{

public:

	bool doneFalling = false;


	int tick() override;

	void draw() override;
	void drawEnergy() override;


	Organics(int, int, int);

};

