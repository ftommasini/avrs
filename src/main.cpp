/*
 * Copyright (C) 2009-2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
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
#include <string>
#include <boost/program_options.hpp>

#include "common.hpp"
#include "system.hpp"
#include "avrsexception.hpp"
#include "utils/timercpu.hpp"

#include "version.hpp"

// On Linux, must be compile with the -D_REENTRANT option.  This tells
// the C/C++ libraries that the functions must be thread-safe
#ifndef _REENTRANT
#error You need to compile with _REENTRANT defined
#endif

using namespace avrs;

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
 * @date 2009-2014
 */

typedef struct
{
	std::string filename;
	bool show_config;
} parameters_t;

// Prototypes
void parse_program_options(int argc, char** argv, parameters_t *params);
void usage();
void print_version();

/**
 * Main function of the AVRS
 *
 * @param argc number of input arguments
 * @param argv array of actual input arguments
 * @return 0 on success, else some error code (mostly 1)
 */
int main(int argc, char *argv[])
{
	parameters_t params;

	// parse options, results in params
	parse_program_options(argc, argv, &params);

	System::ptr_t sys;
	TimerCpu t;
	std::string end_message;

	// create auto_ptr pointer to the system
	try
	{
		stk::Stk::setSampleRate(SAMPLE_RATE);  // set sample rate
#ifdef NDEBUG
		stk::Stk::printErrors(false);
#endif
		sys = System::get_instance(params.filename, params.show_config);
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
		printf("Running time: %.2f s\n", t.elapsed_time(second));
	}
	catch (const AvrsException &e)
	{
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

void parse_program_options(int argc, char** argv, parameters_t *params)
{
	namespace po = boost::program_options;

	std::string filename;
	po::options_description desc("Options");
	desc.add_options()
			("help,h", "Print help messages.")
			("file,f", po::value<std::string>(&filename)->required(), "Configuration file.")
			("show,s", "Show configuration.")
			("version", "Version information.");
	po::positional_options_description positional_options;
	positional_options.add("file", -1);
	po::variables_map vm;

	try
	{
		po::store(
				po::command_line_parser(argc, argv).options(desc).positional(positional_options).run(),
				vm);

		if (vm.count("help") || argc == 1)  // --help or -h option
		{
			usage();
			std::cout << std::endl << desc << std::endl;
		    exit(EXIT_SUCCESS);
		}

		if (vm.count("version"))  // --version option
		{
			print_version();
			exit(EXIT_SUCCESS);
		}

		po::notify(vm);  // verify required options

		if (vm.count("file"))
		{
			params->filename = filename;

			if (vm.count("show"))  // --show or -s option
				params->show_config = true;
			else
				params->show_config = false;
		}
		else
		{
			params->filename = "";
		}
	}
	catch(po::required_option& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		std::cerr << desc << std::endl;
		exit(EXIT_FAILURE);
	}
	catch(po::error& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		std::cerr << desc << std::endl;
		exit(EXIT_FAILURE);
	}
}

void usage()
{
	std::cout << "Usage:\n";
	std::cout << "\tavrs [options] [-f] <configuration file>\n";
}

void print_version()
{
	std::cout << "avrs " << VERSION_NUMBER << std::endl;
	std::cout << "Copyright (C) 2009-2014 Fabián Tommasini" << std::endl;
	std::cout << "                        and others" << std::endl;
	std::cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << std::endl;
	std::cout << "This is free software: you are free to change and redistribute it." << std::endl;
	std::cout << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
}

