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

/**
 * @file tracker.hpp
 */

#ifndef TRACKERSIM_HPP_
#define TRACKERSIM_HPP_

#include <memory>

#include "tracker/trackerbase.hpp"

namespace avrs
{

/**
 * TrackerSim class
 */
class TrackerSim : public TrackerBase
{
public:
	typedef std::auto_ptr<TrackerSim> ptr_t;

	typedef enum
	{
		constant,
		calculated,
		from_file,
		cipic_angles,
		none
	} sim_t;

	virtual ~TrackerSim();
	/// Static factory function for TrackerConstant objects
	static ptr_t create(sim_t sim, unsigned int read_interval_ms, std::string filename = "");

	void calibrate();
	trackerdata_t get_data();
    void start(); ///< start the tracking thread
    void stop();  ///< stop the tracking thread

private:
    TrackerSim(sim_t sim, unsigned int read_interval_ms, std::string filename = "");

    /// initialize the tracker.
    /// @return @b true if successful, @b false otherwise
    bool _init();

	trackerdata_t _data;
	sim_t _sim_type;

	void sim_cipic_angles();
	void sim_from_file();
	void sim_calculated_az();
	void sim_calculated_el();
	void sim_constant();

	FILE *_file;
	std::string _filename;

    volatile bool _running; ///< stops the tracking thread
    unsigned int _read_interval_ms;

    // thread related stuff
    pthread_t _thread_id;
    static void* _threadWrapper(void*);
    void* _thread(void*);
};

inline trackerdata_t TrackerSim::get_data()
{
	return _data;
}

}  // namespace avrs

#endif  // TRACKERSIM_HPP_
