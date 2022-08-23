#pragma once

#include "Object.h"



//Rock class
class Rock :public Object
{

public:

	//Draw rock
	void draw() override; 

	//New rock
	Rock(int X, int Y);

};