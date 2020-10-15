#include "Global.h"

CDROM cd;

CDROM::CDROM()
{

}

CDROM::~CDROM()
{

}

uw CDROM::read(uw address)
{
	switch (address & 0xF) 
	{
	case 0x0:
		return (8 | index);
		break;
	case 0x3:
		switch (index)
		{
		case 0x1:
			return 3;
			break;
		default:
			return 0;
		}
		break;
	default:
		return 0;
	}
}

void CDROM::write(uw address, uw data)
{
	switch (address & 0xF)
	{
	case 0x0:
		index = data & 0x3;
		break;
	default:

		break;
	}
}
