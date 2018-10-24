#pragma once

#include <time.h>
#include "stdtypes.h"
#include <sys/time.h>

class Time
{
	public:
	static void sleep(int ms)
	{
		struct timespec reqtime;
		struct timespec remtime;

		reqtime.tv_sec = ms / 1000;
		reqtime.tv_nsec = (ms % 1000) * 1000 * 1000;
		remtime.tv_sec = 0;
		remtime.tv_nsec = 0;

		struct timespec *current = &reqtime;
		struct timespec *rem = &remtime;

		while(nanosleep(current, rem) != 0)
		{
			struct timespec *tmp = current;
			current = rem;
			rem = tmp;
		}
	}

	static uint64_t getMillis()
	{
		struct timeval tp;
		gettimeofday(&tp, 0);

		return ((uint64_t)tp.tv_sec) * ((uint64_t)1000) + ((uint64_t)tp.tv_usec) / ((uint64_t)1000);
	}
};


