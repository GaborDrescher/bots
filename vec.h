#pragma once

#include "real.h"
#include "botmath.h"

class vec
{
	public:
	real x;
	real y;

	vec() : x(0), y(0) {}
	vec(real x_, real y_) : x(x_), y(y_) {}

	vec operator+(const vec &other) const
	{
		return vec(x+other.x, y+other.y);
	}
	vec operator-(const vec &other) const
	{
		return vec(x-other.x, y-other.y);
	}
	real operator*(const vec &other) const
	{
		return x*other.x + y*other.y;
	}
	vec operator*(const real other) const
	{
		return vec(x*other, y*other);
	}
	vec operator/(const real other) const
	{
		real inv = real(1) / other;
		return vec(x*inv, y*inv);
	}
	real lengthSq() const
	{
		return x*x+y*y;
	}
	real length() const
	{
		return sqrt(lengthSq());
	}
};

