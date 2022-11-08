#pragma once
//#pragma message("	Rock_h")


#include "Field.h"


class Rock :public Object
{

public:

	void draw() override; 

	Rock(int X, int Y);

};