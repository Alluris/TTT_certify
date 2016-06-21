/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

Checks for liballuris++.cpp

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with liballuris. See ../COPYING.LESSER
If not, see <http://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <unistd.h>
#include "liballuris++.h"

int main()
{
  try
    {
      class liballuris a;

      a.set_cyclic_measurement (0, 19);
      a.stop_measurement ();

      //class alluris a("P.26901");

      cout << "get_serial_number ()         = " << a.get_serial_number () << endl;
      cout << "get_next_calibration_date () = " << a.get_next_calibration_date () << endl;
      cout << "get_calibration_date ()      = " << a.get_calibration_date () << endl;
      cout << "get_calibration_number ()    = " << a.get_calibration_number () << endl;
      cout << "get_uncertainty ()           = " << a.get_uncertainty () << endl;
      cout << "get_digits ()                = " << a.get_digits () << endl;
      cout << "get_F_max ()                 = " << a.get_F_max () << endl;
      cout << "get_peak_level ()            = " << a.get_peak_level () << endl;
      //a.set_peak_level (0.8);
      //assert (a.get_peak_level () == 0.8);
      cout << "get_autostop ()              = " << a.get_autostop () << endl;

      sleep (0.1);
      a.set_autostop (30);
      //sleep (0.1);
      //assert (a.get_autostop () == 0);

      a.set_key_lock (1);
      sleep (1);
      a.set_key_lock (0);

      a.set_mode (LIBALLURIS_MODE_PEAK);
      a.start_measurement ();
      usleep (1e6);
      a.stop_measurement ();
      a.start_measurement ();
      a.set_cyclic_measurement (1, 19);

      for (int k=0; k<100; ++k)
        {
          usleep (10e3);
          vector<int> tmp = a.poll_measurement_no_wait ();
          cout << "got " << tmp.size () << " samples" << endl;
        }

      usleep (1e6);
      cout << "--------------------------------------------" << endl;
      for (int k=0; k<100; ++k)
        {
          vector<int> tmp = a.poll_measurement_no_wait ();
          cout << "got " << tmp.size () << " samples" << endl;
        }

      a.set_cyclic_measurement (0, 19);
      a.set_cyclic_measurement (1, 19);
      a.set_cyclic_measurement (0, 19);
      a.stop_measurement ();
    }
  catch (std::runtime_error &e)
    {
      cerr << "ERROR:" << e.what () << endl;
    };
  return 0;
}
