#pragma once

/**
 * @author Marley Arns
 * @version 1.0
 * @github: github.com/ExecutableMarley
 */

#include <chrono>

// Custom name
using tpoint = std::chrono::steady_clock::time_point;

class Intervall
{
private:
	tpoint base;
	int intervall;
public:

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
private:
	tpoint t_now()
	{
		return std::chrono::steady_clock::now();
	}

	long long t_dif(tpoint base, tpoint a)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>
			(base - a).count();
	}
};
