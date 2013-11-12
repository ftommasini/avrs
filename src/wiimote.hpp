/**
 * @file wiimote.hpp
 */

#ifndef WIIMOTE_HPP_
#define WIIMOTE_HPP_

#include <memory>
#include <string>

#include <cwiid.h>

// Some mix between C and C++ because callbacks stuff

typedef struct IRPoints {
	int data[4][2];
	long int timestamp;

	IRPoints()
	{
		// Initialize IR points to 0
		for (int i = 0; i < 4; i++)
			for (int j = 0; i < 2; i++)
				data[i][j] = 0;
	}
} ir_points_t;

// File scope (static)
//static long int g_current_time;
//static ir_points_t g_ir_points;
// Here the origin of image coordinates is at image center
static const int g_image_center_X = 512;
static const int g_image_center_Y = 384;

// Callback prototype
//void wiimote_callback(cwiid_wiimote_t *g_wiimote, int mesg_count,
//		union cwiid_mesg mesg[], struct timespec *timestamp);

static void *g_ptrWiimote = 0;

/**
 * Wrap class for CWiid
 */
class Wiimote
{
public:
	typedef std::auto_ptr<Wiimote> ptr_t;

	/// Destructor
	virtual ~Wiimote();

	/// "Named constructor"
	static ptr_t create(std::string address);

	int getBattery();
	void setIrPoints(ir_points_t ir);
	ir_points_t getIrPoints();
//	void setCurrentTime(long int time);
//	long int getCurrentTime();

	static void wm_callback(cwiid_wiimote_t *g_wiimote, int mesg_count,
			union cwiid_mesg mesg[], struct timespec *timestamp);
private:
	/// Constructor
	Wiimote(std::string address);

	bool _connect();
	bool _disconnect();

	bdaddr_t _bluetooth_address;		///< Bluetooth address-structure
	cwiid_wiimote_t *_wii_remote; 		///< Handles our wiimote connection
	struct cwiid_state _wii_state; 		///< Capture this state on every frame

	long int _current_time;
	ir_points_t _ir_points;
};

inline int Wiimote::getBattery()
{
	if (!_wii_remote)
		return -1;

	return (int) (100.0 * _wii_state.battery / CWIID_BATTERY_MAX);
}

inline void Wiimote::setIrPoints(ir_points_t ir)
{
	_ir_points = ir;
}

inline ir_points_t Wiimote::getIrPoints()
{
	return _ir_points;
}

//inline void Wiimote::setCurrentTime(long int time)
//{
//	_current_time = time;
//}
//
//inline long int Wiimote::getCurrentTime()
//{
//	return _current_time;
//}

#endif  // WIIMOTE_HPP_