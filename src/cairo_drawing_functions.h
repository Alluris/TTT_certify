/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

Create calibration reports using cairo

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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <string>
#include "sqlite_interface.h"

#ifndef CREATE_CAIRO_REPORT_H
#define CREATE_CAIRO_REPORT_H

struct report_result
{
  bool values_below_max_deviation;  // Alle Werte innerhalb der zulässigen Abweichung
  bool timing_violation;
};

report_result create_DIN6789_report ( sqlite3 *db,
                                      int measurement_id,
                                      const char *report_fn,
                                      bool repeat_on_tolerance_violation);

#endif
