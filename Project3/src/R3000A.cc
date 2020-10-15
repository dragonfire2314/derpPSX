/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */

#include "Global.h"

/*  6-bit */
#define opcode \
    ((code >> 26) & 63)

/* 16-bit */
#define imm \
    ((sh)code)

#define immu \
    (code & 0xffff)

/* 32-bit */
#define ob \
    (base[rs] + imm)

#define baddr \
    (pc + (imm << 2))

#define saddr \
    ((code & 0x3ffffff) << 2) | (pc & 0xf0000000)

#define opcodeSWx(o, d) \
    mem.write<uw>(ob & ~3, (base[rt] o shift[d][ob & 3]) | (mem.read<uw>(ob & ~3) & mask[d][ob & 3]))

#define opcodeLWx(o, d) \
    base[rt] = (base[rt] & mask[d][ob & 3]) | (mem.read<uw>(ob & ~3) o shift[d][ob & 3])

CstrMips cpu;

// SLLV
//
// 32 | 16 |  8 |  4 |  2 |  1 |
// ---|----|----|----|----|----| -> 4
//  0 |  0 |  0 |  1 |  0 |  0 |

void CstrMips::reset() {
    memset(&base, 0, sizeof(base));
    memset(&copr, 0, sizeof(copr));
    cop2c = { 0 };
    cop2d = { 0 };
    
    copr[12] = 0x10900000;
    copr[15] = 0x2; // Co-processor Revision
    
    setpc(0xbfc00000);
	//setpc(0x80010870);
    res.s64 = opcodeCount = 0;

	//Temp
	base[29] = 0x500;
}

void CstrMips::setpc(uw addr) {
    pc = addr;
    
    if ((pc >> 20) == 0xbfc) {
        instCache = (uw *)&mem.rom.ptr[addr & 0x000fffff];
    }
    else {
        instCache = (uw *)&mem.ram.ptr[addr & 0x00ffffff];
    }
}

void CstrMips::bootstrap() {
    while(pc != 0x80030000) {
        step(false);
    }
}

#define LAZY_CLOCK \
    64

void CstrMips::run() {
    
}

void CstrMips::step(bool branched) {

	//SysCall Bios A table detection
	if ((pc & 0xFFFFF) == 0xA0) {
		bios.A0_Call();
		//printf("[SYS] A Bios call: %x, %x \n", pc, base[9]);
	}
	if ((pc & 0xFFFFF) == 0xB0) {
		bios.B0_Call();
		//printf("[SYS] B Bios call: %x, %x \n", pc, base[9]);
	}
	if ((pc & 0xFFFFF) == 0xC0) {
		bios.C0_Call();
		//printf("[SYS] C Bios call: %x, %x \n", pc, base[9]);
	}

	//Printf
	if (pc == 0xbfc018e0) {
		//Print someting
		//Copy off message
		uw index = base[4] & 0x1FFFFF;
		uw val1 = base[5] & 0x1FFFFF;
		char c;
		char oldC = 0;
		std::string str;
		c = mem.read<ub>(index);
		oldC = c;
		while (c)
		{
			str += c;
			index++;
			c = mem.read<ub>(index);

			if (oldC == '%') {
				//std::cout << "TEST";
				if (index) {}
				switch (c) {
				case 's':
					std::cout << (mem.ram.ptr + val1);
					break;
				}
			}

			oldC = c;
		}
		std::cout << str << std::endl;
	}
	
	uw code = *instCache++; pc += 4;
    base[0] = 0;
    opcodeCount++;
	//rootCounters.tick();

	if (code == 0x0ff00698) 
	{
		printf("[CPU] PC: %x\n", pc-4);
	}

    switch(opcode) {
        case 0: // SPECIAL
            switch(code & 63) {
                case 0: // SLL
                    if (code) { // No operation?
                        base[rd] = base[rt] << sa;
                    }
                    break;
                    
                case 2: // SRL
                    base[rd] = base[rt] >> sa;
                    break;
                    
                case 3: // SRA
                    base[rd] = (sw)base[rt] >> sa;
                    break;
                    
                case 4: // SLLV
                    base[rd] = base[rt] << (base[rs] & 31);
                    break;
                    
                case 6: // SRLV
                    base[rd] = base[rt] >> (base[rs] & 31);
                    break;
                    
                case 7: // SRAV
                    base[rd] = (sw)base[rt] >> (base[rs] & 31);
                    break;
                    
                case 8: // JR
                    branch(base[rs]);
                    //psx.console(base, pc);
                    break;
                    
                case 9: // JALR
                    base[rd] = pc + 4;
                    branch(base[rs]);
                    break;
                    
                case 12: // SYSCALL
                    pc -= 4;
					//std::cout << "Syscall" << std::endl;
                    exception(0x20, branched);
                    break;
                    
                case 13: // BREAK
                    break;
                    
                case 16: // MFHI
                    base[rd] = res.u32[1];
                    break;
                    
                case 17: // MTHI
                    res.u32[1] = base[rs];
                    break;
                    
                case 18: // MFLO
                    base[rd] = res.u32[0];
                    break;
                    
                case 19: // MTLO
                    res.u32[0] = base[rs];
                    break;
                    
                case 24: // MULT
                    res.s64 = (sd)(sw)base[rs] * (sw)base[rt];
                    break;
                    
                case 25: // MULTU
                    res.s64 = (sd)base[rs] * base[rt];
                    break;
                    
                case 26: // DIV
                    if (base[rt]) {
                        res.u32[0] = (sw)base[rs] / (sw)base[rt];
                        res.u32[1] = (sw)base[rs] % (sw)base[rt];
                    }
                    break;
                    
                case 27: // DIVU
                    if (base[rt]) {
                        res.u32[0] = base[rs] / base[rt];
                        res.u32[1] = base[rs] % base[rt];
                    }
                    break;
                    
                case 32: // ADD
                    base[rd] = base[rs] + base[rt];
                    break;
                    
                case 33: // ADDU
                    base[rd] = base[rs] + base[rt];
                    break;
                    
                case 34: // SUB
                    base[rd] = base[rs] - base[rt];
                    break;
                    
                case 35: // SUBU
                    base[rd] = base[rs] - base[rt];
                    break;
                    
                case 36: // AND
                    base[rd] = base[rs] & base[rt];
                    break;
                    
                case 37: // OR
                    base[rd] = base[rs] | base[rt];
                    break;
                    
                case 38: // XOR
                    base[rd] = base[rs] ^ base[rt];
                    break;
                    
                case 39: // NOR
                    base[rd] = ~(base[rs] | base[rt]);
                    break;
                    
                case 42: // SLT
                    base[rd] = (sw)base[rs] < (sw)base[rt];
                    break;
                    
                case 43: // SLTU
                    base[rd] = base[rs] < base[rt];
                    break;
                    
                default:
                    printx("/// PSeudo 0x%08x | Unknown special opcode 0x%08x | %d", pc, code, (code & 63));
                    break;
            }
            break;
            
        case 1: // REGIMM
            switch(rt) {
                case 0: // BLTZ
                    if ((sw)base[rs] < 0) {
                        branch(baddr);
                    }
                    break;
                    
                case 1: // BGEZ
                    if ((sw)base[rs] >= 0) {
                        branch(baddr);
                    }
                    break;
                    
                case 17: // BGEZAL
                    base[31] = pc + 4;
                    
                    if ((sw)base[rs] >= 0) {
                        branch(baddr);
                    }
                    break;
                    
                default:
                    printx("/// PSeudo 0x%08x | Unknown bcond opcode 0x%08x | %d", pc, code, rt);
                    break;
            }
            break;
            
        case 2: // J
            branch(saddr);
            break;
            
        case 3: // JAL
            base[31] = pc + 4;
            branch(saddr);
            break;
            
        case 4: // BEQ
            if (base[rs] == base[rt]) {
                branch(baddr);
            }
            break;
            
        case 5: // BNE
            if (base[rs] != base[rt]) {
                branch(baddr);
            }
            break;
            
        case 6: // BLEZ
            if ((sw)base[rs] <= 0) {
                branch(baddr);
            }
            break;
            
        case 7: // BGTZ
            if ((sw)base[rs] > 0) {
                branch(baddr);
            }
            break;
            
        case 8: // ADDI
            base[rt] = base[rs] + imm;
            break;
            
        case 9: // ADDIU
            base[rt] = base[rs] + imm;
            break;
            
        case 10: // SLTI
            base[rt] = (sw)base[rs] < imm;
            break;
            
        case 11: // SLTIU
            base[rt] = base[rs] < immu;
            break;
            
        case 12: // ANDI
            base[rt] = base[rs] & immu;
            break;
            
        case 13: // ORI
            base[rt] = base[rs] | immu;
            break;
            
        case 14: // XORI
            base[rt] = base[rs] ^ immu;
            break;
            
        case 15: // LUI
            base[rt] = code << 16;
            break;
            
        case 16: // COP0
			//std::cout << "cop0 something" << std::endl;
            switch(rs) {
                case MFC:
                    base[rt] = copr[rd];
					break;

                case MTC:
                    copr[rd] = base[rt];
                    break;
                    
                case RFE: // Return from exception
                    copr[12] = (copr[12] & ~(0xf)) | ((copr[12] >> 2) & 0xf);
                    break;
                    
                default:
                    printx("/// PSeudo 0x%08x | Unknown cop0 opcode 0x%08x | %d", pc, code, rs);
                    break;
            }
            break;
            
        case 18: // COP2
            switch(rs) {
                case MFC:
                    //readCop2(rd);
                    //base[rt] = cop2d.iuw[rd];
					std::cout << "cop something" << std::endl;
                    break;
                    
                case CFC:
                    //base[rt] = cop2c.iuw[rd];
					std::cout << "cop something" << std::endl;
                    break;
                    
                case MTC:
                    //cop2d.iuw[rd] = base[rt];
					std::cout << "cop something" << std::endl;
                    //writeCop2(rd);
                    break;
                    
                case CTC:
                    //cop2c.iuw[rd] = base[rt];
					std::cout << "cop something" << std::endl;
                    break;
                    
                default: // Execute GTE opcode
                    //executeCop2(code);
					std::cout << "cop something" << std::endl;
                    break;
            }
            break;
            
        case 32: // LB
            base[rt] = (sb)mem.read<ub>(ob);
            break;
            
        case 33: // LH
            base[rt] = (sh)mem.read<uh>(ob);
            break;
            
        case 34: // LWL
            opcodeLWx(<<, 0);
            break;
            
        case 35: // LW
            base[rt] = mem.read<uw>(ob);
            break;
            
        case 36: // LBU
            base[rt] = mem.read<ub>(ob);
            break;
            
        case 37: // LHU
            base[rt] = mem.read<uh>(ob);
            break;
            
        case 38: // LWR
            opcodeLWx(>>, 1);
            break;
            
        case 40: // SB
            mem.write<ub>(ob, base[rt]);
            break;
            
        case 41: // SH
            mem.write<uh>(ob, base[rt]);
            break;
            
        case 42: // SWL
            opcodeSWx(>>, 2);
            break;
            
        case 43: // SW
            mem.write<uw>(ob, base[rt]);
            break;
            
        case 46: // SWR
            opcodeSWx(<<, 3);
            break;
            
        case 50: // LWC2
            //cop2d.iuw[rt] = mem.read<uw>(ob);
            //writeCop2(rt);
			//std::cout << "cop something" << std::endl;
            break;
            
        case 58: // SWC2
            //readCop2(rt);
            //mem.write<uw>(ob, cop2d.iuw[rt]);
			//std::cout << "cop something" << std::endl;
            break;
            
        default:
            printx("/// PSeudo 0x%08x | Unknown basic opcode 0x%08x | %d", pc, code, opcode);
            break;
    }
}

int count2 = 0; // Temp hack

void CstrMips::branch(uw addr) {
    // Execute instruction in slot
    step (true);
    setpc(addr);
    
    if ((++count2 % 4) == 0) { count2 = 0;
        //rootc.update();
        //bus.interruptsUpdate();
        
        // Exceptions
        if (data32 & mask32) {
            if ((copr[12] & 0x401) == 0x401) {
                exception(0x400, false);
            }
        }
    }
}

void CstrMips::exception(uw code, bool branched) {
    if (branched) {
        printx("/// PSeudo Exception %s", "branched");
    }
    
	//std::cout << "execption" << std::endl;

    copr[12] = (copr[12] & ~(0x3f)) | ((copr[12] << 2) & 0x3f);
    copr[13] = code;
    copr[14] = pc;
    
    setpc(0x80);
}

CstrMips::CstrMips()
{

}
