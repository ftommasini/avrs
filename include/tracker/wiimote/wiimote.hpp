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

// Here the origin of image coordinates is at image center
static const int g_image_center_X = 512;
static const int g_image_center_Y = 384;

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

#endif  // WIIMOTE_HPP_
