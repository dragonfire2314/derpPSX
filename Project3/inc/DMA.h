#pragma once
class DMA
{
private:
	struct Channel {
		uw baseAddress;

		uh blockSize;
		uh blockAmount;
		uh numWords;

		ub transferDirection;
		ub step;
		ub mode;
		ub start;
		ub trigger;

		ub enabled;
	};

	Channel channels[7];

	uw dma_control = 0x07654321;

public:
	DMA();
	~DMA();

	void writeControl(uw data);
	uw readControl();

	void dmaWrite(uw address, uw data);
	void dmaChannelControl(ub channel, uw data);
	void dmaBaseAddress(ub channel, uw data);
	void dmaBlockControl(ub channel, uw data);
	uw dmaRead(uw address);

	void dmaGPU();
	void dmaOTC();
};

extern DMA dma;