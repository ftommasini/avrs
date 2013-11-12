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

/**
 * @file tracker.hpp
 */

#ifndef TRACKER_HPP_
#define TRACKER_HPP_

#include <memory>
#include "common.hpp"

using namespace avrs;

/**
 * TrackerBase class
 */
class TrackerBase
{
public:
	typedef std::auto_ptr<TrackerBase> ptr_t;

	virtual void calibrate() = 0;
	virtual trackerdata_t get_data() = 0;  // pure virtual
    virtual void start() = 0;
    virtual void stop() = 0;
};


/**
 * TrackerConstant class
 * Useless, but for debugging purposes
 */
class TrackerConstant : public TrackerBase
{
public:
	typedef std::auto_ptr<TrackerConstant> ptr_t;

	virtual ~TrackerConstant();
	/// Static factory function for TrackerConstant objects
	static ptr_t create(position_t pos, orientation_angles_t ori, unsigned int read_interval_ms);

	void calibrate();
	trackerdata_t get_data();
    void start(); ///< start the tracking thread
    void stop();  ///< stop the tracking thread
private:
	TrackerConstant(position_t pos, orientation_angles_t ori, unsigned int read_interval_ms);

	trackerdata_t _data;

    /// initialize the tracker.
    /// @return @b true if successful, @b false otherwise
    bool _init();
    volatile bool _running; ///< stops the tracking thread

    unsigned int _read_interval_ms;

    // thread related stuff
    pthread_t _thread_id;
    static void* _threadWrapper(void*);
    void* _thread(void*);
};

inline void TrackerConstant::calibrate()
{
	;  // nothing to do
}

inline trackerdata_t TrackerConstant::get_data()
{
	return _data;
}


/**
 * TrackerSimulation class
 */
class TrackerSimulation : public TrackerBase
{
public:
	typedef std::auto_ptr<TrackerSimulation> ptr_t;

	typedef enum
	{
		from_file,
		calculation,
		cipic_measurements
	} sim_t;

	virtual ~TrackerSimulation();
	/// Static factory function for TrackerConstant objects
	static ptr_t create(sim_t sim, unsigned int read_interval_ms, std::string filename = "");

	void calibrate();
	trackerdata_t get_data();
    void start(); ///< start the tracking thread
    void stop();  ///< stop the tracking thread

private:
    TrackerSimulation(sim_t sim, unsigned int read_interval_ms, std::string filename = "");

	trackerdata_t _data;

	sim_t _sim_type;

	void simPositionCIPIC();
	void simPositionCalculatedAz();
	void simPositionCalculatedEl();
	void simPositionFromFile();

	FILE *_file;
	std::string _filename;

    /// initialize the tracker.
    /// @return @b true if successful, @b false otherwise
    bool _init();
    volatile bool _running; ///< stops the tracking thread

    unsigned int _read_interval_ms;

    // thread related stuff
    pthread_t _thread_id;
    static void* _threadWrapper(void*);
    void* _thread(void*);
};

inline void TrackerSimulation::calibrate()
{
	;  // nothing to do
}

inline trackerdata_t TrackerSimulation::get_data()
{
	return _data;
}

#endif  // TRACKER_HPP_
