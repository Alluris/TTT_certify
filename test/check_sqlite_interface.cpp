/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

Checks for sqlite_interface.cpp

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
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <string>
#include "sqlite_interface.h"
#include <unistd.h>

using namespace std;

int main ()
{
  test_object to;
  /*********** CHECK get_accuracy_from_DIN, is_screwdriver, has_no_scale, has_fixed_trigger **********/
  double min_t, max_t;

  to.DIN_type = "I";
  to.DIN_class = "A";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.06);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (! to.is_screwdriver ());
  assert (! to.has_no_scale ());
  assert (! to.has_fixed_trigger ());

  /*
    ISO 6789-1
    to.get_timing_from_DIN (5, min_t, max_t);
    assert (min_t == 0.5 && max_t == 0);
    to.get_timing_from_DIN (50, min_t, max_t);
    assert (min_t == 1.0 && max_t == 0);
    to.get_timing_from_DIN (500, min_t, max_t);
    assert (min_t == 1.5 && max_t == 0);
    to.get_timing_from_DIN (1500, min_t, max_t);
    assert (min_t == 2 && max_t == 0);
  */

  //ISO 6789
  to.get_timing_from_DIN (5, min_t, max_t);
  assert (min_t == 0.5 && max_t == 4);
  to.get_timing_from_DIN (50, min_t, max_t);
  assert (min_t == 0.5 && max_t == 4);
  to.get_timing_from_DIN (500, min_t, max_t);
  assert (min_t == 0.5 && max_t == 4);
  to.get_timing_from_DIN (1500, min_t, max_t);
  assert (min_t == 0.5 && max_t == 4);


  to.DIN_type = "I";
  to.DIN_class = "B";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.04);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (! to.is_screwdriver ());
  assert (! to.has_no_scale ());
  assert (! to.has_fixed_trigger ());
  to.get_timing_from_DIN (99, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 1.0 && max_t == 0);
  */

  // ISO 6789:
  assert (min_t == 0.5 && max_t == 4);

  to.DIN_type = "I";
  to.DIN_class = "C";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.04);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (! to.is_screwdriver ());
  assert (! to.has_no_scale ());
  assert (! to.has_fixed_trigger ());
  to.get_timing_from_DIN (100, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 1.5 && max_t == 0);
  */

  // ISO 6789
  assert (min_t == 0.5 && max_t == 4);

  to.DIN_type = "I";
  to.DIN_class = "D";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.06);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (to.is_screwdriver ());
  assert (! to.has_no_scale ());
  assert (! to.has_fixed_trigger ());
  to.get_timing_from_DIN (10, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 0.5 && max_t == 1.0);
  */
  // ISO 6789
  assert (min_t == 0.5 && max_t == 4.0);

  to.DIN_type = "I";
  to.DIN_class = "E";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.04);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (to.is_screwdriver ());
  assert (! to.has_no_scale ());
  assert (! to.has_fixed_trigger ());
  to.get_timing_from_DIN (200, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 0.5 && max_t == 1.0);
  */
  // ISO6789
  assert (min_t == 0.5 && max_t == 4.0);

  to.DIN_type = "II";
  to.DIN_class = "A";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.04);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (! to.is_screwdriver ());
  assert (! to.has_no_scale ());
  assert (! to.has_fixed_trigger ());
  to.get_timing_from_DIN (10, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 1.0 && max_t == 0.0);
  */
  // ISO6789
  assert (min_t == 0.5 && max_t == 4.0);

  to.DIN_type = "II";
  to.DIN_class = "B";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.04);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (! to.is_screwdriver ());
  assert (to.has_no_scale ());
  assert (to.has_fixed_trigger ());
  to.get_timing_from_DIN (100, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 1.5 && max_t == 0.0);
  */
  // ISO6789
  assert (min_t == 0.5 && max_t == 4.0);

  to.DIN_type = "II";
  to.DIN_class = "C";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.04);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (! to.is_screwdriver ());
  assert (to.has_no_scale ());
  assert (! to.has_fixed_trigger ());
  to.get_timing_from_DIN (10, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 1.0 && max_t == 0.0);
  */
  // ISO 6789
  assert (min_t == 0.5 && max_t == 4.0);

  to.DIN_type = "II";
  to.DIN_class = "D";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.06);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (to.is_screwdriver ());
  assert (! to.has_no_scale ());
  assert (! to.has_fixed_trigger ());
  to.get_timing_from_DIN (10, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 0.5 && max_t == 1.0);
  */
  // ISO 6789
  assert (min_t == 0.5 && max_t == 4.0);

  to.DIN_type = "II";
  to.DIN_class = "E";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.06);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (to.is_screwdriver ());
  assert (to.has_no_scale ());
  assert (to.has_fixed_trigger ());
  to.get_timing_from_DIN (10, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 0.5 && max_t == 1.0);
  */
  // ISO 6789
  assert (min_t == 0.5 && max_t == 4.0);

  to.DIN_type = "II";
  to.DIN_class = "F";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.06);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (to.is_screwdriver ());
  assert (to.has_no_scale ());
  assert (! to.has_fixed_trigger ());
  to.get_timing_from_DIN (10, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 0.5 && max_t == 1.0);
  */
  // ISO 6789
  assert (min_t == 0.5 && max_t == 4.0);

  to.DIN_type = "II";
  to.DIN_class = "G";
  to.max_torque = 11;
  assert (to.get_accuracy_from_DIN() == 0.06);
  to.max_torque = 10;
  assert (to.get_accuracy_from_DIN() == 0.06);
  assert (! to.is_screwdriver ());
  assert (! to.has_no_scale ());
  assert (! to.has_fixed_trigger ());
  to.get_timing_from_DIN (1000, min_t, max_t);
  /* ISO 6789-1
  assert (min_t == 2 && max_t == 0.0);
  */
  // ISO 6789
  assert (min_t == 0.5 && max_t == 4.0);

  // init db

  sqlite3 *db;
  int rc;

  rc = sqlite3_open("ttt_certify.db", &db);
  if( rc )
    {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return -1;
    }

  sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);

  /*********** CHECK test_object save/load cycle **********/

  to.active = true;
  to.equipment_number     = "ZX4";
  to.serial_number        = "XYZ 12345";
  to.manufacturer         = "ACME";
  to.model                = "super special tool";
  to.DIN_type             = "II";
  to.DIN_class            = "B";
  to.dir_of_rotation      = 0;    // 0 = beide, 1 = nur rechtsdrehend, 2 = nur linksdrehend
  to.lever_length         = 0.2345;
  to.min_torque           = 5;
  to.max_torque           = 50;
  to.resolution           = 0.5;
  to.attachments          = "my super attachment";
  to.accuracy             = 0.03;
  to.peak_trigger2_factor = 0.8;

  to.save (db);
  printf ("to rowid = %i\n", to.id);

  // read back
  test_object to2;
  to2.load_with_id (db, to.id);
  assert (to.equal (to2));

  // check set/get active
  int del_id = 14;
  set_test_object_active (db, del_id, false);
  assert (get_test_object_active (db, del_id) == false);

  // 14 is disabled so 13 should be adjacent
  int adj_id = search_active_adjacent_test_object (db, del_id);
  assert (adj_id == 13);

  // also disable 13
  set_test_object_active (db, 13, false);
  assert (get_test_object_active (db, 13) == false);

  // 13 and 14 are disabled so 15 should be the next adjacent
  adj_id = search_active_adjacent_test_object (db, del_id);
  assert (adj_id == 15);

  set_test_object_active (db, del_id, true);
  assert (get_test_object_active (db, del_id) == true);

  // check test_object_has_measurement
  assert (test_object_has_measurement (db, 1) == true);
  assert (test_object_has_measurement (db, 16) == true);
  assert (test_object_has_measurement (db, 2) == false);
  assert (test_object_has_measurement (db, 15) == false);
  assert (test_object_has_measurement (db, 17) == false);

  /*********** CHECK test_person save/load cycle **********/
  test_person tp;
  tp.name  = "Max Mustermann";
  tp.supervisor = "a very important person";
  tp.uncertainty = 0.123;
  // TODO: implement equal, add assert
  tp.save (db);
  printf ("tp rowid = %i\n", tp.id);

  /*********** CHECK torque_tester save/load cycle **********/
  torque_tester tt;
  tt.serial_number    = "L123456";
  tt.manufacturer     = "ACME";
  tt.model            = "xyz model";
  tt.next_calibration_date = "in two years";
  tt.calibration_date = "cal. yesterday";
  tt.calibration_number = "foo_xx_1234";
  tt.max_torque       = 100.34;
  tt.resolution       = 0.1;
  tt.uncertainty_of_measurement = 0.01;

  tt.save (db);
  printf ("tt rowid = %i\n", tt.id);

  int t_id = tt.search_serial_and_next_cal_date (db, "L123456", "in two years");
  printf ("t_id = %i\n", t_id);
  assert (tt.id == t_id);

  // TODO: implement equal, add assert

  /*********** CHECK measurement save/load cycle **********/
  measurement mm;

  mm.norm = "foobar";
  mm.tp = tp;
  mm.tt = tt;
  mm.to = to;

  mm.start_time = get_localtime ();
  sleep (1);
  mm.end_time = get_localtime ();
  mm.raw_data_filename = "foo.bar";
  mm.temperature = 21.234;
  mm.humidity = 45.67;

  // create some measurement_items
  for (int k=0; k<5; ++k)
    mm.add_measurement_item (get_localtime(), k*11, k*22, k/10.0);

  mm.save (db);

  /********** CHECK search_test_objects ******************/
  vector<test_object> vto;
  search_test_objects (db, SERIAL, "FAKE%", vto);
  assert (vto.size () == 16); //id 13 is still diabled

  vto.clear ();
  search_test_objects (db, MODEL, "7%", vto);
  assert (vto.size () == 1);

  vto.clear ();
  search_test_objects (db, MANUFACTURER, "PROX%", vto);
  assert (vto.size () == 2);


  /******** CHECK measurement statistics ******************/
  mm.load_with_id (db, 1);

  cout << mm.norm << endl;
  // display found test_objects with "PROX%"
  //unsigned int k;
  //for (k=0; k<vto.size(); ++k)
  //  cout << vto.at (k) << endl;

  sqlite3_close(db);

  return 0;
}
