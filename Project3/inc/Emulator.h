#pragma once
class Emulator
{
private:

public:
	Emulator() {}
	~Emulator() {}

	void init(std::string bios);
	void reset();
};

extern Emulator emu;

