/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

Definition of test_person, test_object, torque_tester, measurement_item
and measurement. Save/load from sqlite db and print to cairo_context.
See also see create_database.sql

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

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sqlite3.h>
#include <stdexcept>
#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>

#ifndef SQLITE_INTERFACE_H
#define SQLITE_INTERFACE_H

using namespace std;

double cairo_print_text (cairo_t *cr,
                         double left,
                         double top,
                         string text);

double cairo_print_two_columns (cairo_t *cr, double c1, double c2, double top,
                                string c1_lang1,
                                string c1_lang2,
                                string c2_lang);

double cairo_horizontal_line (cairo_t *cr, double c1, double top);
void cairo_centered_text (cairo_t *cr, double x, double y, const char *str);

// get localtime for now in %Y-%m-%d %H:%M:%S format for sqlite
string get_localtime ();

string subst_wildcards (string in);

class test_person
{

public:
  int id;
  string name;
  string supervisor;
  double uncertainty;

  test_person (): id(-1) {}
  void load_with_id (sqlite3 *db, int search_id);
  void save (sqlite3 *db);

  double cairo_print (cairo_t *cr, double c1, double c2, double top);

  friend ostream& operator<<(ostream& os, const test_person& d);
};

class test_object
{
public:
  int id;
  bool active;
  string equipment_number;
  string serial_number;
  string manufacturer;
  string model;
  string DIN_type;        // I oder II
  string DIN_class;       // A, B, C, D, E, F, G
  int dir_of_rotation;    // 0 = beide, 1 = nur rechtsdrehend, 2 = nur linksdrehend
  double lever_length;
  double min_torque;
  double max_torque;
  double resolution;      // Auflösung
  string attachments;     // Anbauteile
  double accuracy;        // Genauigkeit (besser "zulässige Messabweichung"), 0 = aus der DIN EN ISO 6789 bestimmen, sonst Herstellerangabe
  double peak_trigger2_factor;  // Detektionsschwelle für Typ II peak, typischerweise 0.8..0.9. Wenn == 0.0 wird der am TTT eingestellte Wert verwendet

  test_object(): id(-1), dir_of_rotation(-1), lever_length(-1), min_torque(-1), max_torque(-1), resolution(-1), accuracy(-1), peak_trigger2_factor(-1) {}
  void load_with_id (sqlite3 *db, int search_id);
  void save (sqlite3 *db);

  double cairo_print (cairo_t *cr, double c1, double c2, double top);

  friend ostream& operator<<(ostream& os, const test_object&);
  bool equal (const test_object &to);

  string get_type_class ()
  {
    return DIN_type + DIN_class;
  }

  bool is_type (int id)
  {
    return (   (id == 1 && ! DIN_type.compare ("I"))
               || (id == 2 && ! DIN_type.compare ("II")));
  }

  bool is_type (string id)
  {
    return (! DIN_type.compare (id));
  }

  bool is_type_class (string id)
  {
    return (! get_type_class().compare (id));
  }

  static bool is_screwdriver (string tc)
  {
    return (  ! tc.compare ("ID")
              || ! tc.compare ("IE")
              || ! tc.compare ("IID")
              || ! tc.compare ("IIE")
              || ! tc.compare ("IIF"));
  }

  bool is_screwdriver ()
  {
    return is_screwdriver (get_type_class ());
  }

  static bool has_no_scale (string tc)
  {
    return (  ! tc.compare ("IIB")
              || ! tc.compare ("IIC")
              || ! tc.compare ("IIE")
              || ! tc.compare ("IIF"));
  }

  bool has_no_scale ()
  {
    return  has_no_scale (get_type_class ());
  }

  static bool has_fixed_trigger (string tc)
  {
    return (  ! tc.compare ("IIB")
              || ! tc.compare ("IIE"));
  }

  bool has_fixed_trigger ()
  {
    return  has_fixed_trigger (get_type_class ());
  }

  static double get_accuracy_from_DIN (string tc, double max_torque);
  double get_accuracy_from_DIN ();
  double get_accuracy ()
  {
    double ret = accuracy;
    if (ret == 0)
      ret = get_accuracy_from_DIN ();
    return ret;
  }

  // return timing specification from DIN EN ISO 6789 chapter 6.24
  // for torque increase from 80% to 100%
  // screwdriver have min, max, others return 0 for max_t
  static void get_timing_from_DIN (string tc, double torque, double &min_t, double &max_t);
  void get_timing_from_DIN (double torque, double &min_t, double &max_t);

};

class torque_tester
{
public:
  int id;
  string serial_number;
  string manufacturer;
  string model;
  string next_calibration_date;
  string calibration_date;
  string calibration_number;

  double max_torque;
  double resolution;      // Skalenteilung?
  double uncertainty_of_measurement;

  torque_tester (): id(-1), max_torque(0), resolution(0), uncertainty_of_measurement(0) {}
  void load_with_id (sqlite3 *db, int search_id);
  int search_serial_and_next_cal_date (sqlite3 *db, string serial, string cal_date);
  void save (sqlite3 *db);

  double cairo_print (cairo_t *cr, double c1, double c2, double top);

  friend ostream& operator<<(ostream& os, const torque_tester&);
};

class measurement_item
{
public:
  string ts;  //timestamp
  double nominal_value;
  double indicated_value;
  double rise_time;         // Only TypII, see DIN EN ISO 6789-1 6.2.4

  // loading and saving of measurement_item
  // is done in measurement::load/save

//  double rel_deviation ()
//  {
//    return rel_deviation (nominal_value);
//  }

  double rel_deviation (double Xa)
  {
    // DIN EN ISO
    // 6789   chapter 6.4: (X_a - X_r) / X_r * 100
    // 6789-1 chapter 6.7: (X_a - X_r) / X_a * 100

#ifdef ISO6789_1
    return (indicated_value - Xa) / Xa;
#elif defined (ISO6789)
    return (Xa - indicated_value) / indicated_value;
#else
#error No ISO 6789 variant defined
#endif

  }


  bool is_in (double tolerance)
  {
    return abs (rel_deviation (nominal_value)) <= tolerance;
  }

  bool is_in (double tolerance, double Xa)
  {
    return abs (rel_deviation (Xa)) <= tolerance;
  }

  friend ostream& operator<<(ostream& os, const measurement_item&);
};

class measurement
{
private:
  vector<measurement_item *> measurement_items;

public:
  int id;

  string norm;
  test_person tp;
  test_object to;
  torque_tester tt;

  string start_time;
  string end_time;
  string raw_data_filename;
  double temperature;   // [°C]
  double humidity;      // [%rH]

  measurement ();
  ~measurement ();

  void add_measurement_item (string ts, double nominal_value, double indicated_value, double rise_time);
  void add_measurement_item (measurement_item *p);
  void clear_measurement_items ();

  void load_with_id (sqlite3 *db, int search_id);
  void save (sqlite3 *db);

  double total_uncertainty () //Messunsicherheit
  {
    double ret = sqrt ( tt.uncertainty_of_measurement * tt.uncertainty_of_measurement / 4
                        +tp.uncertainty * tp.uncertainty / 4);
    //std::cout << "total_uncertainty () returns " << ret << std::endl;
    return ret;
  }

  double cairo_print_1_meas_table (cairo_t *cr, double c1, double top, unsigned int first, unsigned int last, bool &values_below_max_deviation, bool &timing_violation);
  double cairo_print_5_meas_table (cairo_t *cr, double c1, double top, unsigned int first, unsigned int last, bool &values_below_max_deviation, bool &timing_violation);
  double cairo_print_header (cairo_t *cr, double c1, double c2, double top);
  double cairo_print (cairo_t *cr, double c1, double c2, double top, bool &values_below_max_deviation, bool &timing_violation);
  double cairo_print_conformity (cairo_t *cr, double c1, double top, bool &values_below_max_deviation);

  friend ostream& operator<<(ostream& os, const measurement&);

  bool quick_check_okay ();
};

enum test_object_search_field
{
  SERIAL,
  EQUIPMENTNR,
  MANUFACTURER,
  MODEL
};

void search_test_objects (sqlite3 *db, enum test_object_search_field field, string search_str, vector<test_object> &vto);
void set_test_object_active (sqlite3 *db, int id, bool active);
bool get_test_object_active (sqlite3 *db, int id);
void set_test_object_equipment_number (sqlite3 *db, int id, string equipment_number);
int search_active_adjacent_test_object (sqlite3 *db, int id);
bool test_object_has_measurement (sqlite3 *db, int id);

enum test_person_search_field
{
  NAME,
  SUPERVISOR
};

void search_test_persons (sqlite3 *db, enum test_person_search_field field, string s, vector<test_person> &vtp);

#endif
