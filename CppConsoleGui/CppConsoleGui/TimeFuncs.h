#pragma once

/**
 * @author Marley Arns
 * @version 1.0
 * @github: github.com/ExecutableMarley
 */

#include <chrono>

// Custom
using tpoint = std::chrono::steady_clock::time_point;


struct Intervall
{
	tpoint base;
	int intervall;

	Intervall(int x)
	{
		intervall = x;
	}
	Intervall()
	{
		intervall = 0;
	}

	bool check()
	{
		return check(intervall);
	}
	bool check(int time)
	{
		if (t_dif(t_now(), base) >= time)
		{
			base = t_now();
			return true;
		}
		return false;
	}

	void set(int x)
	{
		intervall = x;
	}

	tpoint t_now()
	{
		return std::chrono::steady_clock::now();
	}

	long long t_dif(tpoint base, tpoint a)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(base - a).count();
	}
};
