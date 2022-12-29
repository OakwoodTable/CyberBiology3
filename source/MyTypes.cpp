
#include "MyTypes.h"



void Color::operator/=(int d)
{
	r /= d;
	g /= d;
	b /= d;
}

void Color::operator+=(Color c)
{
	//Check if color component is out of bounds [0...255]
	auto limit = [&](int* n)
	{
		if (*n < 0)
			*n = 0;
		else if (*n > 255)
			*n = 255;
	};

	r += c.r;
	g += c.g;
	b += c.b;

	limit(&r);
	limit(&g);
	limit(&b);
}

void Color::SetRandom()
{
	r = RandomVal(256);
	g = RandomVal(256);
	b = RandomVal(256);
}

void Color::RandomChange(int str)
{
	auto change = [&](int* val)
	{
		*val += (RandomVal((2 * str) + 1) - str);

		if (*val > 255)
			*val = 255;
		else if (*val < 0)
			*val = 0;
	};

	change(&r);
	change(&g);
	change(&b);
}

int Color::ToInt()
{
	intchar un;

	un.c[0] = r;
	un.c[1] = g;
	un.c[2] = b;
	un.c[3] = 0;

	return un.i;
}

void Color::FromInt(int data)
{
	intchar un;

	un.i = data;

	r = un.c[0];
	g = un.c[1];
	b = un.c[2];
}

Color::Color(int data)
{
	FromInt(data);
}

Color::Color() {}

Point::Point(int X, int Y) :x(X), y(Y) {}

Point::Point() :x(-1), y(-1) {}

void Point::Shift(int X, int Y) 
{
	x += X;
	y += Y;
}

void Point::Set(int X, int Y) 
{
	x = X;
	y = Y;
}

bool Rect::IsInBounds(Point p)
{
	return ((p.x >= x) && (p.y >= y) && (p.x <= x + w) && (p.y <= y + h));
}

float RandomFloatInRange(float min, float max) 
{ 
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min))); 
}
