
#include "Utils.h"



void Color::operator/=(int d)
{
	repeat(3)
		c[i] /= d;
}

void Color::operator+=(Color C)
{
	unsigned short tmpVal;

	repeat(3)
	{
		tmpVal = c[i];

		tmpVal += C.c[i];

		if (tmpVal > 255)
			tmpVal = 255;

		c[i] = static_cast<byte>(tmpVal);
	}
}

void Color::SetRandom()
{
	#ifdef PresetRandomColors
	{
		uint i = RandomVal(21);

		c[0] = presetColors[i][0];
		c[1] = presetColors[i][1];
		c[2] = presetColors[i][2];
	}
	#else
	{
		c[0] = RandomVal(256);
		c[1] = RandomVal(256);
		c[2] = RandomVal(256);
	}
	#endif

	change_vector[0] = RandomVal(256);
	change_vector[1] = RandomVal(256);
	change_vector[2] = RandomVal(256);

	NormalizeChangeVector();
}

void Color::RandomChange(const int strMin, const int strMax)
{
	if (RandomPercent(5))
	{
		change_vector[0] += RandomVal(20);
		change_vector[1] += RandomVal(20);
		change_vector[2] += RandomVal(20);

		NormalizeChangeVector();
	}

	repeat(3)
	{
		short& s = c[i];

		s += change_vector[i];

		if (s < 0)
		{
			s *= -1;
			change_vector[i] *= -1;
		}
		else if(s > 255)
		{
			s -= (s - 255) * 2;
			change_vector[i] *= -1;
		}
	}
}

void Color::NormalizeChangeVector()
{
	float magnitude = sqrt(
		(change_vector[0] * change_vector[0]) + 
		(change_vector[1] * change_vector[1]) + 
		(change_vector[2] * change_vector[2])) 
		/ 18.0f; // A little correction
	 
	repeat(3)
	{
		change_vector[i] /= magnitude;
	}
}

void Color::Set(int r, int g, int b)
{
	c[0] = r;
	c[1] = g;
	c[2] = b;
}

Color Color::GetRandomColor()
{
	Color toRet;

	toRet.SetRandom();

	return toRet;
}

Point::Point(int X, int Y) :x(X), y(Y) 
{}

Point::Point() :x(-1), y(-1) 
{}

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

Point Point::operator+(Point toAdd)
{
	return {x + toAdd.x, y + toAdd.y};
}

bool Rect::IsInBounds(Point p)
{
	return ((p.x >= x) && (p.y >= y) && (p.x <= x + w) && (p.y <= y + h));
}

int RandomValRange(int min, int max)
{
	return min + (rand() % (max - min));
}

float RandomFloatInRange(float min, float max)
{ 
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min))); 
}
