#include "Global.h"

#define LOG_F(x)

mips3000a mips;

void inline mips3000a::lui()
{
	register_file[rt_new] = (immediate << 16);
}

void inline mips3000a::ori()
{
	register_file[rt_new] = (register_file[rs_new] & 0xFFFF0000) | (immediate | (register_file[rs_new] & 0xFFFF));
}

void inline mips3000a::sh()
{
	if ((immediate & 0x8000) == 0x8000)
	{
		mem.write<uw>(register_file[rt_new] & 0xFFFF, (((0xFFFF << 16) | immediate) + (register_file[rs_new])));
	}
	else
	{
		mem.write<uw>(register_file[rt_new] & 0xFFFF, immediate + register_file[rs_new]);
	}
}

void inline mips3000a::sb()
{
	if ((immediate & 0x8000) == 0x8000)
	{
		mem.write<uw>(register_file[rt_new] & 0xFF, (((0xFFFF << 16) | immediate) + (register_file[rs_new])));
	}
	else
	{
		mem.write<uw>(register_file[rt_new] & 0xFF, immediate + register_file[rs_new]);
	}
}

void inline mips3000a::sw() 
{
	if ((immediate & 0x8000) == 0x8000) 
	{
		mem.write<uw>(register_file[rt_new], (((0xFFFF << 16) | immediate) + register_file[rs_new]));
	}
	else 
	{
		mem.write<uw>(register_file[rt_new], immediate + register_file[rs_new]);
	}
}

void inline mips3000a::lw()
{
	if ((immediate & 0x8000) == 0x8000)
	{
		register_file[rt_new] = mem.read<uw>(((0xFFFF << 16) | immediate) + register_file[rs_new]);
	}
	else
	{
		register_file[rt_new] = mem.read<uw>(immediate + register_file[rs_new]);
	}
}

void inline mips3000a::lhu()
{
	if ((immediate & 0x8000) == 0x8000)
	{
		register_file[rt_new] = mem.read<uw>(((0xFFFF << 16) | immediate) + register_file[rs_new]) & 0xFFFF;
	}
	else
	{
		register_file[rt_new] = mem.read<uw>(immediate + register_file[rs_new]) & 0xFFFF;
	}
}

void inline mips3000a::lb()
{
	if ((immediate & 0x8000) == 0x8000)
	{
		register_file[rt_new] = mem.read<uw>(((0xFFFF << 16) | immediate) + register_file[rs_new]) & 0xFF;
	}
	else
	{
		register_file[rt_new] = mem.read<uw>(immediate + register_file[rs_new]) & 0xFF;
	}
}

void inline mips3000a::addi()
{
	if ((immediate & 0x8000) == 0x8000)
	{
		register_file[rt_new] = ((0xFFFF << 16) | immediate) + register_file[rs_new];
	}
	else
	{
		register_file[rt_new] = immediate + register_file[rs_new];
	}
}

void inline mips3000a::andi()
{
	if ((immediate & 0x8000) == 0x8000)
	{
		register_file[rt_new] = ((0xFFFF << 16) | immediate) & register_file[rs_new];
	}
	else
	{
		register_file[rt_new] = immediate & register_file[rs_new];
	}
}

void inline mips3000a::slti()
{
	if ((immediate & 0x8000) == 0x8000)
	{
		if (register_file[rs_new] < (((0xFFFF << 16) | immediate)))
			register_file[rt_new] = 1;
		else
			register_file[rt_new] = 0;
	}
	else
	{
		if (register_file[rs_new] < immediate)
			register_file[rt_new] = 1;
		else
			register_file[rt_new] = 0;
	}
}

void inline mips3000a::sltiu()
{
	if ((immediate & 0x8000) == 0x8000)
	{
		if (register_file[rs_new] < (((0xFFFF << 16) | immediate)))
			register_file[rt_new] = 1;
		else
			register_file[rt_new] = 0;
	}
	else
	{
		if (register_file[rs_new] < immediate)
			register_file[rt_new] = 1;
		else
			register_file[rt_new] = 0;
	}
}

void inline mips3000a::bne()
{
	uint8_t rs_new_copy = rs_new;
	uint8_t rt_new_copy = rt_new;
	uint16_t imm_copy = immediate;

	cycle();

	IP -= 4;

	uint32_t temp;
	if ((imm_copy & 0x8000) == 0x8000)
	{
		temp = ((0xFFFF << 18) | (imm_copy << 2));
	}
	else
	{
		temp = imm_copy << 2;
	}

	if (register_file[rs_new_copy] != register_file[rt_new_copy]) {
		IP = (IP + temp);
	}
	else IP += 4;
}

void inline mips3000a::beqz()
{
	uint8_t rs_new_copy = rs_new;
	uint8_t rt_new_copy = rt_new;
	uint16_t imm_copy = immediate;

	cycle();

	IP -= 4;

	uint32_t temp;
	if ((imm_copy & 0x8000) == 0x8000)
	{
		temp = ((0xFFFF << 18) | (imm_copy << 2));
	}
	else
	{
		temp = imm_copy << 2;
	}

	if (register_file[rs_new_copy] == 0) {
		IP = (IP + temp);
	}
	else IP += 4;
}

void inline mips3000a::blez()
{
	uint8_t rs_new_copy = rs_new;
	uint8_t rt_new_copy = rt_new;
	uint16_t imm_copy = immediate;


	cycle();

	IP -= 4;

	uint32_t temp;
	if ((imm_copy & 0x8000) == 0x8000)
	{
		temp = ((0xFFFF << 18) | (imm_copy << 2));
	}
	else
	{
		temp = imm_copy << 2;
	}

	LOG_F(INFO, "Instruction: BLEZ | Possible Jump: %x", (IP + temp));
	if (register_file[rs_new_copy] == 0 || (register_file[rs_new_copy] & 0x80000000) == 0x80000000) {
		IP = (IP + temp);
		LOG_F(WARNING, "JUMP TAKEN");
	}
	else IP += 4;
}

void inline mips3000a::bgez()
{
	uint8_t rs_new_copy = rs_new;
	uint8_t rt_new_copy = rt_new;
	uint16_t imm_copy = immediate;

	LOG_F(WARNING, "Branch on greater than or equal to zero called, executing next instruction");

	cycle();

	IP -= 4;

	uint32_t temp;
	if ((imm_copy & 0x8000) == 0x8000)
	{
		temp = ((0xFFFF << 18) | (imm_copy << 2));
	}
	else
	{
		temp = imm_copy << 2;
	}

	LOG_F(INFO, "Instruction: BLEZ | Possible Jump: %x", (IP + temp));
	if (register_file[rs_new_copy] == 0 || (register_file[rs_new_copy] & 0x80000000) == 0) {
		IP = (IP + temp);
		LOG_F(WARNING, "JUMP TAKEN");
	}
	else IP += 4;
}

void inline mips3000a::bgtz()
{
	uint8_t rs_new_copy = rs_new;
	uint8_t rt_new_copy = rt_new;
	uint16_t imm_copy = immediate;

	LOG_F(WARNING, "Branch on greater than zero called, executing next instruction");

	cycle();

	IP -= 4;

	uint32_t temp;
	if ((imm_copy & 0x8000) == 0x8000)
	{
		temp = ((0xFFFF << 18) | (imm_copy << 2));
	}
	else
	{
		temp = imm_copy << 2;
	}

	LOG_F(INFO, "Instruction: BGTZ | Possible Jump: %x", (IP + temp));
	if (register_file[rs_new_copy] != 0 && (register_file[rs_new_copy] & 0x80000000) == 0) {
		IP = (IP + temp);
		LOG_F(WARNING, "JUMP TAKEN");
	}
	else IP += 4;
}

void inline mips3000a::jal()
{
	uint32_t target_address_copy = target_address;

	cycle();

	register_file[31] = IP;
	IP = (IP & 0xF0000000) | (target_address_copy << 2);

	//Function bypasses
	//Printf
	if (IP == 0xbfc018e0) {
		//Print someting
		LOG_F(ERROR, "Printf was called from kernel code!");
		//Copy off message
		int index = register_file[4];
		char c;
		std::string str;
		c = mem.read<ub>(index);
		while (c)
		{
			str += c;
			index++;
			c = mem.read<ub>(index);
		}
		std::cout << str << std::endl;
	}
}

void inline mips3000a::jalr()
{
	uint8_t rs_new_copy = rs_new;
	uint8_t rd_new_copy = rd_new;
	if (rd_new_copy == 0) rd_new_copy = 31;

	cycle();

	register_file[rd_new_copy] = IP;
	IP = register_file[rs_new_copy];
}

void inline mips3000a::jr()
{
	uint8_t rs_new_copy = rs_new;

	cycle();

	IP = register_file[rs_new_copy];
}


mips3000a::mips3000a()
{

	//Name map
	instruction_names[0x00] = "ALU ";
	instruction_names[0x08] = "ADDI";
	instruction_names[0x09] = "ADIU";
	instruction_names[0x0C] = "ANDI";
	instruction_names[0x04] = "BEQ ";
	instruction_names[0x01] = "BGEZ";
	instruction_names[0x07] = "BGTZ";
	instruction_names[0x06] = "BLEZ";
	instruction_names[0x05] = "BNE ";
	instruction_names[0x02] = "J   ";
	instruction_names[0x03] = "JAL ";
	instruction_names[0x20] = "LB  ";
	instruction_names[0x24] = "LBU ";
	instruction_names[0x21] = "LH  ";
	instruction_names[0x25] = "LHU ";
	instruction_names[0x0F] = "LUI ";
	instruction_names[0x23] = "LW  ";
	instruction_names[0x0D] = "ORI ";
	instruction_names[0x28] = "SB  ";
	instruction_names[0x29] = "SH  ";
	//instruction_names.insert_new(std::pair<uint8_t, std::string>());
}

mips3000a::~mips3000a()
{

}

void mips3000a::cycle()
{
	cycle_counter++;
	//Check if IP is pointed at a bios call table
	if (IP == 0xa0 || IP == 0xb0 || IP == 0xc0) {
		//Call the bios function then set proform a jr ra instruction
		//bus_CallBiosFunction(IP, register_file[9]);
		//IP = register_file[31];

		//LOG_F();
	}
	//Check for breakpoint
	if (breakpoint == IP || cycle_counter == counter_max || IP == 0) {
		isStep = true;	
		LOG_F(ERROR, "Cycle Counter: %x", cycle_counter);
		system("pause");
	}
	//Fetch an opcode
	if ((IP >> 20) == 0xbfc) {
		instCache = (uw*)& mem.rom.ptr[addr & 0x000fffff];
	}
	else {
		instCache = (uw*)& mem.ram.ptr[addr & 0x00ffffff];
	}
	Instruction = mem.read<uw>(IP);
	prevInstruction = IP;
	IP += 4;
	if (Instruction == 0) {
		LOG_F(WARNING, "Instruction: NOP");
		instruction_data t("NOP", prevInstruction, opcode);
		past_instructions.push_back(t);
		if (past_instructions.size() > 100) past_instructions.pop_front();
		return; //Catch NOP
	}
	//Decode and register fetch
	opcode = (Instruction & 0xFC000000) >> 26;
	rs_new = (Instruction & 0x3E00000) >> 21;
	rt_new = (Instruction & 0x1F0000) >> 16;
	rd_new = (Instruction & 0xF800) >> 11;
	shamt = (Instruction & 0x7C0) >> 6;
	funct = (Instruction & 0x3F);
	immediate = (Instruction & 0xFFFF);
	target_address = (Instruction & 0x3FFFFFF);

	LOG_F(WARNING, "OPCODE: %X", opcode);

	//Save instruction
	instruction_data t(instruction_names[opcode], prevInstruction, opcode);
	past_instructions.push_back(t);
	if (past_instructions.size() > 100) past_instructions.pop_front();
	

	//Execute
	switch (opcode)
	{
	case 0x00: //Speacial instruction, ALU
		LOG_F(INFO, "Instruction: %s | rt_new: %x | rs_new: %x | rd_new: %x", "ALU", rt_new, rs_new, rd_new);
		alu();
		break;
	case 0x01: //BGEZ
		bgez();
		break;
	case 0x02: //J
		LOG_F(INFO, "Instruction: J | Jump Address: %x", (IP & 0xF0000000) | (target_address << 2));
		IP = (IP & 0xF0000000) | (target_address << 2);
		break;
	case 0x03: //JAL
		LOG_F(INFO, "Instruction: JAL | Jump Address: %x", (IP & 0xF0000000) | (target_address << 2));
		jal();
		break;
	case 0x04: //BEQZ
		beqz();
		break;
	case 0x05: //BNE
		bne();
		break;
	case 0x06: //BLEZ
		blez();
		break;
	case 0x07: //BGTZ
		bgtz();
	case 0x08: //ADDI
		LOG_F(INFO, "Instruction: ADDI | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		addi();
		break;
	case 0x09: //LI - shares opcode with ADDUI and its very confusing to me (going to impliment ADDUI for now)
		LOG_F(INFO, "Instruction: ADDUI | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		//register_file[rt_new] = register_file[rs_new] + immediate;
		addi();
		break;
	case 0x0a: //SLTI
		LOG_F(INFO, "Instruction: SLTI | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		slti();
		break;
	case 0x0b: //SLTIU
		LOG_F(INFO, "Instruction: SLTIU | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		sltiu();
		break;
	case 0x0C: //ANDI
		LOG_F(INFO, "Instruction: ANDI | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		andi();
		break;
	case 0x10: //MTCz or MFCz
		coprocessor_call();
		break;
	case 0x0F: //LUI
		LOG_F(INFO, "Instruction: LUI | rt_new: %x | Immediate: %x", rt_new, immediate);
		lui();
		break;
	case 0x0D: //ORI
		LOG_F(INFO, "Instruction: ORI | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		ori();
		break;
	case 0x20: //LB
		LOG_F(INFO, "Instruction: LB | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		lb();
		break;
	case 0x23: //LW
		LOG_F(INFO, "Instruction: LW | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		lw();
		break;
	case 0x24: //LBU
		lb();
		break;
	case 0x25: //LHU
		lhu();
		break;
	case 0x28: //SB
		LOG_F(INFO, "Instruction: SB | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		sb();
		break;
	case 0x29:
		LOG_F(INFO, "Instruction: SH | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		sh();
		break;
	case 0x2B: //SW
		LOG_F(INFO, "Instruction: SW | rt_new: %x | base: %x | Immediate: %x", rt_new, rs_new, immediate);
		sw();
		break;
	default:
		LOG_F(ERROR, "UNKNOWN OPCODE: %x | %x | %x", opcode, IP - 4, Instruction);
		isStep = true;
	}

	//Log all cpu info
	if (isStep) {
		LOG_F(INFO, "Instruction Pointer: %x", IP - 4);
		LOG_F(INFO, "Instruction: %X", Instruction);
		LOG_F(INFO, "--------------------------------------------------------------");
		LOG_F(INFO, "ZR: %x | AT: %x | V0: %x | V1: %x | A0: %x | A1: %x | A2: %x | A3: %x", register_file[0], register_file[1], register_file[2], register_file[3], register_file[4], register_file[5], register_file[6], register_file[7]);
		LOG_F(INFO, "T0: %x | T1: %x | T2: %x | T3: %x | T4: %x | T5: %x | T6: %x | T7: %x", register_file[8], register_file[9], register_file[10], register_file[11], register_file[12], register_file[13], register_file[14], register_file[15]);
		LOG_F(INFO, "S0: %x | S1: %x | S2: %x | S3: %x | S4: %x | S5: %x | S6: %x | S7: %x", register_file[16], register_file[17], register_file[18], register_file[19], register_file[20], register_file[21], register_file[22], register_file[23]);
		LOG_F(INFO, "T8: %x | T9: %x | K0: %x | K1: %x | GP: %x | SP: %x | FP: %x | RA: %x", register_file[24], register_file[25], register_file[26], register_file[27], register_file[28], register_file[29], register_file[30], register_file[31]);
		LOG_F(INFO, "--------------------------------------------------------------");
		LOG_F(ERROR, "Cycle Counter: %x", cycle_counter);

		system("pause");
	}


	//Set zero register to zero
	register_file[0] = 0;
}

void mips3000a::setBreakPoint(uint32_t address)
{
	breakpoint = address;
}

void mips3000a::coprocessor_call() 
{
	uint8_t cop_num = (opcode & 0x03);
	uint32_t cop_command = (Instruction & 0x1FFFFFF);
	switch(rs_new)
	{
	case 0x00: //MFCz
		//register_file[rt_new] = cops[cop_num]->MoveFromCoprocessor(rd_new);
		break;
	case 0x04: //MTCz
		//cops[cop_num]->MoveToCoprocessor(register_file[rt_new], rd_new);
		break;
	case 0x10: //COPz
		LOG_F(ERROR, "COP%x | IP: %x | Command: %x", cop_num, IP, cop_command);

		break;
	default:
		LOG_F(FATAL, "Command not found, Coprocessor? %x", rs_new);
	}
}

void mips3000a::systemCall(uint32_t code)
{
	LOG_F(ERROR, "System Call");
}

void mips3000a::alu()
{
	int32_t signed_rs_new;
	int32_t signed_rt_new;

	signed int temp;

	switch (funct) 
	{
	case 0x00: //SLL
		register_file[rd_new] = register_file[rt_new] << shamt;
		break;
	case 0x02: //SRL
		register_file[rd_new] = register_file[rt_new] >> shamt;
		break;
	case 0x03: //SRA
		temp = register_file[rt_new];
		temp >> shamt;
		register_file[rd_new] = temp;
		break;
	case 0x04: //SLLV
		register_file[rd_new] = register_file[rt_new] << register_file[rs_new];
		break;
	case 0x08: //JR
		jr();
		break;
	case 0x09: //JALR
		jalr();
		break;
	case 0x0C: //SYSCALL

		break;
	case 0x20: //ADD - Broken
		register_file[rd_new] = register_file[rs_new] + register_file[rt_new];
		break;
	case 0x21: //ADDU
		register_file[rd_new] = register_file[rs_new] + register_file[rt_new];
		break;
	case 0x23: //SUBU
		register_file[rd_new] = register_file[rs_new] - register_file[rt_new];
		break;
	case 0x24: //AND
		register_file[rd_new] = register_file[rs_new] & register_file[rt_new];
		break;
	case 0x25: //OR
		register_file[rd_new] = register_file[rs_new] | register_file[rt_new];
		break;
	case 0x0D: //BREAK
		LOG_F(ERROR, "Break point hit, nothing will happen properly");
		break;
	//case 0x1A: // DIV

		//break;
	case 0x2a: //SLT
		signed_rs_new = register_file[rs_new];
		signed_rt_new = register_file[rt_new];
		if (signed_rs_new < signed_rt_new) register_file[rd_new] = 1;
		else register_file[rd_new] = 0;
		break;
	case 0x2b: //SLTU
		if (register_file[rs_new] < register_file[rt_new]) register_file[rd_new] = 1;
		else register_file[rd_new] = 0;
		break;
	default:
		isStep = true;
		LOG_F(ERROR, "ALU funct code not implimented: %x", funct);
	}
}

















/* OLD CYCLE FUNCTION

void mips3000a::cycle()
{
	//HACK c0 42 05 80
	if (IP == 0x800546f0) { register_file[31] = 0x800542c0; return; }
	//Check for breakpoint
	if (breakpoint == IP || IP == 0) {
		isStep = true;
		LOG_F(ERROR, "Hit zero, from: %x", prevInstruction);
		system("pause");
	}
	if (r2Breakpoint == register_file[2]) isStep = true;
	//Fetch an opcode
	Instruction = mem.read<uw>(IP);
	prevInstruction = IP;
	IP += 4;
	if (Instruction == 0) {
		LOG_F(WARNING, "Instruction: NOP");
		return; //Catch NOP
	}
	//Decode and register fetch
	opcode = (Instruction & 0xFC000000) >> 26;
	rs_new = (Instruction & 0x3E00000) >> 21;
	rt_new = (Instruction & 0x1F0000) >> 16;
	rd_new = (Instruction & 0xF800) >> 11;
	shamt = (Instruction & 0x7C0) >> 6;
	funct = (Instruction & 0x3F);
	immediate = (Instruction & 0xFFFF);
	target_address = (Instruction & 0x3FFFFFF);

	LOG_F(WARNING, "OPCODE: %X", opcode);

	//Catch Bios calls and pass them to my custom bios
	if (opcode == 0 && funct == 0x08 && register_file[rs_new] == 0xA0) {
		LOG_F(ERROR, "BIOS FUNCTION CALLING");

		return;
	}

	//Save instruction
	instruction_data t(instruction_names[opcode], prevInstruction, opcode);
	past_instructions.push_back(t);
	if (past_instructions.size() > 1000) past_instructions.pop_front();

	//Execute
	switch (opcode)
	{
	case 0x00: //Speacial instruction, ALU
		LOG_F(INFO, "Instruction: %s | rt_new: %x | rs_new: %x | rd_new: %x", "ALU", rt_new, rs_new, rd_new);
		alu();
		break;
	case 0x02: //J
		LOG_F(INFO, "Instruction: J | Jump Address: %x", (IP & 0xF0000000) | (target_address << 2));
		IP = (IP & 0xF0000000) | (target_address << 2);
		break;
	case 0x03: //JAL
		LOG_F(INFO, "Instruction: JAL | Jump Address: %x", (IP & 0xF0000000) | (target_address << 2));
		jal();
		break;
	case 0x04: //BEQZ
		beqz();
		break;
	case 0x05: //BNE
		bne();
		break;
	case 0x06: //BLEZ
		blez();
		break;
	case 0x07: //BGTZ
		bgtz();
	case 0x08: //ADDI
		LOG_F(INFO, "Instruction: ADDI | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		addi();
		break;
	case 0x09: //LI - shares opcode with ADDUI and its very confusing to me (going to impliment ADDUI for now)
		LOG_F(INFO, "Instruction: ADDUI | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		//register_file[rt_new] = register_file[rs_new] + immediate;
		addi();
		break;
	case 0x0a: //SLTI
		LOG_F(INFO, "Instruction: SLTI | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		slti();
		break;
	case 0x0b: //SLTIU
		LOG_F(INFO, "Instruction: SLTIU | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		sltiu();
		break;
	case 0x0C: //ANDI
		LOG_F(INFO, "Instruction: ANDI | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		andi();
		break;
	case 0x10: //MTCz or MFCz
		LOG_F(ERROR, "Unimplimented instructed, MTCz or MFCz, no coprocess has been made yet");
		break;
	case 0x0F: //LUI
		LOG_F(INFO, "Instruction: LUI | rt_new: %x | Immediate: %x", rt_new, immediate);
		lui();
		break;
	case 0x0D: //ORI
		LOG_F(INFO, "Instruction: ORI | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		ori();
		break;
	case 0x20: //LB
		LOG_F(INFO, "Instruction: LB | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		lb();
		break;
	case 0x23: //LW
		LOG_F(INFO, "Instruction: LW | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		lw();
		break;
	case 0x24: //LBU
		lb();
		break;
	case 0x25: //LHU
		lhu();
		break;
	case 0x28: //SB
		LOG_F(INFO, "Instruction: SB | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		sb();
		break;
	case 0x29:
		LOG_F(INFO, "Instruction: SH | rt_new: %x | rs_new: %x | Immediate: %x", rt_new, rs_new, immediate);
		sh();
		break;
	case 0x2B: //SW
		LOG_F(INFO, "Instruction: SW | rt_new: %x | base: %x | Immediate: %x", rt_new, rs_new, immediate);
		sw();
		break;
	default:
		LOG_F(ERROR, "UNKNOWN OPCODE: %x | %x | %x", opcode, IP - 4, Instruction);
		isStep = true;
	}

	//Log all cpu info
	if (isStep) {
		LOG_F(INFO, "Instruction Pointer: %x", IP - 4);
		LOG_F(INFO, "Instruction: %X", Instruction);
		LOG_F(INFO, "--------------------------------------------------------------");
		LOG_F(INFO, "ZR: %x | AT: %x | V0: %x | V1: %x | A0: %x | A1: %x | A2: %x | A3: %x", register_file[0], register_file[1], register_file[2], register_file[3], register_file[4], register_file[5], register_file[6], register_file[7]);
		LOG_F(INFO, "T0: %x | T1: %x | T2: %x | T3: %x | T4: %x | T5: %x | T6: %x | T7: %x", register_file[8], register_file[9], register_file[10], register_file[11], register_file[12], register_file[13], register_file[14], register_file[15]);
		LOG_F(INFO, "S0: %x | S1: %x | S2: %x | S3: %x | S4: %x | S5: %x | S6: %x | S7: %x", register_file[16], register_file[17], register_file[18], register_file[19], register_file[20], register_file[21], register_file[22], register_file[23]);
		LOG_F(INFO, "T8: %x | T9: %x | K0: %x | K1: %x | GP: %x | SP: %x | FP: %x | RA: %x", register_file[24], register_file[25], register_file[26], register_file[27], register_file[28], register_file[29], register_file[30], register_file[31]);
		LOG_F(INFO, "--------------------------------------------------------------");

		system("pause");
	}


	//Set zero register to zero
	register_file[0] = 0;
}

*/