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
 * @file wiimote.cpp
 */

#include "tracker/wiimote/wiimote.hpp"

Wiimote::Wiimote(std::string address)
	: 	_wii_remote(NULL)
{
	str2ba(address.c_str(), &_bluetooth_address);
}

Wiimote::~Wiimote()
{
	this->_disconnect();
}

Wiimote::ptr_t Wiimote::create(std::string address)
{
	ptr_t w(new Wiimote(address));
	g_ptrWiimote = w.get();

	if (!w->_connect())
	{
		w.release(); /// TODO is it correct?
		return w;  // NULL????
	}
	else
		return w;
}

bool Wiimote::_connect()
{
	/* Establish a continous and non-blocking connection */
	// CWIID_FLAG_CONTINUOUS | CWIID_FLAG_NONBLOCK
	_wii_remote = cwiid_connect(&_bluetooth_address, CWIID_FLAG_MESG_IFC);

	if (_wii_remote == 0)
	{
		return false; // failed to connect
	}

	/* Power on LED 2 + 3 */
	if (cwiid_command(_wii_remote, CWIID_CMD_LED, CWIID_LED2_ON | CWIID_LED3_ON)
			!= 0)
	{
		this->_disconnect();
		return false;
	}

	/* Enable host-features: Infrared light sensor*/
	if (cwiid_command(_wii_remote, CWIID_CMD_RPT_MODE,
			CWIID_RPT_IR | CWIID_RPT_BTN) != 0)
	{
		this->_disconnect();
		return false;
	}

	if (cwiid_get_state(_wii_remote, &_wii_state) != 0)
	{
		_disconnect();
		return -1;
	}

	if (cwiid_set_mesg_callback(_wii_remote, &Wiimote::wm_callback) != 0)
	{
		this->_disconnect();
		return false;
	}

	return true;  // everything is OK...
}

bool Wiimote::_disconnect()
{
	if (_wii_remote)
	{
		if (cwiid_disconnect(_wii_remote) != 0)
		{
			return false;
		}
	}

	return true;
}

void Wiimote::wm_callback(cwiid_wiimote_t *g_wiimote, int mesg_count,
		union cwiid_mesg mesg[], struct timespec *timestamp)
{
	static Wiimote* wm = (Wiimote *) g_ptrWiimote;
	static ir_points_t ir;
	static int ir_sizes[2] = { 3, 3 };  // Expect it to be around 2-8 in wiimote state struct
	static int detected_points;

	for (int j = 0; j < mesg_count; j++)
	{
		if ((mesg[j].type == CWIID_MESG_IR) && (g_wiimote))  // the only message that is important
		{
			detected_points = 0;

			// Get IR values
			for (int i = 0; i < 4; i++)
			{
				if (mesg[j].ir_mesg.src[i].valid)
				{
					detected_points++;
					// Get the point positions
					ir.data[i][0] = mesg[j].ir_mesg.src[i].pos[CWIID_X]
							- g_image_center_X;
					ir.data[i][1] = mesg[j].ir_mesg.src[i].pos[CWIID_Y]
							- g_image_center_Y;

					if (mesg[j].ir_mesg.src[i].size != -1)
					{
						ir_sizes[i] = mesg[j].ir_mesg.src[i].size + 1;  // TODO why this???
					}
				}

				//g_current_time = timestamp->tv_nsec;  // TODO mutex??
			}

			ir.timestamp = timestamp->tv_nsec;
			wm->setIrPoints(ir);

			if (detected_points < 4)  // missed some points
				;  // do nothing (for now...)
		}
	}
}
