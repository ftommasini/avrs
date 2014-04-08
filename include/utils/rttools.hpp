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
 * \file rttools.h
 */

#ifndef RTTOOLS_HPP_
#define RTTOOLS_HPP_

#include <string>
#include <unistd.h>

// RTAI headers
extern "C" {
#include <rtai_lxrt.h>
}
extern "C" {
#include <rtai_mbx.h>
}

// AVRS headers
#include "common.hpp"

namespace avrs
{

namespace rttools
{
	// RT Mailboxes functions
	MBX *get_mbx(std::string name, int size);
	int del_mbx(std::string);

	/**
	 * Get CPU code for real-time tasks
	 * @param cpu number of CPU to select, 0 for all CPUs
	 * @return code of CPU selected
	 */
	unsigned int cpu_id(unsigned int cpu);
}

// real-time tick time
#define TICK_TIME       	((long) ((BUFFER_SAMPLES * 1E9) / SAMPLE_RATE))  // in ns

#define MBX_TRACKER_NAME	"MBXTRA"
#define MBX_TRACKER_BLOCK	2  // 2 blocks of tracker data

// for output FIFO
#define RTF_OUT_NUM     	0
#define RTF_OUT_DEV			"/dev/rtf0"
#define RTF_OUT_BLOCK		(BUFFER_SAMPLES * N_CHANNELS)

// CPU definitions
#define CPU_MAX				(sysconf(_SC_NPROCESSORS_ONLN))  // maximum number of available CPUs (Linux platform)

}  // namespace avrs

#endif  // RTTOOLS_HPP_
