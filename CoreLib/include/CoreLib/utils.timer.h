#pragma once

#include "globals.h"

namespace
{
	using namespace std;
	using namespace std::chrono;
}

class SuperTimer
{
public:

	void SetStart () { P1 = high_resolution_clock::now(); }
	void SetEnd   () { P2 = high_resolution_clock::now(); }

	constexpr
	int64_t GetFrequency() const { return duration_cast<nanoseconds>(seconds(1)).count(); } //
	int64_t GetTimeTicks() const { return                            (P2 - P1).count(); }   //
	int64_t GetTimeNano () const { return duration_cast<nanoseconds> (P2 - P1).count(); }   // 1'000'000'000 (per sec)
	int64_t GetTimeMicro() const { return duration_cast<microseconds>(P2 - P1).count(); }   // 1'000'000     (per sec)
	int64_t GetTimeMili () const { return duration_cast<milliseconds>(P2 - P1).count(); }   // 1'000         (per sec)
	double  GetTimeReal () const { return (double)GetTimeTicks() / GetFrequency();      }   // 1.0           (per sec)

	// Ñyclical count without measurement errors (1.0 per second)
	double GetRoundTime()
	{
		// if start point not setted yet
		if (P1.time_since_epoch().count() == 0)
		P1 = chrono::high_resolution_clock::now();
		P2 = chrono::high_resolution_clock::now();
		const auto duration = static_cast<double>((P2 - P1).count());
		P1 = P2;

		return duration / GetFrequency();
	}

	bool InEach(const double period, const uint32_t maxCollect = 2)
	{
		collected += GetRoundTime();

		if (collected < period)
		{
			return false;
		}
		else
		{
			if (collected > period * maxCollect)
				collected = period * maxCollect;

			collected -= period;
			return true;
		}
	}

private:
	chrono::time_point<chrono::high_resolution_clock> P1;
	chrono::time_point<chrono::high_resolution_clock> P2;

	double collected = 0.;
};