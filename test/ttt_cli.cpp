/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

TTT (tool torque tester) CLI (commandline interface)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  See ../COPYING
If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <unistd.h>

#ifndef WIN32
#include <sys/poll.h>
#endif

#include <libintl.h>

#include <locale.h>
#include "ttt.h"

void print_indicated_torque (double v)
{
  printf ( gettext ("indicated torque = %5.2f\n"), v);
}

void print_nominal_torque (double v)
{
  printf ( gettext ("nominal torque = %5.2f\n"), v);
}

void print_peak_torque (double v)
{
  printf ( gettext ("peak torque = %5.2f\n"), v);
}

void print_instruction (string s)
{
  printf ( gettext ("instruction: %s\n"), s.c_str ());
}

void print_step (string s, double v)
{
  printf ( gettext ("step (%4.1f%%): %s\n"), v * 100, s.c_str ());
}

void print_result (string s)
{
  printf ( gettext ("result: %s\n"), s.c_str ());
}

int main ()
{
  /*
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
  */

  setlocale (LC_ALL, "");
  //bindtextdomain("ttt","/usr/share/locale");
  bindtextdomain("ttt","./po");
  textdomain ("ttt");

  static double start_peak_torque_factor = 0.6;
  static double stop_peak_torque_factor = 0.1;
  class ttt my (print_indicated_torque, print_nominal_torque, print_peak_torque, print_instruction, print_step, print_result, "ttt_certify.db", start_peak_torque_factor, stop_peak_torque_factor);

  my.load_test_person (1);
  my.load_test_object (1);
  my.load_torque_tester ();

  //my.connect_measurement_input ("./test/create_test_signal/IA_min2.1_max10.1.log");
  //my.connect_measurement_input ("./test/create_test_signal/IIA_min2.1_max10.1.log");

  my.connect_TTT ();
  //my.connect_TTT ("L.12345");

  my.add_DIN6789_steps (false);

  // für simple_click_min0.0_max10.0.log
  //my.connect_measurement_input ("./test/create_test_signal/simple_click_min0.0_max10.0.log");
  //my.add_step (new preload_torque_tester_step (10));
  //my.add_step (new preload_test_object_step (10));
  //my.add_step (new peak_click_step (10, 0.5, 10, false));
  //my.add_step (new peak_meas_step (10));

  my.start_sequencer(21.23, 34.56);

  try
    {

#ifndef WIN32

      struct pollfd fds;
      int tret;
      fds.fd = 0;
      fds.events = POLLIN;

      char reply; //send "c" to abort capturing
      do
        {
          //use return as confirmation
          tret = poll (&fds, 1, 0);
          if (tret == 1)
            {
              reply = getc(stdin);
              if  (reply==10)
                my.set_confirmation();
            }
          usleep(100e3);
        }
      while (my.run ());
#else
//no confirmation
      do
        {
          usleep(100e3);
        }
      while (my.run ());
#endif

      my.print_result ();

    }
  catch (exception& e)
    {
      cout << "Exception while running sequencer: " << e.what() << endl;
      my.disconnect_TTT ();
    }

  return 0;
}
