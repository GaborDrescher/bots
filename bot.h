#pragma once

#include "real.h"
#include "vec.h"

class Bot
{
	public:
	Bot *next;
	vec pos;
	vec vel;
	vec force;
	real invmass;
	real radius;
};

