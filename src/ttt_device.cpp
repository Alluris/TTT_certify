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

#include "ttt_device.h"

ttt_device::ttt_device ()
  : scale(-1), digits (-1), uncertainty(-1), peak_level(-1), measuring(false), streaming(false),
    old_autostop(-1)
{
  cout << "ttt_device c'tor" << endl;
  init ();
}

ttt_device::ttt_device (string serial)
  : al(serial), scale(-1), resolution(-1), uncertainty(-1), measuring(false), streaming(false)
{
  cout << "ttt_device c'tor serial = " << serial << endl;
  init ();
}

void ttt_device::init ()
{
  cout << "ttt_device::init clear_RX" << endl;
  al.clear_RX (500);
  cout << "ttt_device::init stop streaming and measurement" << endl;
  al.set_cyclic_measurement (0, TTT_PACKET_SIZE);
  //al.clear_RX (200);
  al.stop_measurement ();
  //al.clear_RX (200);

  cout << "ttt_device::init set defaults (900Hz, no upper and no lower limit)" << endl;

  // lock keypad
  al.set_key_lock (1);

  // save autostop so that it can be restored
  old_autostop = al.get_autostop ();

  // disable autostop
  al.set_autostop (0);

  al.set_memory_mode (LIBALLURIS_MEM_MODE_DISABLED);
  al.set_mode (LIBALLURIS_MODE_PEAK);
  al.set_unit (LIBALLURIS_UNIT_N);
  al.set_upper_limit (0);
  al.set_lower_limit (0);

  serial = al.get_serial_number ();
  digits = al.get_digits ();
  scale = 1.0 / pow(10, digits);

  next_cal_date = al.get_next_calibration_date ();
  cal_date = al.get_calibration_date ();
  cal_number = al.get_calibration_number ();

  max_M = al.get_F_max ();
  uncertainty = al.get_uncertainty ();
  peak_level = al.get_peak_level ();
  resolution = pow(10, -digits) * al.get_resolution ();

  cout << "ttt_device::init" << endl;
  cout << "  serial            = " << serial << endl;
  cout << "  next_cal_date     = " << next_cal_date << endl;
  cout << "  cal_date          = " << cal_date << endl;
  cout << "  cal_number        = " << cal_number << endl;
  cout << "  scale             = " << scale << endl;
  cout << "  resolution        = " << resolution << endl;
  cout << "  max_M             = " << max_M << endl;
  cout << "  uncertainty (k=2) = " << uncertainty << endl;
  cout << "  peak_level        = " << peak_level << endl;
  cout << "  old_autostop      = " << old_autostop << endl;
}

ttt_device::~ttt_device ()
{
  stop ();
  al.set_key_lock (0);

  // if the value for autostop was lost, use a default of 3s
  if (old_autostop == 0)
    old_autostop = 3;

  al.set_autostop (old_autostop);

  cout << "ttt_device d'tor finished" << endl;
}

void ttt_device::start ()
{
  if (! measuring)
    {
      cout << "ttt_device::start: start measurement" << endl;
      al.start_measurement ();
      measuring = true;
    }

  if (! streaming)
    {
      // enable streaming
      cout << "ttt_device::start: start streaming" << endl;
      al.set_cyclic_measurement (1, TTT_PACKET_SIZE);
      streaming = true;
    }
}

void ttt_device::stop ()
{
  if (streaming)
    {
      al.set_cyclic_measurement (0, TTT_PACKET_SIZE);
      streaming = false;
    }

  if (measuring)
    {
      al.stop_measurement ();
      measuring = false;
    }
}

void ttt_device::tare ()
{
  cout << "ttt_device::tare ()" << endl;

  bool was_streaming = streaming;
  if (streaming)
    {
      cout << "ttt_device::tare stop streaming ()" << endl;
      al.set_cyclic_measurement (0, TTT_PACKET_SIZE);
      streaming = false;
    }

  al.tare ();

  if (was_streaming)
    {
      // re-enable streaming
      cout << "ttt_device::tare start streaming ()" << endl;
      al.set_cyclic_measurement (1, TTT_PACKET_SIZE);
      streaming = true;
    }
}

vector<double> ttt_device::poll_measurement ()
{
  // don't check if measurement is running.
  // returns an empty vector if not.
  vector<int> tmp = al.poll_measurement_no_wait ();
  //cout << "ttt_device::poll_measurement got " << tmp.size () << " samples" << endl;

  std::vector<double> samples;
  for (unsigned int k=0; k<tmp.size (); ++k)
    samples.push_back (tmp[k] * scale);
  return samples;
}
