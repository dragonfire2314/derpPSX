#pragma once
class HardwareIO
{
private:

public:
	HardwareIO() {}
	~HardwareIO() {}

	uw* dataPointerForGPU_Hack = 0;
	uw dataForGPU_Hack = 0;

	uw i_stat = 0;
	uw i_mask = 0;

	uw read(uw address);
	void write(uw data, uw address);
};

extern HardwareIO io;