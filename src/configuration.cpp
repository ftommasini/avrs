/*
 * Copyright (C) 2011-2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
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
 * @file configuration.cpp
 */

#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/make_shared.hpp>

#include "utils/configfilereader.hpp"
#include "utils/tokenizer.hpp"
#include "utils/math.hpp"
#include "configuration.hpp"
#include "common.hpp"
#include "avrsexception.hpp"

namespace avrs
{

ConfigurationManager::ConfigurationManager()
{
	_conf = boost::make_shared<configuration_t>();
}

ConfigurationManager::~ConfigurationManager()
{
	;
}

configuration_t::ptr_t ConfigurationManager::get_configuration()
{
	return _conf;
}

void ConfigurationManager::show_configuration()
{
	printf("AVRS simulation configuration\n");
	printf("-----------------------------\n");

	printf("\nRoom section\n\n");
	printf("ROOM_DXF_FILE = %s\n", _conf->dxf_file.c_str());
	printf("ROOM_VOLUME = %.2f\n", _conf->volume);
	printf("ROOM_N_SURFACES = %d\n", _conf->n_surfaces);
	printf("ROOM_FILTER_SURFACES_FILE = %s\n", _conf->filter_surf_file.c_str());
	printf("Coefficients:\n");

	for (unsigned int i = 0; i < _conf->n_surfaces; i++)
	{
		std::vector<double> row_b = _conf->b_coeff.at(i);
		printf("Surface %d\n", i + 1);
		printf("b: ");

		for (unsigned int j = 0; j < row_b.size(); j++)
			printf("%.6f ", row_b[j]);

		std::vector<double> row_a = _conf->a_coeff.at(i);
		printf("\na: ");

		for (unsigned int j = 0; j < row_a.size(); j++)
			printf("%.6f ", row_a[j]);

		printf("\n");
	}

	printf("ISM_MAX_ORDER = %d\n", _conf->max_order);
	printf("ISM_MAX_DISTANCE = %.2f\n", _conf->max_distance);

	printf("\nSound source section\n\n");
	printf("SOUND_SOURCE_IR_FILE = %s\n", _conf->ir_file.c_str());
	printf("SOUND_SOURCE_DIRECTIVITY_FILE = %s\n", _conf->directivity_file.c_str());
	printf("SOUND_SOURCE_POSITION = %.2f, %.2f, %.2f\n",
			_conf->sound_source->pos[0], _conf->sound_source->pos[1], _conf->sound_source->pos[2]);
	printf("SOUND_SOURCE_ORIENTATION = \n");

	printf("\nListener section\n\n");
	printf("LISTENER_POSITION = %.2f, %.2f, %.2f\n",
			(_conf->listener->get_position())[0],
			(_conf->listener->get_position())[1],
			(_conf->listener->get_position())[2]);
	printf("LISTENER_ORIENTATION = %.2f, %.2f\n",
			_conf->listener->get_orientation().az,
			_conf->listener->get_orientation().el);
	printf("LISTENER_HRTF_FILE = %s\n", _conf->hrtf_file.c_str());

	printf("\nInput section\n\n");
	printf("ANECHOIC_FILE = %s\n", _conf->anechoic_file.c_str());

	printf("\nOutput section\n\n");
	printf("MASTER_GAIN_DB = %.2f\n", _conf->master_gain_db);

	printf("\nGeneral section\n\n");
	printf("TEMPERATURE = %.2f\n", _conf->temperature);
	printf("ANGLE_THRESHOLD = %.2f\n", _conf->angle_threshold);
	printf("BIR_LENGTH = %.2f\n", _conf->bir_length_sec);
}

void ConfigurationManager::load_configuration(const std::string filename)
{
	boost::filesystem::path p_conf(filename);
	_filename = filename;
	_path = p_conf.parent_path().string();
	ConfigFileReader cfr(p_conf.string());
	int coord;
	string delimiter = ","; // delimiter for coordinates
	string tmp;
	string b_title;
	string a_title;
	stringstream id;

	// Room
	if (!cfr.readInto(tmp, "ROOM_DXF_FILE"))
		throw AvrsException("Error in configuration file: ROOM_DXF_FILE is missing");

	_conf->dxf_file = full_path(tmp);

	if (!cfr.readInto(_conf->volume, "ROOM_VOLUME"))
		throw AvrsException("Error in configuration file: ROOM_VOLUME is missing");

	if (!cfr.readInto(_conf->n_surfaces, "ROOM_N_SURFACES"))
		throw AvrsException("Error in configuration file: ROOM_N_SURFACES is missing");

	if (!cfr.readInto(tmp, "ROOM_FILTER_SURFACES_FILE"))
		throw AvrsException("Error in configuration file: ROOM_FILTER_SURFACES_FILE is missing");

	_conf->filter_surf_file = full_path(tmp);

	if (!load_surface_filters(_conf->filter_surf_file))
		throw AvrsException("Error loading surface filters");

	if (!cfr.readInto(_conf->rt60_0, "ROOM_RT60_0"))
		throw AvrsException("Error loading reverberation time at DC");

	if (!cfr.readInto(_conf->rt60_pi, "ROOM_RT60_PI"))
		throw AvrsException("Error loading reverberation time at PI");

	// ISM parameters
	if (!cfr.readInto(_conf->max_distance, "ISM_MAX_DISTANCE"))
		throw AvrsException("Error in configuration file: ISM_MAX_DISTANCE is missing");

	if (!cfr.readInto(_conf->max_order, "ISM_MAX_ORDER"))
		throw AvrsException("Error in configuration file: ISM_MAX_ORDER is missing");

	if (!cfr.readInto(_conf->transition_time, "ISM_TRANSITION_TIME"))
		throw AvrsException("Error in configuration file: ISM_TRANSITION_TIME is missing");

	// Sound Source
	if (!cfr.readInto(tmp, "SOUND_SOURCE_IR_FILE"))
		throw AvrsException("Error in configuration file: SOUND_SOURCE_IR_FILE is missing");

	_conf->ir_file = full_path(tmp);;

	if (!cfr.readInto(tmp, "SOUND_SOURCE_DIRECTIVITY_FILE"))
		throw AvrsException("Error in configuration file: SOUND_SOURCE_DIRECTIVITY_FILE is missing");

	_conf->directivity_file = full_path(tmp);

	_conf->sound_source = SoundSource::create(_conf->ir_file);
	assert(_conf->sound_source.get() != NULL);

	if (!cfr.readInto(tmp, "SOUND_SOURCE_POSITION"))
		throw AvrsException("Error in configuration file: SOUND_SOURCE_POSITION is missing");

	Tokenizer t1(tmp, delimiter);
	coord = 0;

	while (t1.next_token())
	{
		assert(coord <= 3);
		_conf->sound_source->pos.at(coord++) = (float) atof(t1.get_token().c_str());
	}

	// orientation of sound source is discarded (omnidirectional only)

	// FDN
	if (!cfr.readInto(tmp, "FDN_FILTER_COEFF_B"))
		throw AvrsException("Error in configuration file: FDN_FILTER_COEFF_B is missing");

	_conf->fdn_b_coeff = full_path(tmp);

	if (!cfr.readInto(tmp, "FDN_FILTER_COEFF_A"))
		throw AvrsException("Error in configuration file: FDN_FILTER_COEFF_A is missing");

	_conf->fdn_a_coeff = full_path(tmp);

	// Listener
	_conf->listener = Listener::create();
	assert(_conf->listener.get() != NULL);

	if (!cfr.readInto(tmp, "LISTENER_POSITION"))
		throw AvrsException("Error in configuration file: LISTENER_POSITION is missing");

	point3_t pos;
	Tokenizer t2(tmp, delimiter);
	coord = 0;

	while (t2.next_token())
	{
		assert(coord <= 3);
		pos.at(coord++) = (float) atof(t2.get_token().c_str());
	}

	//_conf->listener->set_position_reference(pos);

	if (!cfr.readInto(tmp, "LISTENER_ORIENTATION"))
		throw AvrsException("Error in configuration file: LISTENER_ORIENTATION is missing");

	orientation_angles_t ori;
	Tokenizer t3(tmp, delimiter);
	t3.next_token();
	ori.az = (float) atof(t3.get_token().c_str());
	t3.next_token();
	ori.el = (float) atof(t3.get_token().c_str());

	//_conf->listener->set_orientation_reference(ori);
	_conf->listener->set_initial_point_of_view(ori, pos);

	if (!cfr.readInto(tmp, "LISTENER_HRTF_FILE"))
		throw AvrsException("Error in configuration file: LISTENER_HRTF_FILE is missing");

	_conf->hrtf_file = full_path(tmp);

	// Input
	if (!cfr.readInto(tmp, "ANECHOIC_FILE"))
		throw AvrsException("Error in configuration file: ANECHOIC_FILE is missing");

	_conf->anechoic_file = full_path(tmp);

	// Output
	if (!cfr.readInto(_conf->master_gain_db, "MASTER_GAIN_DB"))
		throw AvrsException("Error in configuration file: MASTER_GAIN_DB is missing");

	// Tracker
	if (!cfr.readInto(tmp, "TRACKER_SIM_FILE"))
		throw AvrsException("Error in configuration file: TRACKER_SIM_FILE is missing");

	_conf->tracker_sim_file = full_path(tmp);

	// General
	if (!cfr.readInto(_conf->temperature, "TEMPERATURE"))
		throw AvrsException("Error in configuration file: TEMPERATURE is missing");

	_conf->speed_of_sound = avrs::math::speed_of_sound(_conf->temperature);

	if (!cfr.readInto(tmp, "AIR_ABSORPTION_FILE"))
		throw AvrsException("Error in configuration file: AIR_ABSORPTION_FILE is missing");

	_conf->air_absorption_file = full_path(tmp);

	if (!cfr.readInto(_conf->angle_threshold, "ANGLE_THRESHOLD"))
		throw AvrsException("Error in configuration file: ANGLE_THRESHOLD is missing");

	if (!cfr.readInto(_conf->bir_length_sec, "BIR_LENGTH"))
		throw AvrsException("Error in configuration file: BIR_LENGTH is missing");

	_conf->bir_length_samples = (unsigned long) (_conf->bir_length_sec * SAMPLE_RATE);
}

std::string ConfigurationManager::full_path(const std::string relative_path)
{
	boost::filesystem::path p_rel(relative_path);
	boost::filesystem::path p_full = _path / p_rel;
	return p_full.string();
}

bool ConfigurationManager::load_surface_filters(std::string filename)
{
	bool ok = true;
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit );

	try {
		file.open(filename.c_str(), std::ios::in | std::ios::binary);

		// Number of surfaces to read
		uint n_surf;
		file.read(reinterpret_cast<char *>(&n_surf), sizeof(uint));

		// Filters order
		uint order;
		file.read(reinterpret_cast<char *>(&order), sizeof(uint));

		for (uint i = 0; i < n_surf; i++)
		{
			std::vector<double> row_b(order + 1);
			std::vector<double> row_a(order + 1);

			// B coefficients
			file.read(reinterpret_cast<char *>(&row_b[0]), sizeof(double) * (order + 1));
			// A coefficients
			file.read(reinterpret_cast<char *>(&row_a[0]), sizeof(double) * (order + 1));

			_conf->b_coeff.push_back(row_b);
			_conf->a_coeff.push_back(row_a);
		}

		file.close();
		ok = true;
	}
	catch (std::ifstream::failure ex)
	{
		std::cout << ex.what() << std::endl;
		ok = false;
	}
	catch (std::string ex)
	{
		std::cout << ex << std:: endl;
		ok = false;
	}

	return ok;
}

}  // namespace avrs
