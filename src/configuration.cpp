/**
 * \file configuration.cpp
 * \brief
 */

#include <cstdio>

#include "configuration.hpp"
#include "configfilereader.hpp"
#include "common.hpp"
#include "tokenizer.hpp"
#include "mathtools.hpp"

bool avrs::load_sim_file(const string filename, config_sim_t &c)
{
	// todo check for errors!

	ConfigFileReader cfr(filename);
	int coord;
	string delimiter = ","; // delimiter for coordinates
	string tmp;
	string b_title;
	string a_title;
	stringstream id;
	bool ok;

	// Room
	cfr.readInto(c.dxf_file, "ROOM_DXF_FILE");
	cfr.readInto(c.volume, "ROOM_VOLUME");
	ok = cfr.readInto(c.n_surfaces, "ROOM_N_SURFACES");

	if (!ok)
		c.n_surfaces = 0;

	cfr.readInto(c.filter_surf_file, "ROOM_FILTER_SURFACES_FILE");
	ok = load_surface_filters(c.filter_surf_file, c);

	if (!ok)
		ERROR("surface filters");

	cfr.readInto(c.max_distance, "ISM_MAX_DISTANCE");
	cfr.readInto(c.max_order, "ISM_MAX_ORDER");

	// Sound Source
	cfr.readInto(c.ir_file, "SOUND_SOURCE_IR_FILE");
	cfr.readInto(c.directivity_file, "SOUND_SOURCE_DIRECTIVITY_FILE");
	c.sound_source = SoundSource::create(c.ir_file);
	assert(c.sound_source.get() != NULL);
	cfr.readInto(tmp, "SOUND_SOURCE_POSITION");
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
	cfr.readInto(tmp, "LISTENER_POSITION");
	point3d_t pos;
	Tokenizer t2(tmp, delimiter);
	coord = 0;

	while (t2.next_token())
	{
		assert(coord <= 3);
		pos.at(coord++) = (float) atof(t2.get_token().c_str());
	}

	c.listener->set_position_reference(pos);
	cfr.readInto(tmp, "LISTENER_ORIENTATION");
	orientation_angles_t ori;
	Tokenizer t3(tmp, delimiter);
	t3.next_token();
	ori.az = (float) atof(t3.get_token().c_str());
	t3.next_token();
	ori.el = (float) atof(t3.get_token().c_str());
	//c.listener->set_orientation_reference(ori);

//	ori.az = -185;
//	ori.el = -5;
//	pos.at(0) = 5;
//	pos.at(1) = 42;
//	pos.at(2) = 35;

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

	cfr.readInto(c.hrtf_file, "LISTENER_HRTF_FILE");
	cfr.readInto(c.hrtf_filter_file, "LISTENER_FILTER_HRTF_FILE");

	// Input
	cfr.readInto(c.anechoic_file, "ANECHOIC_FILE");

	// Output
	cfr.readInto(c.master_gain_db, "MASTER_GAIN_DB");

	// General
	cfr.readInto(c.temperature, "TEMPERATURE");
	c.speed_of_sound = mathtools::speed_of_sound(c.temperature);
	cfr.readInto(c.angle_threshold, "ANGLE_THRESHOLD");
	cfr.readInto(c.bir_length_sec, "BIR_LENGTH");
	c.bir_length_samples = (unsigned long) (c.bir_length_sec * SAMPLE_RATE);

	return true;
}

void avrs::show_sim_config(const config_sim_t &c)
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

bool avrs::load_surface_filters(string filename, config_sim_t &c)
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
