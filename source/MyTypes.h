#pragma once

//System headers
#include "Systems.h"


typedef unsigned int uint;
typedef std::uint8_t byte;

using std::thread;
using std::mutex;
using std::condition_variable_any;

using std::string;
using std::vector;

using std::min;
using std::to_string;


struct Color
{
	byte c[3];

	void operator/=(int);
	void operator+=(Color);

	void Set(int r, int g, int b);
	void SetRandom();
	void RandomChange(const int str);


	static Color GetRandomColor();
};


struct Point
{
	int x, y;

	Point(int X, int Y);
	Point();

	void Shift(int X, int Y);	
	void Set(int X, int Y);

	Point operator+(Point toAdd);	
};

struct Rect final: public SDL_Rect
{
	bool IsInBounds(Point p);	
};


//TODO
//Get random number (0 to max-1)
#define RandomVal(max) (rand()%(max))
#define RandomValRange(min, max) (RandomVal(max-min) + min)
float RandomFloatInRange(float min, float max);

//Roll (chance in percents)
#define RandomPercent(val) ((rand()%1000)>=(1000 - (val * 10)))

//Roll (chance in 1/10 percent)
#define RandomPercentX10(val) ((rand()%1000)>=(1000 - (val)))

//Simple loop
#define repeat(times) for(int i=0;i<times;++i)




//High precision tick counter
typedef std::chrono::steady_clock Clock;
typedef Clock::time_point TimePoint;
typedef std::chrono::duration<int, std::milli> Duration;

#define TimeMSBetween(t2, t1) duration_cast<Duration>(t2 - t1).count() 