#include "Global.h"

Emulator emu;

void Emulator::init(std::string bios)
{
	//Load bios
	std::ifstream Openfile("SCPH1001.bin", std::ios::binary | std::ios::in);
	uint32_t size;
	if (Openfile) {}
	else { std::cout << "Bios not found." << std::endl; }
	size = 524288;

	mem.rom.ptr = new uint8_t[size];
	Openfile.read((char*)mem.rom.ptr, size);

	/*std::ifstream Openfile("roms/SCUS_941.88", std::ios::binary | std::ios::in);
	uint32_t size;
	if (Openfile) {}
	else { std::cout << "Bios not found." << std::endl; }
	size = 0x94800;

	

	mem.rom.ptr = new uint8_t[size];
	Openfile.read((char*)mem.ram.ptr + 0x10000, size);*/

	//init gpu
	gpu.gpu_init();
}

void Emulator::reset()
{
	cpu.reset();
	mem.reset();
}