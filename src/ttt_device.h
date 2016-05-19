/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

class ttt_device: TTT (tool torque tester) hardware interface

This file is part of TTT_certify.

TTT_certify is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TTT_certify is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  See ../COPYING
If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef TTT_DEVICE_H
#define TTT_DEVICE_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <libintl.h>
#include <sstream>
#include "liballuris++.h"

using namespace std;

#define TTT_SERIAL_LEN 30
#define TTT_PACKET_SIZE 19

class ttt_device
{
private:
  liballuris al;

  void init ();

  // settings
  double scale;
  double resolution;
  double max_M;
  string serial;
  string next_cal_date;
  string cal_date;
  string cal_number;
  double uncertainty;   //relative erweiterte Messunsicherheit (k=2)
  double peak_level;
  bool measuring;
  bool streaming;

public:
  ttt_device ();
  ttt_device (string serial);
  ~ttt_device ();

  void check_error (int err);

  string get_serial ()
  {
    return serial;
  }

  string get_next_cal_date ()
  {
    return next_cal_date;
  }

  string get_cal_date ()
  {
    return cal_date;
  }

  string get_cal_number ()
  {
    return cal_number;
  }

  double get_uncertainty ()
  {
    return uncertainty;
  }

  double get_peak_level ()
  {
    return peak_level;
  }

  double get_max_torque ()
  {
    return max_M;
  }

  string get_model ()
  {
    if (max_M == 2)
      return "TTT-300B2";
    else if (max_M == 5)
      return "TTT-300B5";
    else if (max_M == 10)
      return "TTT-300C1";
    else if (max_M == 25)
      return "TTT-300C2";
    else if (max_M == 50)
      return "TTT-300C5";
    return "unknown";
  }

  double get_resolution ()
  {
    return resolution;
  }

  void start ();
  void stop ();
  void tare ();
  vector<double> poll_measurement ();
};

#endif
