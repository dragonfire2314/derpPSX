#pragma once

class CDROM
{
private:
	ub index = 0;
public:
	CDROM();
	~CDROM();

	uw read(uw address);
	void write(uw address, uw data);
};

extern CDROM cd;