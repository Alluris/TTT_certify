/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

Tests for class ttt_device

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
#include <stdlib.h>
#include <unistd.h>
#include "ttt_device.h"

double sample_1s (ttt_device &td)
{
  cout << "collecting 900 samples..." << endl;
  vector<double> samples;
  do
    {
      vector<double> tmp = td.poll_measurement();
      samples.insert(samples.end(), tmp.begin(), tmp.end());
    }
  while (samples.size () < 900);

  double sum = 0;
  for(std::vector<double>::iterator it = samples.begin(); it != samples.end(); ++it)
    sum += *it;

  return sum/samples.size ();
}

int main()
{
  ttt_device td;

  //normal usage
  /*
    td.start ();

    for (int k=0; k<10; ++k)
      cout << sample_1s (td) << endl;

    td.tare ();

    for (int k=0; k<10; ++k)
      cout << sample_1s (td) << endl;

    td.stop ();
  */

  // polling without starting
  /*
    vector<double> tmp = td.poll_measurement();
    for (int k=0; k<100; ++k)
      tmp = td.poll_measurement();
  */


//many start/stop/tare

  td.start ();
  td.start ();
  td.tare ();
  cout << sample_1s (td) << endl;
  td.start ();
  cout << sample_1s (td) << endl;
  cout << sample_1s (td) << endl;
  td.stop ();
  td.tare ();

  return EXIT_SUCCESS;
}
