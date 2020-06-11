/*

Copyright (C) 2020 Alluris GmbH & Co. KG <weber@alluris.de>

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
#include <libintl.h>

//#define SILENT_LIBALLURIS
#include "liballuris.h"

#ifndef LIBALLURIS_CPP_H
#define LIBALLURIS_CPP_H

#define SERIAL_LEN 30
#define VARIANT_LEN 8
#define FIRMWARE_LEN 10
#define POLL_PACKET_SIZE 19

using namespace std;

class liballuris
{
private:
  libusb_context* usb_ctx;
  libusb_device_handle* usb_h;

public:
  string err_msg (int err);

  liballuris ();
  explicit liballuris (string serial);
  ~liballuris ();

  void set_debug_level (int l);

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
  int get_upper_limit ();
  void set_lower_limit (int limit);
  int get_lower_limit ();

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

  void set_data_ratio (int decimate);
  int get_pos_peak ();
  int get_neg_peak ();
  void clear_pos_peak ();
  void clear_neg_peak ();
  string get_variant ();
  string get_firmware ();

  liballuris_state read_state ();

  // Motorfunktionen
  void start_motor_reference_run (char start);
  void set_motor_disable(bool state);
  bool get_motor_enable ();

  void set_buzzer_motor (bool state);
  bool get_buzzer_motor ();

  void set_motor_start (uint8_t start);
  void set_motor_stopp (uint8_t state);
};

#endif
