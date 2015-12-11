/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

Checks for cairo_drawing_functions.cpp

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

#include <stdio.h>
#include "cairo_drawing_functions.h"

using namespace std;

int main()
{
  sqlite3 *db;
  int rc;

  rc = sqlite3_open_v2 ("ttt_certify.db", &db, SQLITE_OPEN_READWRITE, 0);
  //printf ("sqlite3_open_v2 returned %i\n", rc);
  if(rc != SQLITE_OK)
    {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return -1;
    }

  report_result ret;
  ret = create_DIN6789_report (db, 1, "cairo_report_1.pdf");
  printf ("values_below_max_deviation = %i\n", ret.values_below_max_deviation);
  printf ("timing_violation           = %i\n", ret.timing_violation);

  ret = create_DIN6789_report (db, 2, "cairo_report_2.pdf");
  printf ("values_below_max_deviation = %i\n", ret.values_below_max_deviation);
  printf ("timing_violation           = %i\n", ret.timing_violation);

  //cairo_debug_reset_static_data ();

  sqlite3_close(db);
  return rc;
}
