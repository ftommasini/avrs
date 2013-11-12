/*
 * Copyright (C) 2009-2013 Fabián C. Tommasini <fabian@tommasini.com.ar>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 *
 */

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
