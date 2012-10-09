/**
 * \file configuration.hpp
 * \brief
 */

#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <string>

#include "soundsource.hpp"
#include "listener.hpp"

using namespace std;

namespace avrs
{

//typedef struct
//{
//    std::string exec_name;                ///< name of executable (without path)
//    std::string name;                     ///< name of system
//} config_sys_t;

// Simulation config
typedef struct
{
	// Room
	string dxf_file;
	float volume;
	unsigned int n_surfaces;
	string filter_surf_file;
	float max_distance;
	unsigned int max_order;
	std::vector< std::vector<double> > b_coeff;
	std::vector< std::vector<double> > a_coeff;

	// Sound Source
	string ir_file;
	string directivity_file;
	SoundSource::ptr_t sound_source;

	// Listener
	string hrtf_file;
	string hrtf_filter_file;
	Listener::ptr_t listener;

	// Output
	float master_gain_db; ///< correction factor in dB

	// Input
	string anechoic_file;

	// General
	float temperature;
	float speed_of_sound;
	float angle_threshold;
	float bir_length_sec; ///< binaural impulse response (BIR) length in seconds
	unsigned long bir_length_samples;
} config_sim_t;

bool load_sim_file(const string filename, config_sim_t &c);
void show_sim_config(const config_sim_t &conf);
bool load_surface_filters(string filename, config_sim_t &c);

} // namespace

#endif  // CONFIGURATION_H_
