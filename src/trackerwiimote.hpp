/**
 * @file trackerwiimote.h
 */

#ifndef TRACKERWIIMOTE_HPP_
#define TRACKERWIIMOTE_HPP_

#include <pthread.h>
#include <memory>
#include <string>

#include <cwiid.h>

#include "posit.hpp"
#include "wiimote.hpp"
#include "common.hpp"
#include "tracker.hpp"

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

#endif  // TRACKERWIIMOTE_HPP_
