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

#ifndef TRACKERWIIMOTE_HPP_
#define TRACKERWIIMOTE_HPP_

#include <pthread.h>
#include <memory>
#include <string>
#include <cwiid.h>

#include "tracker/wiimote/posit.hpp"
#include "tracker/wiimote/wiimote.hpp"
#include "tracker/trackerbase.hpp"
#include "common.hpp"

namespace avrs
{

class TrackerWiimote : public TrackerBase
{
public:
	typedef std::auto_ptr<TrackerWiimote> ptr_t;

	/// Destructor
	virtual ~TrackerWiimote();

	/// Named constructor
	static ptr_t create(std::string object, std::string address,
			const unsigned int read_interval = 10);

	void calibrate();
	avrs::trackerdata_t get_data();
    void start(); ///< start the tracking thread
    void stop();  ///< stop the tracking thread

	// Debug purposes
	void printRotation(double rotation[][3]);
	void printPoints(const ir_points_t *ir);

private:
	/// Constructor
	TrackerWiimote(std::string object, std::string address,
			const unsigned int read_interval);

	const int _focal_length;  ///< Focal length in pixels

	//Controller& _controller;
	Wiimote::ptr_t _wm;

    avrs::trackerdata_t _current_position;
    avrs::trackerdata_t _ref_position;
    avrs::trackerdata_t _corrected_position;

    std::string _object_name;
    avrs::trackerdata_t _data;
    unsigned int _read_interval_ms;

    // POSIT
    object_t _object;
	image_t _image;
    camera_t _camera;

    void _posit(ir_points_t *ir_points, ir2object_points_t *ir2obj);

    /// interval in ms to wait after each read cycle
    unsigned int _read_interval;

    bool _running; ///< stops the tracking thread

    /// initialize the tracker.
    /// @return @b true if successful, @b false otherwise
    void _init();

    // thread related stuff
    pthread_t _thread_id;
    static void* _thread_wrapper(void*);
    void* _thread(void*);
};

inline avrs::trackerdata_t TrackerWiimote::get_data()
{
	return _corrected_position;
}

}  // namespace avrs

#endif  // TRACKERWIIMOTE_HPP_
