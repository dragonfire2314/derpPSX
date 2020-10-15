#include "Global.h"

HardwareIO io;

uw HardwareIO::read(uw address)
{
	//Catch SPU
	if (address >= 0x1f801c00 && address <= 0x1f801e80) return 0;

	//Catch DMA
	if (address >= 0x1f801080 && address <= 0x1f8010ef)
	{
		//std::cout << "[DMA] read: " << std::hex << address << std::endl;
		return dma.dmaRead(address);
	}

	//Catch CDROM
	if (address >= 0x1f801800 && address <= 0x1f801803)
	{
		//std::cout << "[CDROM] read: " << std::hex << address << std::endl;
		cd.read(address);
		//cd.load_PXS_EXE();
		//system("PAUSE");
		return 0;
	}

	switch (address) 
	{
	case 0x1F8010F0: //Read DMA
		//std::cout << "[DMA] control read: " << std::hex << address << std::endl;
		return dma.readControl();
		break;
	case 0x1F801070: //Interupt status
		//std::cout << "[INT] stat read: " << std::hex << address << std::endl;
		return i_stat;
	case 0x1F801074: //Interupt mask
		//std::cout << "[INT] mask read: " << std::hex << address << std::endl;
		return i_mask;
	case 0x1F801DAE: //SPU Status Register
		//std::cout << "[SPU] stat read: " << std::hex << address << std::endl;
		return 0;
		break;
	case 0x1F801814: //GPU Status Register
		//std::cout << "[GPU] stat read: " << std::hex << address << std::endl;
		//std::cout << "GPU Status read: " << std::hex << *(cpu.instCache) << std::endl;
		return gpu.gpuReadStat();
		//return 0;
		break;
	case 0x1F801810: //GPU READ
		//std::cout << "[GP0] read: " << std::hex << address << std::endl;
		return 0;
		break;
	default:
		//std::cout << "[IO] read: " << std::hex << address << std::endl;
		return 0;
	}

	return uw();
}

void HardwareIO::write(uw data, uw address)
{
	//Catch SPU
	if (address >= 0x1f801c00 && address <= 0x1f801e80) return;
	
	//std::cout << "IO write: " << std::hex << address << " data: " << data << std::endl;

	//Catch DMA
	if (address >= 0x1f801080 && address <= 0x1f8010ef) 
	{
		//std::cout << "[DMA] write: " << std::hex << address << " data: " << data << std::endl;
		dma.dmaWrite(address, data);
		return;
	}

	//Catch CDROM
	if (address >= 0x1f801800 && address <= 0x1f801803)
	{
		//std::cout << "[CDROM] write: " << std::hex << address << " data: " << data << std::endl;
		cpu.pc = 0xbfc069a0;
		cd.write(address, data);
		//system("PAUSE");
	}

	switch (address) 
	{
	case 0x1F801070: //Interupt status
		//std::cout << "[INT] stat write: " << std::hex << address << " data: " << data << std::endl;
		i_stat = data;
		break;
	case 0x1F801074: //Interupt mask
		//std::cout << "[INT] mask write: " << std::hex << address << " data: " << data << std::endl;
		i_mask = data;
		break;
	case 0x1F8010F0: //Write DMA
		//std::cout << "[DMA] write control: " << std::hex << address << " data: " << data << std::endl;
		dma.writeControl(data);
		break;
	case 0x1f801810: // GPU Send GP0 command
		//std::cout << "[GPO] send commande: " << std::hex  << " data: " << data << std::endl;
		dataForGPU_Hack = data;
		dataPointerForGPU_Hack = &dataForGPU_Hack;
		gpu.gpu0Command(dataPointerForGPU_Hack, 1);
		break;
	case 0x1f801814: // GPU Send GP1 command
		//std::cout << "[GP1] send commande: " << std::hex << " data: " << data << std::endl;
		gpu.gpu1Command(data);
		break;
	default:
		//std::cout << "[Hardware] write: " << std::hex << address << " data: " << data << std::endl;
		break;
	}

	//Catch Trace
	if (address == 0x1F802041) {
		printf("[Trace] %x\n", data);
		system("PAUSE");
	}
}
