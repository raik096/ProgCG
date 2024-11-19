#pragma once

struct Color
{
	//float* values;
	float values[4];

	Color(float red = 1, float green = 1, float blue = 1, float alpha = 1);

	void Set(float red, float green, float blue, float alpha = -1);

	float R();
	float G();
	float B();
	float A();

	operator float* () const;
};