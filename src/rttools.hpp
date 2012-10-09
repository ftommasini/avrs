/**
 * \file rttools.h
 */

#ifndef RTTOOLS_HPP_
#define RTTOOLS_HPP_

#include <string>
#include <unistd.h>

// RTAI headers
#include <rtai_lxrt.h>
#include <rtai_mbx.h>

// AVRS headers
#include "common.hpp"

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

// mailboxes names
//#define MBX_OUT_NAME		"MBXOUT"
//#define MBX_OUT_BLOCK		(BUFFER_SAMPLES * N_CHANNELS)

#define MBX_TRACKER_NAME	"MBXTRA"
#define MBX_TRACKER_BLOCK	2  // 2 blocks of tracker data

// for output FIFO
#define RTF_OUT_NUM     	0
#define RTF_OUT_DEV			"/dev/rtf0"
#define RTF_OUT_BLOCK		(BUFFER_SAMPLES * N_CHANNELS)

// CPU definitions
#define CPU_MAX				(sysconf(_SC_NPROCESSORS_ONLN))  // maximum number of available CPUs (Linux platform)

#endif  // RTTOOLS_HPP_
