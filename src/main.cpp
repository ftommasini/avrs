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

#include <cstdio>
#include <cstdlib>
#include <exception>

#include "common.hpp"
#include "system.hpp"
#include "configuration.hpp"
#include "timer.hpp"
#include "avrsexception.hpp"

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
 * @version 0.2.0
 *
 * @date 2009-2013
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

	//print_about(); // TODO if quiet option is activated, not shown
	ConfigurationManager cm;
	configuration_t *config;

	try
	{
		cm.load_configuration(argv[1]);
		//configuration_ptr config_ptr = cm.get_configuration();
		config = cm.get_configuration();
	}
	catch (const AvrsException &e)
	{
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}

	cm.show_configuration();

	System::ptr_t sys;
	Timer t;
	string end_message;

	// create auto_ptr pointer to the system
	sys = System::create(config);
	assert(sys.get() != NULL);
	printf("Running... (end with \'q\' + Enter)\n");
	// get start time (precision is not necessary, only for informative purposes)
	t.start();
	// run the auralization system
	bool ok = sys->run();
	// get end time
	t.stop();

	if (ok)
		end_message = "Ending...";
	else
		end_message = "Ending with errors...";

	// show final messages
	printf("%s ", end_message.c_str());
	printf("Running time: %.2f s\n", t.get_elapsed_s());

	return EXIT_SUCCESS;
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
