/**
 * \file rttools.cpp
 */

#include "rttools.hpp"

MBX *rttools::get_mbx(std::string name, int size)
{
	unsigned long mbx_id = nam2num(name.c_str());
	MBX *mbx = (MBX *) rt_get_adr(mbx_id);

	if (!mbx)  // if doesn't exist, create a new mailbox
	{
		mbx = rt_mbx_init(mbx_id, size);
	}

	return mbx;
}

int rttools::del_mbx(std::string name)
{
	unsigned long mbx_id = nam2num(name.c_str());
	MBX *mbx = (MBX *) rt_get_adr(mbx_id);
	int retval = 0;  // success

	if (mbx)  // if exist, delete the mailbox
	{
		retval = rt_mbx_delete(mbx);
	}

	return retval;
}

unsigned int rttools::cpu_id(unsigned int cpu)
{
	assert(cpu <= CPU_MAX);

	if (cpu == 0)
		return 0xFF;  // all available CPUs
	else
		return (1 << (cpu - 1));
}
