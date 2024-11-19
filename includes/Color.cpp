#include "Color.h"

Color::Color(float red, float green, float blue, float alpha)
{
	values[0] = red;
	values[1] = green;
	values[2] = blue;
	values[3] = alpha;
}

void Color::Set(float red, float green, float blue, float alpha)
{
	values[0] = red;
	values[1] = green;
	values[2] = blue;

	if (alpha != -1)
		values[3] = alpha;
}

float Color::R()
{
	return values[0];
}

float Color::G()
{
	return values[1];
}

float Color::B()
{
	return values[2];
}

float Color::A()
{
	return values[3];
}

Color::operator float* () const
{
	return (float*)values;
}
