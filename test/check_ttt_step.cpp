/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

Checks for TTTs steps

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

/*
 * Die Idee ist es später viele Geräte in die Datenbank zu schreiben und dafür
 * DIN 6789-1 Tests zu erzeugen und zu überprüfen
 */

#include <stdio.h>
#include <unistd.h>

#include <libintl.h>
#include <locale.h>

#define TEST_DEBUG_COUT
#include "ttt.h"

int main (int argc, char **argv)
{
  class ttt my (NULL, NULL, NULL, NULL, NULL, NULL, "ttt_certify.db");

  my.load_test_person (1);
  my.load_torque_tester ();

  int id = 1;
  if (argc == 2)
    id = atoi (argv[1]);
  else
    {
      cerr << "Usage: check_ttt_step TEST_OBJECT_RECORD_ID" << endl;
      return -1;
    }
  my.load_test_object (id);
  my.print_test_object ();

  my.add_DIN6789_steps (false);

  return 0;
}
