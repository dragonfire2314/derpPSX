#include "Global.h"

BiosCalls bios;

BiosCalls::BiosCalls()
{
}


BiosCalls::~BiosCalls()
{
}

void BiosCalls::A0_Call()
{
	switch (cpu.base[9])
	{
	case 0x00:
		printf("TEST: fileName: %x\n", cpu.base[4]);
		printf("TEST: mode: %x\n", cpu.base[5]);

		printf("A0 Call 0x00, FileOpen(filename, accessmode)\n");

		system("pause");
		break;
	default:
		printf("A0 Call not implimented %x\n", cpu.base[9]);
	}
}

void BiosCalls::B0_Call()
{
	switch (cpu.base[9])
	{
	default:
		printf("B0 Call not implimented %x\n", cpu.base[9]);
	}
}

void BiosCalls::C0_Call()
{
	switch (cpu.base[9])
	{
	default:
		printf("B0 Call not implimented %x\n", cpu.base[9]);
	}
}

//Returns a ub file handle
void FileOpen(char* fileName, uw mode) 
{

}