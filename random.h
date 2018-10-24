#pragma once

#include "stdtypes.h"

class Random
{
	private:
	Random(const Random &rand); //do not copy
	uint64_t status;

	public:
	Random() : status(42) {}

	uint64_t get()
	{
		status = ((uint64_t)6364136223846793005) * status + ((uint64_t)1442695040888963407);
		return status;
	}

};


