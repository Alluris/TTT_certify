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
#include "confuse.h"
#include <unistd.h>

#ifndef WIN32
#include <sys/poll.h>
#endif

#include <libintl.h>

#include <locale.h>
#include "ttt.h"

void print_indicated_torque (double v)
{
  cout << "indicated torque = " << v << endl;
}

void print_nominal_torque (double v)
{
  cout << "nominal torque = " << v << endl;
}

void print_peak_torque (double v)
{
  static double old_v = -1e6;
  if (v != old_v)
    {
      cout << "peak torque = " << v << endl;
      old_v = v;
    }
}

void print_instruction (string s)
{
  cout << "instruction: " << s << endl;
}

void print_step (string s, double v)
{
  static double old_v = -1e6;
  if (v != old_v)
    {
      cout << "step (" << v*100 << "%): " << s << endl;
      old_v = v;
    }
}

void print_result (string s)
{
  cout << "result: " << s << endl;
}

/*
  Usage: ttt_sim TEST_OBJECT_RECORD_ID SIM_FN
*/
int main (int argc, char **argv)
{
  int to_id = 1;
  if (argc != 3)
    {
      cerr << "Usage: ttt_sim TEST_OBJECT_RECORD_ID SIM_FN" << endl;
      return -1;
    }

  to_id = atoi (argv[1]);

  // read setting with libconfuse
  static char *database = NULL;
  static double start_peak_torque_factor = 0.6;
  static double stop_peak_torque_factor = 0.1;

  cfg_opt_t opts[] =
  {
    CFG_SIMPLE_STR("database", &database),
    CFG_SIMPLE_FLOAT("start_peak_torque_factor", &start_peak_torque_factor),
    CFG_SIMPLE_FLOAT("stop_peak_torque_factor", &stop_peak_torque_factor),
    CFG_END()
  };
  cfg_t *cfg;

  /* set default value for the server option */
  database = strdup ("ttt_certify.db");
  cfg = cfg_init (opts, 0);
  cfg_parse (cfg, "../src/ttt_gui.conf");

  printf("database: %s\n", database);
  printf("start_peak_torque_factor: %f\n", start_peak_torque_factor);
  printf("stop_peak_torque_factor: %f\n", stop_peak_torque_factor);

  setlocale (LC_ALL, "");
  //bindtextdomain("ttt","/usr/share/locale");
  bindtextdomain("ttt","./po");
  textdomain ("ttt");

  //class ttt my (print_indicated_torque, print_nominal_torque, print_peak_torque, print_instruction, print_step, print_result, "ttt_certify.db");
  class ttt my (NULL, NULL, NULL, NULL, print_step, print_result, database, start_peak_torque_factor, stop_peak_torque_factor);

  my.load_test_person (1);
  my.load_test_object (to_id);
  my.load_torque_tester ();

  my.connect_measurement_input (argv[2]);
  cout << "Used simulation file = " << argv[2] << endl;
  my.start_sequencer_ISO6789 (21.23, 34.56, false, false);

  try
    {
      do
        usleep(100e3);
      while (my.run ());

      my.print_result ();
    }
  catch (exception& e)
    {
      cout << "Exception while running sequencer: " << e.what() << endl;
      my.disconnect_TTT ();
      return -1;
    }

  return 0;
}
