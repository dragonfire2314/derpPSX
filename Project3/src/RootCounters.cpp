#include "Global.h"

RootCounters rootCounters;

void RootCounters::tick()
{
	eightClock++;
	if (eightClock == 8) {
		eight = true;
	}
	for (int i = 0; i < 3; i++) 
	{
		//Tick counter
		ub clock_source = ((counters[0].mode & 0x300) >> 8);
		ub irqT   = ((counters[0].mode & 0x10) >> 4);
		ub irqMax = ((counters[0].mode & 0x20) >> 5);
		ub enable = ((counters[0].mode & 0x01));
		//Check if counter is enabled
		if (enable == 1) { return; }
		if (clock_source == 0 || clock_source == 2) { //System clock
			counters[0].count++;
		}
		else if (clock_source == 1 || clock_source == 3) { //Other clock
			switch (i) {
			case 0:
				//Dot clock
				break;
			case 1:
				//Hblank
				break;
			case 2:
				//1/8 Clock
				
				if (eight) counters[i].count++;
				break;
			}
		}
		//Trigger IRQ
		if (irqT == 1) {
			if (counters[i].count == counters[i].target) std::cout << std::hex << "IRQ Triggered" << std::endl;//Trigger IRQ
		}
		if (irqMax == 1) {
			if (counters[i].count >= 0xFFFF) std::cout << std::hex << "IRQ Triggered" << std::endl;//Trigger IRQ
		}
	}

	
	//Check if
	
	eight = false;
}

void RootCounters::setCount(uw counter, uw data)
{
	counters[counter].count = data;
}

void RootCounters::setMode(uw counter, uw data)
{
	counters[counter].mode = data;
}

void RootCounters::setTarget(uw counter, uw data)
{
	counters[counter].target = data;
}
