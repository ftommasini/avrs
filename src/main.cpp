/**
 * @file main.cpp
 * @brief Main command line interface
 */

#include <cstdio>
#include <cstdlib>
#include <ctime>  // gettimeofday
// AVRS headers
#include "common.hpp"
#include "system.hpp"
#include "configuration.hpp"

using namespace std;

// On Linux, must be  compile with the -D_REENTRANT option.  This tells
// the C/C++ libraries that the functions must be thread-safe
#ifndef _REENTRANT
#error You need to compile with _REENTRANT defined since this uses threads
#endif

/**
 * @mainpage
 * Acoustic Virtual Reality %System (AVRS) for auralization purposes.
 *
 * @see http://avrsystem.sourceforge.net
 * @see http://www.investigacion.frc.utn.edu.ar/cintra
 *
 * @author Fabián C. Tommasini
 * @author Oscar A. Ramos (scientific supervision)
 * @author Mariano Araneda (contributor)
 * @author Sebastián P. Ferreyra (contributor)
 * @author G. Agustín Cravero (contributor)
 *
 * @version 0.1.1
 *
 * @date 2009-2012
 *
 * @section License
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 */

// Global variables

// Prototypes
static void usage();
static void print_about();

/**
 * Main function of the AVRS
 *
 * @param argc number of input arguments
 * @param argv array of actual input arguments
 * @return 0 on success, else some error code (mostly 1)
 */
int main(int argc, char *argv[])
{
	// Check given arguments
	if (argc != 2)
	{
		usage();
		exit(EXIT_FAILURE);
	}

	// TODO parse arguments

	avrs::config_sim_t config_sim;
	System::ptr_t sys;
	string end_message;
	string config_filename;
	struct timeval start;
	struct timeval end;
	float elapsed_sec;
	bool ok;

	config_filename = argv[1];
	ok = avrs::load_sim_file(config_filename, config_sim);

	if (!ok)
	{
		ERROR("Problem loading simulation configuration file");
		exit(EXIT_FAILURE);
	}

	print_about(); // TODO if quiet option is activated, not shown

	//avrs::show_sim_config(config_sim);

	// create auto_ptr pointer to the system
	sys = System::create(&config_sim);
	assert(sys.get() != NULL);
	printf("Running... (end with \'q\' + Enter)\n");
	// get start time
	// precision is not necessary, only for informative purposes (gettimeofday have a precision about 40-50 us)
	gettimeofday(&start, 0);
	// run the auralization system
	ok = sys->run();
	// get end time
	gettimeofday(&end, 0);
	// calculate running time
	elapsed_sec = (float) (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1E6;

	if (ok)
		end_message = "Ending...";
	else
		end_message = "Ending with errors...";

	// show final messages
	printf("%s ", end_message.c_str());
	printf("Running time: %.2f s\n", elapsed_sec);

	return 0;
}

void print_about()
{
	const string about = "AVRS (Acoustic Virtual Reality System)\n" //	" "PACKAGE_STRING" aka "NICKNAME"\n\n"
				"Version 0.1.1 (2009-2012)\n"
				"\n"
				"Main developer: Fabián C. Tommasini\n"
				"Scientific supervision: Oscar A. Ramos\n"
				"Contributors: Mariano Araneda, Sebastián P. Ferreyra, G. Agustín Cravero\n"
				"\n"
				"Developed at:\n"
				"\n"
				"[english]\n"
				"Research and Transfer Center on Acoustics\n"
				"National Technological University, Cordoba Regional Faculty\n"
				"Cordoba, Argentina\n"
				"\n"
				"[español]\n"
				"Centro de Investigación y Transferencia en Acústica\n"
				"Universidad Tecnológica Nacional, Facultad Regional Córdoba\n"
				"Córdoba, Argentina\n"
				"\n"
				"With the support of CONICET, Argentina\n"
				"\n"
				"Website: http://avrsystem.sourceforge.net\n"
				"E-mail contact: fabian@tommasini.com.ar\n";
	printf("%s\n", about.c_str());
}

void usage()
{
	printf("Usage:\n");
	printf("\tavrs <simulation configuration file>\n");
}
