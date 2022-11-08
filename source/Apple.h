#pragma once
//#pragma message("	Apple_h")


#include "Field.h"


class Apple :public Object
{

public:

	void draw() override;

	Apple(int X, int Y, int Energy = 0);

};