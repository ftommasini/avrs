/*
 * Copyright (C) 2011-2013 Fabián C. Tommasini <fabian@tommasini.com.ar>
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
 * @file configuration.hpp
 */

#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <string>

#include "soundsource.hpp"
#include "listener.hpp"

namespace avrs
{

// Simulation configuration
typedef struct
{
	// General
	std::string name;
	float temperature;
	float speed_of_sound;
	float angle_threshold;
	float bir_length_sec; ///< binaural impulse response (BIR) length in seconds
	unsigned long bir_length_samples;

	// Room
	std::string dxf_file;
	float volume;
	unsigned int n_surfaces;
	std::string filter_surf_file;

	// ISM parameters
	float max_distance;
	unsigned int max_order;

	std::vector< std::vector<double> > b_coeff;
	std::vector< std::vector<double> > a_coeff;

	// Sound Source
	std::string ir_file;
	std::string directivity_file;
	SoundSource::ptr_t sound_source;

	// Listener
	std::string hrtf_file;
	Listener::ptr_t listener;

	// Output
	float master_gain_db; ///< correction factor in dB

	// Input
	std::string anechoic_file;
} configuration_t;

//typedef boost::shared_ptr<configuration_t> configuration_ptr;
typedef configuration_t* configuration_ptr;

class ConfigurationManager
{
public:
	ConfigurationManager();
	virtual ~ConfigurationManager();

	void load_configuration(const std::string filename);
	configuration_ptr get_configuration();
	void show_configuration();

private:
	//configuration_ptr _conf;
	configuration_ptr _conf;
	std::string _filename;
	std::string _path;

	std::string full_path(const std::string relative_path);
	bool load_surface_filters(std::string filename);
};

} // namespace

#endif  // CONFIGURATION_HPP_
