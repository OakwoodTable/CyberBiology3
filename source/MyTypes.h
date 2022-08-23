#pragma once

//System headers
#include "Systems.h"


typedef unsigned int uint;
typedef unsigned char byte;


struct Point
{
	int x, y;

	Point(int X, int Y) :x(X), y(Y) {};

	Point():x(-1), y(-1) {};

	void Shift(int X, int Y)
	{
		x += X;
		y += Y;
	}

	void Set(int X, int Y)
	{
		x = X;
		y = Y;
	}
};

struct Rect : public SDL_Rect
{
	bool IsInBounds(Point p)
	{
		return ((p.x >= x) && (p.y >= y) && (p.x <= x + w) && (p.y <= y + h));
	}
};



//Get random number (0 to max-1)
#define RandomVal(max) (rand()%(max))

//Roll for probability (chance in percents)
#define RandomPercent(val) ((rand()%1000)>=(1000 - (val * 10)))

//Roll for probability (chance in 1/10 percent)
#define RandomPercentX10(val) ((rand()%1000)>=(1000 - (val)))

//Simple cycle
#define repeat(times) for(int i=0;i<times;++i)
