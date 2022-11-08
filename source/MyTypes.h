#pragma once

//System headers
#include "Systems.h"


typedef unsigned int uint;
typedef unsigned char byte;


struct Color
{
	int r=0, g=0, b=0;

	void operator/=(int);
	void operator+=(Color);

	void SetRandom();

	void RandomChange(int str);

	int ToInt();
	void FromInt(int);

	Color(int);
	Color();

private:

	union intchar
	{
		int i;
		char c[4];
	};
};


struct Point
{
	int x, y;


	Point(int X, int Y);
	Point();

	void Shift(int X, int Y);	
	void Set(int X, int Y);
	
};

struct Rect final: public SDL_Rect
{
	bool IsInBounds(Point p);	
};




//Get random number (0 to max-1)
#define RandomVal(max) (rand()%(max))

//Get random float in given range
float RandomFloatInRange(float min, float max);

//Roll (chance in percents)
#define RandomPercent(val) ((rand()%1000)>=(1000 - (val * 10)))

//Roll (chance in 1/10 percent)
#define RandomPercentX10(val) ((rand()%1000)>=(1000 - (val)))

//Simple loop
#define repeat(times) for(int i=0;i<times;++i)
