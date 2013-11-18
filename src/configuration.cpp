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
 * \file configuration.cpp
 */

#include <cstdio>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "configuration.hpp"
#include "configfilereader.hpp"
#include "common.hpp"
#include "tokenizer.hpp"
#include "mathtools.hpp"

void avrs::load_config_file(const string filename, avrs::configuration_t &c)
{
	using namespace boost::filesystem;

	path p_conf(filename);
	ConfigFileReader cfr(p_conf.string());
	int coord;
	string delimiter = ","; // delimiter for coordinates
	string tmp;
	string b_title;
	string a_title;
	stringstream id;

	// Room
	if (!cfr.readInto(tmp, "ROOM_DXF_FILE"))
		throw "Error in configuration file: ROOM_DXF_FILE is missing";

	path p_dxf(tmp);
	path p_dxf_full = p_conf.parent_path() / p_dxf;
	c.dxf_file = p_dxf_full.string();

	if (!cfr.readInto(c.volume, "ROOM_VOLUME"))
		throw "Error in configuration file: ROOM_VOLUME is missing";

	if (!cfr.readInto(c.n_surfaces, "ROOM_N_SURFACES"))
		throw "Error in configuration file: ROOM_N_SURFACES is missing";

	if (!cfr.readInto(tmp, "ROOM_FILTER_SURFACES_FILE"))
		throw "Error in configuration file: ROOM_FILTER_SURFACES_FILE is missing";

	path p_surf(tmp);
	path p_surf_full = p_conf.parent_path() / p_surf;
	c.filter_surf_file = p_surf_full.string();

	if (!load_surface_filters(c.filter_surf_file, c))
		throw "Error loading surface filters";

	// ISM parameters
	if (!cfr.readInto(c.max_distance, "ISM_MAX_DISTANCE"))
		throw "Error in configuration file: ISM_MAX_DISTANCE is missing";

	if (!cfr.readInto(c.max_order, "ISM_MAX_ORDER"))
		throw "Error in configuration file: ISM_MAX_ORDER is missing";

	// Sound Source
	if (!cfr.readInto(tmp, "SOUND_SOURCE_IR_FILE"))
		throw "Error in configuration file: SOUND_SOURCE_IR_FILE is missing";

	path p_ir(tmp);
	path p_ir_full = p_conf.parent_path() / p_ir;
	c.ir_file = p_ir_full.string();

	if (!cfr.readInto(tmp, "SOUND_SOURCE_DIRECTIVITY_FILE"))
		throw "Error in configuration file: SOUND_SOURCE_DIRECTIVITY_FILE is missing";

	path p_dir(tmp);
	path p_dir_full = p_conf.parent_path() / p_dir;
	c.directivity_file = p_dir_full.string();

	c.sound_source = SoundSource::create(c.ir_file);
	assert(c.sound_source.get() != NULL);

	if (!cfr.readInto(tmp, "SOUND_SOURCE_POSITION"))
		throw "Error in configuration file: SOUND_SOURCE_POSITION is missing";

	Tokenizer t1(tmp, delimiter);
	coord = 0;

	while (t1.next_token())
	{
		assert(coord <= 3);
		c.sound_source->pos.at(coord++) = (float) atof(t1.get_token().c_str());
	}

	// orientation of sound source is discarded

	// Listener
	c.listener = Listener::create();
	assert(c.listener.get() != NULL);

	if (!cfr.readInto(tmp, "LISTENER_POSITION"))
		throw "Error in configuration file: LISTENER_POSITION is missing";

	point3d_t pos;
	Tokenizer t2(tmp, delimiter);
	coord = 0;

	while (t2.next_token())
	{
		assert(coord <= 3);
		pos.at(coord++) = (float) atof(t2.get_token().c_str());
	}

	c.listener->set_position_reference(pos);

	if (!cfr.readInto(tmp, "LISTENER_ORIENTATION"))
		throw "Error in configuration file: LISTENER_ORIENTATION is missing";

	orientation_angles_t ori;
	Tokenizer t3(tmp, delimiter);
	t3.next_token();
	ori.az = (float) atof(t3.get_token().c_str());
	t3.next_token();
	ori.el = (float) atof(t3.get_token().c_str());
	//c.listener->set_orientation_reference(ori);

	// azimuth (-180, +180]
	if (ori.az > 180)
		ori.az -= 360;
	else if (ori.az  < -180)
		ori.az += 360;

	// elevation [-90, +90]
	if (ori.el > 90)
		ori.el = 180 - ori.el;
	else if (ori.el < -90)
		ori.el = -180 - ori.el;

	c.listener->set_initial_point_of_view(ori, pos);

	if (!cfr.readInto(c.hrtf_file, "LISTENER_HRTF_FILE"))
		throw "Error in configuration file: LISTENER_HRTF_FILE is missing";

	if (!cfr.readInto(tmp, "LISTENER_FILTER_HRTF_FILE"))  // IIR filters coefficients
		throw "Error in configuration file: LISTENER_FILTER_HRTF_FILE is missing";

	path p_hrtf(tmp);
	path p_hrtf_full = p_conf.parent_path() / p_hrtf;
	c.hrtf_filter_file = p_hrtf_full.string();

	// Input
	if (!cfr.readInto(tmp, "ANECHOIC_FILE"))
		throw "Error in configuration file: ANECHOIC_FILE is missing";

	path p_a(tmp);
	path p_a_full = p_conf.parent_path() / p_a;
	c.anechoic_file = p_a_full.string();

	// Output
	if (!cfr.readInto(c.master_gain_db, "MASTER_GAIN_DB"))
		throw "Error in configuration file: MASTER_GAIN_DB is missing";

	// General
	if (!cfr.readInto(c.temperature, "TEMPERATURE"))
		throw "Error in configuration file: TEMPERATURE is missing";

	c.speed_of_sound = mathtools::speed_of_sound(c.temperature);

	if (!cfr.readInto(c.angle_threshold, "ANGLE_THRESHOLD"))
		throw "Error in configuration file: ANGLE_THRESHOLD is missing";

	if (!cfr.readInto(c.bir_length_sec, "BIR_LENGTH"))
		throw "Error in configuration file: BIR_LENGTH is missing";

	c.bir_length_samples = (unsigned long) (c.bir_length_sec * SAMPLE_RATE);
}

void avrs::show_config(const avrs::configuration_t &c)
{
	printf("AVRS simulation configuration\n");
	printf("-----------------------------\n");

	printf("\nRoom section\n\n");
	printf("ROOM_DXF_FILE = %s\n", c.dxf_file.c_str());
	printf("ROOM_VOLUME = %.2f\n", c.volume);
	printf("ROOM_N_SURFACES = %d\n", c.n_surfaces);
	printf("ROOM_FILTER_SURFACES_FILE = %s\n", c.filter_surf_file.c_str());
	printf("Coefficients:\n");

	for (unsigned int i = 0; i < c.n_surfaces; i++)
	{
		std::vector<double> row_b = c.b_coeff.at(i);
		printf("Surface %d\n", i + 1);
		printf("b: ");

		for (unsigned int j = 0; j < row_b.size(); j++)
			printf("%.6f ", row_b[j]);

		std::vector<double> row_a = c.a_coeff.at(i);
		printf("\na: ");

		for (unsigned int j = 0; j < row_a.size(); j++)
			printf("%.6f ", row_a[j]);

		printf("\n");
	}

	printf("ISM_MAX_ORDER = %d\n", c.max_order);
	printf("ISM_MAX_DISTANCE = %.2f\n", c.max_distance);

	printf("\nSound source section\n\n");
	printf("SOUND_SOURCE_IR_FILE = %s\n", c.ir_file.c_str());
	printf("SOUND_SOURCE_DIRECTIVITY_FILE = %s\n", c.directivity_file.c_str());
	printf("SOUND_SOURCE_POSITION = %.2f, %.2f, %.2f\n",
			c.sound_source->pos[0], c.sound_source->pos[1], c.sound_source->pos[2]);
	printf("SOUND_SOURCE_ORIENTATION = \n");

	printf("\nListener section\n\n");
	printf("LISTENER_POSITION = %.2f, %.2f, %.2f\n",
			(c.listener->get_position())[0], (c.listener->get_position())[1], (c.listener->get_position())[2]);
	printf("LISTENER_ORIENTATION = %.2f, %.2f\n",
			c.listener->get_orientation().az, c.listener->get_orientation().el);
	printf("LISTENER_HRTF_FILE = %s\n", c.hrtf_file.c_str());
	printf("LISTENER_FILTER_HRTF_FILE = %s\n", c.hrtf_filter_file.c_str());

	printf("\nInput section\n\n");
	printf("ANECHOIC_FILE = %s\n", c.anechoic_file.c_str());

	printf("\nOutput section\n\n");
	printf("MASTER_GAIN_DB = %.2f\n", c.master_gain_db);

	printf("\nGeneral section\n\n");
	printf("TEMPERATURE = %.2f\n", c.temperature);
	printf("ANGLE_THRESHOLD = %.2f\n", c.angle_threshold);
	printf("BIR_LENGTH = %.2f\n", c.bir_length_sec);
}

bool avrs::load_surface_filters(string filename, avrs::configuration_t &c)
{
	FILE *p_file;
	size_t n;
	unsigned int n_surf, order;

	p_file = fopen(filename.c_str(), "rb");

	if (!p_file)
	{
		return false;
	}

	// Number of surfaces to read
	n = fread(&n_surf, sizeof(unsigned int), 1, p_file);

	if (n != 1)
		goto error;

	// Filters order
	n = fread(&order, sizeof(unsigned int), 1, p_file);

	if (n != 1)
		goto error;

	for (unsigned int i = 0; i < n_surf; i++)
	{
		std::vector<double> row_b(order + 1);

		n = fread(&row_b[0], sizeof(double), order + 1, p_file);

		if (n != order + 1)
			goto error;

		c.b_coeff.push_back(row_b);
		std::vector<double> row_a(order + 1);

		n = fread(&row_a[0], sizeof(double), order + 1, p_file);

		if (n != order + 1)
			goto error;

		c.a_coeff.push_back(row_a);
	}

	fclose(p_file);
	return true;

error: // read error management
	fclose(p_file);
	return false;
}

