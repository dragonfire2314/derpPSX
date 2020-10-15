#pragma once
#include <string>
#include <map>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <vector>
#include <queue>

class mips3000a
{
private:
	friend class Bios;

	struct instruction_data
	{
		std::string name;
		uint32_t location;
		uint8_t opcode;

		instruction_data(std::string _name, uint32_t _location, uint8_t _opcode) {
			name = _name;
			location = _location;
			opcode = _opcode;
		}
	};

	std::deque<instruction_data> past_instructions;
	std::string instruction_names[64];

	//Breakpoints
	uint32_t breakpoint = 0xFFFFFFFF;
	uint32_t r2Breakpoint = 0xFFFFFFFF;
	bool isStep = false;

	//Internals
	uint32_t IP = 0xbfc00000;
	uint32_t Instruction = 0;
	uint8_t opcode = 0;

	uint8_t rs_new, rt_new, rd_new, shamt, funct;
	uint16_t immediate;
	uint32_t target_address;

	uint32_t prevInstruction = 0;

	//Register file
	uint32_t register_file[32];

	//Opcode functions
	void inline lui();
	void inline ori();
	void inline sw();
	void inline bne();
	void inline addi();
	void inline lw();
	void inline sh();
	void inline sb();
	void inline andi();
	void inline lb();
	void inline beqz();
	void inline blez();
	void inline bgtz();
	void inline jal();
	void inline jalr();
	void inline slti();
	void inline lhu();
	void inline sltiu();
	void inline jr();
	void inline bgez();

	void inline coprocessor_call();
	void systemCall(uint32_t code);

	uint32_t cycle_counter = 0;
	uint32_t counter_max = 0xFFFFFFFF;
public:
	mips3000a();
	~mips3000a();

	void cycle();

	//Takes an address with a 0x0cf00000 offfset for now
	void setBreakPoint(uint32_t address);
	void setR2BreakPoint(uint32_t value) { r2Breakpoint = value; }
	void setCycleBreak(uint32_t value) { counter_max = value; }

	void alu();

	bool getStep() { return isStep; }

	void setIP(uint32_t value) { IP = value; }
};

extern mips3000a mips;
