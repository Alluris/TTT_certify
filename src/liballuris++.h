/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

C++ interface to liballuris.

Liballuris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Liballuris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with liballuris. See ../COPYING.LESSER
If not, see <http://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cmath>     //for round

//#define SILENT_LIBALLURIS
#include "liballuris.h"

#ifndef LIBALLURIS_CPP_H
#define LIBALLURIS_CPP_H

#define SERIAL_LEN 30
#define POLL_PACKET_SIZE 19

using namespace std;

class liballuris
{
private:
  libusb_context* usb_ctx;
  libusb_device_handle* usb_h;

public:
  liballuris ();
  liballuris (string serial);
  ~liballuris ();

  string get_serial_number ();
  string get_next_calibration_date ();
  string get_calibration_date ();
  string get_calibration_number ();
  double get_uncertainty ();

  int get_digits ();
  int get_resolution ();
  int get_F_max ();

  void set_cyclic_measurement (bool enable, int packet_length);

  vector<int> poll_measurement_no_wait ();
  void tare ();

  void start_measurement ();
  void stop_measurement ();

  void set_upper_limit (int limit);
  void set_lower_limit (int limit);
  void set_mode (enum liballuris_measurement_mode mode);
  void set_unit (enum liballuris_unit unit);
  void clear_RX (int timeout);

  void set_peak_level (double level);
  double get_peak_level ();
  void set_autostop (int t);
  int get_autostop ();
  void set_key_lock (bool active);

  void set_memory_mode (liballuris_memory_mode mode);
  vector<int> read_memory ();
  void clear_memory ();

};

#endif
