#pragma once
class RootCounters
{
private:
	struct Counter 
	{
		uw count;
		uw target;
		uw mode;
	};

	Counter counters[4] = { 0 };

	uw eightClock = 0;
	bool eight = false;

public:
	RootCounters() {}
	~RootCounters() {}

	void tick();
	void setCount(uw counter, uw data);
	void setMode(uw counter, uw data);
	void setTarget(uw counter, uw data);
};

extern RootCounters rootCounters;

