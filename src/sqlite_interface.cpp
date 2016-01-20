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

#include <float.h>
#include <time.h>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include "sqlite_interface.h"
#include "cairo_print_devices.h"

double cairo_print_text (cairo_t *cr,
                         double left,
                         double top,
                         string text)
{
  cairo_font_extents_t fe;
  cairo_font_extents (cr, &fe);

  int start = 0;
  size_t i = string::npos;
  string sub;
  do
    {
      i = text.find ("\n", start);
      sub = text.substr (start, i);
      text = text.substr (i+1);

      //cout << "---" << sub << "---" << endl;

      cairo_move_to (cr, left, top);
      cairo_show_text (cr, sub.c_str ());
      top += fe.height;

    }
  while (i != string::npos);

  return top;
}

double cairo_print_two_columns (cairo_t *cr, double c1, double c2, double top,
                                string c1_lang1,
                                string c1_lang2,
                                string c2_lang)
{
  cairo_font_extents_t fe;
  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_font_extents (cr, &fe);

  if (! c1_lang1.empty ())
    {
      cairo_move_to (cr, c1, top);
      cairo_show_text (cr, c1_lang1.c_str ());
    }

  if (! c2_lang.empty ())
    {
      cairo_move_to (cr, c2, top);
      cairo_show_text (cr, c2_lang.c_str ());
    }

  top += fe.height;
  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);


  if (! c1_lang2.empty ())
    {
      cairo_move_to (cr, c1, top);
      cairo_show_text (cr, c1_lang2.c_str ());
    }
  top += 1.5 * fe.height;
  return top;
}

double cairo_print_legend (cairo_t *cr, double c1, double c2, double top,
                           string formula,
                           string lang1,
                           string lang2)
{
  cairo_font_extents_t fe;
  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_font_extents (cr, &fe);

  cairo_move_to (cr, c1, top);
  cairo_show_text (cr, formula.c_str ());

  cairo_move_to (cr, c2, top);
  cairo_show_text (cr, lang1.c_str ());

  top += fe.height;
  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);

  cairo_move_to (cr, c2, top);
  cairo_show_text (cr, lang2.c_str ());

  top += 1.5 * fe.height;
  return top;
}

double cairo_horizontal_line (cairo_t *cr, double c1, double top)
{
  cairo_set_line_width (cr, 0.02);
  cairo_move_to (cr, c1, top);
  cairo_line_to (cr, c1 + 18, top);
  cairo_stroke (cr);
  return top + 1;
}

void cairo_centered_text (cairo_t *cr, double x, double y, const char *str)
{
  cairo_text_extents_t extents;
  cairo_text_extents (cr, str, &extents);

  x = x - (extents.width/2 + extents.x_bearing);
  y = y - (extents.height/2 + extents.y_bearing);

  cairo_move_to (cr, x, y);
  cairo_show_text (cr, str);
}

string get_localtime ()
{
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
  return buffer;
}

string subst_wildcards (string in)
{
  std::replace( in.begin(), in.end(), '*', '%');
  std::replace( in.begin(), in.end(), '?', '_');
  return in;
}

void test_person::load_with_id (sqlite3 *db, int search_id)
{
  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2 (db, "SELECT * FROM test_person WHERE rowid = ?1", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      rc = sqlite3_bind_int (pStmt, 1, search_id);
      if (rc == SQLITE_OK)
        {
          rc = sqlite3_step (pStmt);
          if (rc == SQLITE_ROW)
            {
              id = sqlite3_column_int (pStmt, 0);
              name = (const char*) sqlite3_column_text (pStmt, 1);
              supervisor = (const char*) sqlite3_column_text (pStmt, 2);
              uncertainty = sqlite3_column_double (pStmt, 3);
            }
          sqlite3_finalize(pStmt);
          if (rc == SQLITE_DONE)
            {
              throw runtime_error ("No test_person with given id");
            }
        }
      else
        fprintf(stderr, "SQL error from sqlite3_bind_int: %i = %s\n", rc, sqlite3_errmsg(db));
    }
  else
    fprintf(stderr, "SQL error from sqlite3_prepare_v2: %i = %s\n", rc, sqlite3_errmsg(db));
}

void test_person::save (sqlite3 *db)
{
  cout << "test_person::save" << endl;
  cout << *this;

  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2 (db, "INSERT INTO test_person (name, supervisor, uncertainty)"
                               "VALUES (?1, ?2, ?3);", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      sqlite3_bind_text (pStmt, 1, name.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 2, supervisor.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_double (pStmt, 3, uncertainty);
      rc = sqlite3_step (pStmt);
      sqlite3_finalize(pStmt);
      if (rc != SQLITE_DONE)
        {
          fprintf(stderr, "test_person::save sqlite3_step failed %i = %s\n", rc, sqlite3_errmsg(db));
          throw runtime_error ("test_person::save sqlite3_step failed");
        }

      id = sqlite3_last_insert_rowid (db);
    }
  else
    {
      fprintf(stderr, "test_person::save sqlite3_prepare_v2 failed %i = %s\n", rc, sqlite3_errmsg(db));
      throw runtime_error ("test_person::save sqlite3_prepare_v2 failed");
    }
}

double test_person::cairo_print (cairo_t *cr, double c1, double c2, double top)
{
  //source, font size, face and so on has to be initialized
  ostringstream os;
  os << id;

  //top = cairo_print_two_columns (cr, c1, c2, top, "Prüfer id", "inspector id", os.str ());
  top = cairo_print_two_columns (cr, c1, c2, top, "Prüfer", "name", name);
  top = cairo_print_two_columns (cr, c1, c2, top, "Verantwortlicher", "supervisor", supervisor);

  ostringstream os_uncertainty;
  os_uncertainty << uncertainty * 100 << " %";

  top = cairo_print_two_columns (cr, c1, c2, top, "Messunsicherheit", "uncertainty", os_uncertainty.str ());
  return top;
}

void test_object::load_with_id (sqlite3 *db, int search_id)
{
  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2 (db, "SELECT * FROM test_object WHERE rowid = ?1", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      rc = sqlite3_bind_int (pStmt, 1, search_id);
      if (rc == SQLITE_OK)
        {
          rc = sqlite3_step (pStmt);
          if (rc == SQLITE_ROW)
            {
              id = sqlite3_column_int (pStmt, 0);
              active = sqlite3_column_int (pStmt, 1);
              equipment_number = (const char*) sqlite3_column_text (pStmt, 2);
              serial_number = (const char*) sqlite3_column_text (pStmt, 3);
              manufacturer = (const char*) sqlite3_column_text (pStmt, 4);
              model = (const char*) sqlite3_column_text (pStmt, 5);
              DIN_type = (const char*) sqlite3_column_text (pStmt, 6);
              DIN_class = (const char*) sqlite3_column_text (pStmt, 7);
              dir_of_rotation = sqlite3_column_int (pStmt, 8);
              lever_length = sqlite3_column_double (pStmt, 9);
              min_torque = sqlite3_column_double (pStmt, 10);
              max_torque = sqlite3_column_double (pStmt, 11);
              resolution = sqlite3_column_double (pStmt, 12);
              attachments = (const char*) sqlite3_column_text (pStmt, 13);
              accuracy = sqlite3_column_double (pStmt, 14);
            }
          sqlite3_finalize(pStmt);
          if (rc == SQLITE_DONE)
            {
              throw runtime_error ("No test_object with given id");
            }
        }
      else
        fprintf(stderr, "SQL error from sqlite3_bind_int: %i = %s\n", rc, sqlite3_errmsg(db));
    }
  else
    fprintf(stderr, "SQL error from sqlite3_prepare_v2: %i = %s\n", rc, sqlite3_errmsg(db));
}

void test_object::save (sqlite3 *db)
{
  cout << "test_object::save" << endl;
  cout << *this;

  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2 (db, "INSERT INTO test_object (active, serial_number, equipment_number, manufacturer, model, DIN_type, "
                               "DIN_class, dir_of_rotation, lever_length, min_torque, max_torque, resolution, attachments, accuracy)"
                               "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14);", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      sqlite3_bind_int (pStmt, 1, active);
      sqlite3_bind_text (pStmt, 2, serial_number.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 3, equipment_number.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 4, manufacturer.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 5, model.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 6, DIN_type.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 7, DIN_class.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_int (pStmt, 8, dir_of_rotation);
      sqlite3_bind_double (pStmt, 9, lever_length);
      sqlite3_bind_double (pStmt, 10, min_torque);
      sqlite3_bind_double (pStmt, 11, max_torque);
      sqlite3_bind_double (pStmt, 12, resolution);
      sqlite3_bind_text (pStmt, 13, attachments.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_double (pStmt, 14, accuracy);
      rc = sqlite3_step (pStmt);
      sqlite3_finalize(pStmt);
      if (rc != SQLITE_DONE)
        {
          fprintf(stderr, "test_object::save sqlite3_step failed %i = %s\n", rc, sqlite3_errmsg(db));
          throw runtime_error ("test_object::save sqlite3_step failed");
        }

      id = sqlite3_last_insert_rowid (db);
    }
  else
    {
      fprintf(stderr, "test_object::save sqlite3_prepare_v2 failed %i = %s\n", rc, sqlite3_errmsg(db));
      throw runtime_error ("test_object::save sqlite3_prepare_v2 failed");
    }
}

double test_object::cairo_print (cairo_t *cr, double c1, double c2, double top)
{
  //source, font size, face and so on has to be initialized
  top = cairo_print_two_columns (cr, c1, c2, top, "Gegenstand", "Object", model);
  top = cairo_print_two_columns (cr, c1, c2, top, "Hersteller", "Manufacturer", manufacturer);
  top = cairo_print_two_columns (cr, c1, c2, top, "Seriennummer", "Serial number", serial_number);
  top = cairo_print_two_columns (cr, c1, c2, top, "Prüfmittelnummer", "Test equipment number", equipment_number);

  double scale = 14;
  cairo_save (cr);
  cairo_translate (cr, c1, top);
  cairo_scale (cr, scale, scale);
  cairo_set_line_width (cr, 0.03/scale);   //0.3mm

  string tmp = DIN_type + DIN_class;
  top += 1 + scale * cairo_print_str (cr, tmp.c_str ());    // Print device
  cairo_restore (cr);

  string cat_type = DIN_type + " Klasse " + DIN_class;
  top = cairo_print_two_columns (cr, c1, c2, top, "Typ", "Type", cat_type);

  // Funktionsrichtung
  string str_rot;
  switch (dir_of_rotation)
    {
    case 0:
      str_rot = "rechts-/ linksdrehend";
      break;
    case 1:
      str_rot = "rechtsdrehend";
      break;
    case 2:
      str_rot = "linksdrehend";
      break;
    default:
      break;
    }
  top = cairo_print_two_columns (cr, c1, c2, top, "Funktionsrichtung(en)", "direction of rotation", str_rot);

  // Don't print lever length for screwdrivers
  if (! is_screwdriver ())
    {
      ostringstream os_lever_length;
      os_lever_length << lever_length << " m";
      top = cairo_print_two_columns (cr, c1, c2, top, "Abstand Kraftangriffspunkt", "lever length", os_lever_length.str ());
    }

  // Don't print min torque if it has a fixed trigger
  if (! has_fixed_trigger ())
    {
      ostringstream os_min;
      os_min << min_torque << " Nm";
      top = cairo_print_two_columns (cr, c1, c2, top, "Unterer Grenzwert", "Min torque", os_min.str ());
    }

  ostringstream os_max;
  os_max << max_torque << " Nm";
  top = cairo_print_two_columns (cr, c1, c2, top, "Oberer Grenzwert", "Max torque", os_max.str ());

  // Print resolution if device has a scale
  if (! has_no_scale ())
    {
      ostringstream os_resolution;
      os_resolution << resolution << " Nm";
      top = cairo_print_two_columns (cr, c1, c2, top, "Auflösung", "Resolution", os_resolution.str ());
    }

  ostringstream os_accuracy;
  if (accuracy == 0)
    {
      accuracy = get_accuracy_from_DIN ();
      os_accuracy << accuracy * 100 << " % (gemäß DIN 6789-1)";
    }
  else
    os_accuracy << accuracy * 100 << " % (gemäß Herstellerangabe)";

  top = cairo_print_two_columns (cr, c1, c2, top, "höchstzulässige Abweichung", "maximum deviation", os_accuracy.str ());

  top = cairo_print_two_columns (cr, c1, c2, top, "Anbauteile", "attachments", attachments);

  return top;
}

/*
 * DIN EN ISO 6789.1:2015
 * 5.1.5 Table 3, 4, 5
 */
double test_object::get_accuracy_from_DIN (string tc, double max_torque)
{
  if (! tc.compare ("IA") || ! tc.compare ("ID"))
    return 0.06;
  else if (! tc.compare ("IB") || ! tc.compare ("IC") || ! tc.compare ("IE"))
    return (max_torque > 10)? 0.04 : 0.06;
  else if (! tc.compare ("IIA"))
    return (max_torque > 10)? 0.04 : 0.06;
  else if (! tc.compare ("IID") || ! tc.compare ("IIG"))
    return 0.06;
  else if (! tc.compare ("IIB") | ! tc.compare ("IIC"))
    return (max_torque > 10)? 0.04 : 0.06;
  else if (! tc.compare ("IIE") || ! tc.compare ("IIF"))
    return 0.06;
  else
    throw runtime_error ("Unknown DIN_type and DIN_class combination");
}

double test_object::get_accuracy_from_DIN ()
{
  return test_object::get_accuracy_from_DIN (get_type_class (), max_torque);
}

// return timing specification from DIN EN ISO 6789 chapter 6.24
// for torque increase from 80% to 100%
// screwdriver have min, max, others return 0 for max_t

void test_object::get_timing_from_DIN (string tc, double torque, double &min_t, double &max_t)
{
  if (test_object::is_screwdriver (tc))
    {
      min_t = 0.5;
      max_t = 1.0;
    }
  else
    {
      max_t = 0;
      if (torque < 10.0)
        min_t = 0.5;

      else if (torque < 100.0)
        min_t = 1.0;

      else if (torque < 1000.0)
        min_t = 1.5;

      else
        min_t = 2.0;
    }
}

void test_object::get_timing_from_DIN (double torque, double &min_t, double &max_t)
{
  get_timing_from_DIN (get_type_class (), torque, min_t, max_t);
}

void torque_tester::load_with_id (sqlite3 *db, int search_id)
{
  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2 (db, "SELECT * FROM torque_tester WHERE rowid = ?1", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      rc = sqlite3_bind_int (pStmt, 1, search_id);
      if (rc == SQLITE_OK)
        {
          rc = sqlite3_step (pStmt);
          if (rc == SQLITE_ROW)
            {
              id = sqlite3_column_int (pStmt, 0);
              serial_number = (const char*) sqlite3_column_text (pStmt, 1);
              manufacturer = (const char*) sqlite3_column_text (pStmt, 2);
              model = (const char*) sqlite3_column_text (pStmt, 3);
              next_calibration_date = (const char*) sqlite3_column_text (pStmt, 4);
              calibration_date = (const char*) sqlite3_column_text (pStmt, 5);
              calibration_number = (const char*) sqlite3_column_text (pStmt, 6);
              max_torque = sqlite3_column_double (pStmt, 7);
              resolution = sqlite3_column_double (pStmt, 8);
              uncertainty_of_measurement = sqlite3_column_double (pStmt, 9);
            }
          sqlite3_finalize(pStmt);
          if (rc == SQLITE_DONE)
            {
              throw runtime_error ("No test_object with given id");
            }
        }
      else
        fprintf(stderr, "SQL error from sqlite3_bind_int: %i = %s\n", rc, sqlite3_errmsg(db));
    }
  else
    fprintf(stderr, "SQL error from sqlite3_prepare_v2: %i = %s\n", rc, sqlite3_errmsg(db));
}

int torque_tester::search_serial_and_next_cal_date (sqlite3 *db, string serial, string next_cal_date)
{
  sqlite3_stmt *pStmt;
  int tmp_id = -1;
  int rc = sqlite3_prepare_v2 (db, "SELECT id FROM torque_tester WHERE serial_number = ?1 AND next_calibration_date = ?2", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      sqlite3_bind_text (pStmt, 1, serial.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 2, next_cal_date.c_str (), -1, SQLITE_STATIC);
      rc = sqlite3_step (pStmt);
      if (rc == SQLITE_ROW)
        tmp_id = sqlite3_column_int (pStmt, 0);

      rc = sqlite3_step (pStmt);
      if (rc != SQLITE_DONE)
        throw runtime_error ("torque_tester::search_serial_and_next_cal_date more than one torqe_tester with given serial and next_cal_date");

      sqlite3_finalize(pStmt);
    }
  else
    fprintf(stderr, "SQL error from sqlite3_prepare_v2: %i = %s\n", rc, sqlite3_errmsg(db));
  return tmp_id;
}

void torque_tester::save (sqlite3 *db)
{
  cout << "torque_tester::save" << endl;
  cout << *this;

  // check if one with same serial and calibration_date already exists
  int tmp_id = search_serial_and_next_cal_date (db, serial_number, next_calibration_date);
  if (tmp_id != -1)
    throw runtime_error ("torque_tester::save torque_tester with given serial and next_calibration_date already exists");

  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2 (db, "INSERT INTO torque_tester (serial_number, manufacturer, model, next_calibration_date, calibration_date, "
                               "calibration_number, max_torque, resolution, uncertainty_of_measurement)"
                               "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9);", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      sqlite3_bind_text (pStmt, 1, serial_number.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 2, manufacturer.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 3, model.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 4, next_calibration_date.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 5, calibration_date.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 6, calibration_number.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_double (pStmt, 7, max_torque);
      sqlite3_bind_double (pStmt, 8, resolution);
      sqlite3_bind_double (pStmt, 9, uncertainty_of_measurement);
      rc = sqlite3_step (pStmt);
      sqlite3_finalize(pStmt);
      if (rc != SQLITE_DONE)
        {
          fprintf(stderr, "torque_tester::save sqlite3_step failed %i = %s\n", rc, sqlite3_errmsg(db));
          throw runtime_error ("torque_tester::save sqlite3_step failed");
        }

      id = sqlite3_last_insert_rowid (db);
    }
  else
    {
      fprintf(stderr, "torque_tester::save sqlite3_prepare_v2 failed %i = %s\n", rc, sqlite3_errmsg(db));
      throw runtime_error ("torque_tester::save sqlite3_prepare_v2 failed");
    }
}

double torque_tester::cairo_print (cairo_t *cr, double c1, double c2, double top)
{
  //source, font size, face and so on has to be initialized
  top = cairo_print_two_columns (cr, c1, c2, top, "Normale", "Standards", model);
  top = cairo_print_two_columns (cr, c1, c2, top, "Hersteller", "Manufacturer", manufacturer);
  top = cairo_print_two_columns (cr, c1, c2, top, "Seriennummer", "Serial number", serial_number);
  top = cairo_print_two_columns (cr, c1, c2, top, "Nächstes Kalibrierdatum", "next calibration date", next_calibration_date);
  top = cairo_print_two_columns (cr, c1, c2, top, "Kalibrierdatum", "calibration date", calibration_date);
  top = cairo_print_two_columns (cr, c1, c2, top, "Kalibriernummer", "calibration number", calibration_number);

  ostringstream os_max;
  os_max << max_torque << " Nm";
  top = cairo_print_two_columns (cr, c1, c2, top, "Oberer Grenzwert", "Max torque", os_max.str ());

  ostringstream os_resolution;
  os_resolution << resolution << " Nm";
  top = cairo_print_two_columns (cr, c1, c2, top, "Skalenteilung", "Resolution", os_resolution.str ());

  ostringstream os_uncertainty_of_measurement;
  os_uncertainty_of_measurement << uncertainty_of_measurement * 100 << " %";
  top = cairo_print_two_columns (cr, c1, c2, top, "Messunsicherheit", "uncertainty of measurement", os_uncertainty_of_measurement.str ());

  return top;
}

measurement::measurement ()
  : id(-1), temperature(0), humidity(0)
{


}

measurement::~measurement ()
{
  clear_measurement_item ();
  //cout << "measurement d'tor" << endl;
}

void measurement::add_measurement_item (string ts, double nominal_value, double indicated_value, double rise_time)
{
  measurement_item *p = new measurement_item;
  p->ts = ts;
  p->nominal_value = nominal_value;
  p->indicated_value = indicated_value;
  p->rise_time = rise_time;

  measurement_items.push_back (p);
}

void measurement::add_measurement_item (measurement_item *p)
{
  measurement_items.push_back (p);
}

void measurement::clear_measurement_item ()
{
  for (unsigned int k=0; k<measurement_items.size (); ++k)
    delete measurement_items[k];
  measurement_items.clear ();
}

/*
 * load measurement with test_person, test_object, torque_tester
 */
void measurement::load_with_id (sqlite3 *db, int search_id)
{
  sqlite3_stmt *pStmt;

  int test_person_id = -1;
  int test_object_id = -1;
  int torque_tester_id = -1;

  // FIMXE: replace *
  int rc = sqlite3_prepare_v2 (db, "SELECT * FROM measurement WHERE rowid = ?1", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      rc = sqlite3_bind_int (pStmt, 1, search_id);
      if (rc == SQLITE_OK)
        {
          rc = sqlite3_step (pStmt);
          if (rc == SQLITE_ROW)
            {
              id = sqlite3_column_int (pStmt, 0);
              test_person_id = sqlite3_column_int (pStmt, 1);
              test_object_id = sqlite3_column_int (pStmt, 2);
              torque_tester_id = sqlite3_column_int (pStmt, 3);
              start_time = (const char*) sqlite3_column_text (pStmt, 4);
              end_time = (const char*) sqlite3_column_text (pStmt, 5);
              raw_data_filename = (const char*) sqlite3_column_text (pStmt, 6);
              temperature = sqlite3_column_double (pStmt, 7);
              humidity = sqlite3_column_double (pStmt, 8);
            }
          sqlite3_finalize(pStmt);
          if (rc == SQLITE_DONE)
            {
              throw runtime_error ("No measurement with given id");
            }
        }
      else
        fprintf(stderr, "SQL error from sqlite3_bind_int: %i = %s\n", rc, sqlite3_errmsg(db));
    }
  else
    fprintf(stderr, "SQL error from sqlite3_prepare_v2: %i = %s\n", rc, sqlite3_errmsg(db));

  // load test_person, test_object, torque_tester
  tp.load_with_id (db, test_person_id);
  to.load_with_id (db, test_object_id);
  tt.load_with_id (db, torque_tester_id);

  // load measurement_items
  rc = sqlite3_prepare_v2 (db, "SELECT ts,nominal_value,indicated_value,rise_time FROM measurement_item "
                           "WHERE measurement = ?1 ORDER BY ts", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      rc = sqlite3_bind_int (pStmt, 1, id);
      if (rc == SQLITE_OK)
        {
          do
            {
              rc = sqlite3_step (pStmt);
              if (rc == SQLITE_ROW)
                {
                  measurement_item *tmp = new measurement_item;
                  tmp->ts = (const char*) sqlite3_column_text (pStmt, 0);
                  tmp->nominal_value = sqlite3_column_double (pStmt, 1);
                  tmp->indicated_value = sqlite3_column_double (pStmt, 2);
                  tmp->rise_time = sqlite3_column_double (pStmt, 3);
                  measurement_items.push_back (tmp);

                  cout << *tmp << endl;
                }
            }
          while (rc != SQLITE_DONE);
          sqlite3_finalize(pStmt);
        }
      else
        fprintf(stderr, "SQL error from sqlite3_bind_int: %i = %s\n", rc, sqlite3_errmsg(db));
    }
  else
    fprintf(stderr, "SQL error from sqlite3_prepare_v2: %i = %s\n", rc, sqlite3_errmsg(db));
}

void measurement::save (sqlite3 *db)
{
  // check if we should create a new test_person entry
  if (tp.id == 0)
    tp.save (db);

  // check if we should create a new torque_tester entry
  if (tt.id == 0)
    tt.save (db);

  // check if we should create a new test_object entry
  if (to.id == 0)
    to.save (db);

  srand (time(NULL) + to.id);

  sqlite3_stmt *pStmt;
  sqlite3_exec(db, "BEGIN;", 0, 0, 0);
  // insert measurement
  int rc = sqlite3_prepare_v2 (db, "INSERT INTO measurement"
                               "(test_person_id, test_object_id, torque_tester_id, start_time,"
                               "end_time, raw_data_filename, temperature, humidity)"
                               "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8);", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      cout << "measurement::save test_person.id = " << tp.id << endl;
      cout << "measurement::save test_object.id = " << to.id << endl;
      cout << "measurement::save torque_tester.id = " << tt.id << endl;

      sqlite3_bind_int (pStmt, 1, tp.id);
      sqlite3_bind_int (pStmt, 2, to.id);
      sqlite3_bind_int (pStmt, 3, tt.id);
      sqlite3_bind_text (pStmt, 4, start_time.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 5, end_time.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_text (pStmt, 6, raw_data_filename.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_double (pStmt, 7, temperature);
      sqlite3_bind_double (pStmt, 8, humidity);


      for (int j=0; j<5; ++j)
        {
          rc = sqlite3_step (pStmt);
          if (rc == SQLITE_BUSY)
            {
              // wait up to 2s
              int delay = round (rand () / (RAND_MAX + 1.0) * 2e6);
              cout << "Database is busy, wait for " << delay/1.0e3 << " ms..." << endl;
              usleep (delay);
            }
          else
            break;
        }

      sqlite3_finalize (pStmt);
      if (rc != SQLITE_DONE)
        {
          fprintf(stderr, "measurement::save sqlite3_step for measurement failed %i = %s\n", rc, sqlite3_errmsg(db));
          throw runtime_error ("measurement::save sqlite3_step for measurement failed");
        }

      id = sqlite3_last_insert_rowid (db);
    }
  else
    {
      fprintf(stderr, "measurement::save sqlite3_prepare_v2 for measurement failed %i = %s\n", rc, sqlite3_errmsg(db));
      throw runtime_error ("measurement::save sqlite3_prepare_v2 for measurement failed");
    }

  // insert measurement_items
  rc = sqlite3_prepare_v2 (db, "INSERT INTO measurement_item"
                           "(ts, measurement, nominal_value, indicated_value, rise_time)"
                           "VALUES (?1, ?2, ?3, ?4, ?5);", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      for (unsigned int k=0; k<measurement_items.size (); ++k)
        {
          sqlite3_bind_text (pStmt, 1, measurement_items[k]->ts.c_str (), -1, SQLITE_STATIC);
          sqlite3_bind_int (pStmt, 2, id);
          sqlite3_bind_double (pStmt, 3, measurement_items[k]->nominal_value);
          sqlite3_bind_double (pStmt, 4, measurement_items[k]->indicated_value);
          sqlite3_bind_double (pStmt, 5, measurement_items[k]->rise_time);

          rc = sqlite3_step (pStmt);
          if (rc != SQLITE_DONE)
            {
              fprintf(stderr, "measurement::save sqlite3_step for measurement_items failed %i = %s\n", rc, sqlite3_errmsg(db));
              throw runtime_error ("measurement::save sqlite3_step for measurement_items failed");
            }
          sqlite3_reset (pStmt);
        }
      sqlite3_finalize(pStmt);
    }
  else
    {
      fprintf(stderr, "measurement::save sqlite3_prepare_v2 for measurement_items failed %i = %s\n", rc, sqlite3_errmsg(db));
      throw runtime_error ("measurement::save sqlite3_prepare_v2 for measurement_items failed");
    }
  sqlite3_exec(db, "COMMIT;", 0, 0, 0);
}

// 5 Messungen pro Nominalwert
double measurement::cairo_print_5_meas_table (cairo_t *cr, double c1, double top, unsigned int first, unsigned int last, bool &values_below_max_deviation, bool &timing_violation)
{
  cairo_font_extents_t fe;
  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_font_extents (cr, &fe);

  double header_height = 1.5;
  double width = 18.0;
  int columns = 8;  //Drehmoment, 5 Messwerte, zulässige Abweichung, Messunsicherheit, Bewertung

  // brechnete Werte
  double col_width = width/columns;
  double col_height = 2.8 * fe.height;

  // draw table header
  cairo_move_to (cr, c1, top);
  cairo_line_to (cr, c1 + width, top);
  cairo_stroke (cr);
  cairo_move_to (cr, c1, top + header_height);
  cairo_line_to (cr, c1 + width, top + header_height);
  cairo_stroke (cr);

  // nominal_value muss monoton sein, das sollte dank ORDER BY in der
  // SELECT Anweisung der Fall sein

  double old_nominal_value = 0;
  int row = 0;
  int col = 0;
  int cnt_per_nominal = 0;
  double y;
  bool confirm = 1;
  bool new_row = 1;

  double mean = 0;
  for (unsigned int k = first; k <= last ; ++k)
    mean += measurement_items[k]->indicated_value;
  mean = mean / measurement_items.size ();

  //cout << "first=" << first << " last=" << last << endl;

  for (unsigned int k = first; k <= last ; ++k)
    {
      double nominal_value = measurement_items[k]->nominal_value;
      // Typ IIC und IIF verwenden arithmetisches Mittel
      if (to.has_no_scale () && ! to.has_fixed_trigger ())
        nominal_value = mean;

      cairo_set_source_rgb (cr, 0, 0, 0);
      cout << "new_row=" << new_row
           << " nominal_value=" << nominal_value
           << " old_nominal_value=" << old_nominal_value
           << " row=" << row
           << " col=" << col << endl;

      if (nominal_value != old_nominal_value)
        {
          new_row = 1;
          cnt_per_nominal = 0;
        }

      if (new_row)
        {
          row++;
          col = 0;

          y = top + header_height + row * col_height;
          cairo_move_to (cr, c1, y);
          cairo_line_to (cr, c1 + width, y);
          cairo_stroke (cr);

          char str[40];
          if (nominal_value != old_nominal_value)
            {
              // erste Spalte mit nominal_value
              snprintf (str, 40, "%.1f Nm", nominal_value);
              cairo_centered_text (cr, c1 + col_width/2, y - col_height / 2, str);
            }

          // Gesamte Standardmessunsicherheit
          snprintf (str, 40, "%.2f Nm", abs (total_uncertainty () * nominal_value));
          cairo_centered_text (cr, c1 + width - 1.5 * col_width, y - col_height / 2, str);
          //snprintf (str, 40, "%.2f %%", total_uncertainty () * 100);
          //cairo_centered_text (cr, c1 + width - 1.5 * col_width, y - 1 * col_height / 3, str);

          new_row = 0;
        }

      // Bewertung generieren
      double accuracy = to.accuracy;
      if (accuracy == 0)
        accuracy = to.get_accuracy_from_DIN ();

      if (! measurement_items[k]->is_in (accuracy, nominal_value))
        {
          cairo_set_source_rgb (cr, 1, 0, 0);
          confirm = 0;
          values_below_max_deviation = 0;
        }

      // rise_time überprüfen
      bool rise_time_okay = 1;

      // rise_time ist nur für TypII interessant
      // Siehe DIN 6789-1 Kapitel 6.2.4
      if (to.is_type (2))
        {
          double rise_time = measurement_items[k]->rise_time;
          double min_rise_time;
          double max_rise_time;
          to.get_timing_from_DIN (measurement_items[k]->nominal_value, min_rise_time, max_rise_time);
          bool rise_time_okay = (rise_time >= min_rise_time && (rise_time<= max_rise_time || max_rise_time == 0));

          if (! rise_time_okay)
            timing_violation = 1;
        }

      //cout << "min_rise_time=" << min_rise_time << " max_rise_time=" << max_rise_time << " rise_time=" << rise_time << endl;

      // eigentliche Messwerte einfüllen
      char str[40];
      snprintf (str, 40, "%.2f Nm%s", measurement_items[k]->indicated_value, (rise_time_okay) ? "": "*");
      cairo_centered_text (cr, c1 + col_width * (col + 1.5), y - 2 * col_height / 3, str);

      //  Abweichung
      snprintf (str, 40, "(%.2f %%)", measurement_items[k]->rel_deviation (nominal_value) * 100);
      cairo_centered_text (cr, c1 + col_width * (col + 1.5), y - col_height / 3, str);

      cnt_per_nominal++;
      col++;

      // 5 Messungen je Nominalwert
      if (   (! to.has_no_scale () && cnt_per_nominal == 5)
             || (to.has_no_scale () && cnt_per_nominal == 10))
        {
          strncpy (str, (confirm)? "pass" : "fail", 40);
          cairo_set_source_rgb (cr, (confirm)? 0:1, 0, 0);
          cairo_centered_text (cr, c1 + width - col_width/2, y - col_height / 2, str);
          cairo_set_source_rgb (cr, 0, 0, 0);
          confirm = 1;
        }

      if (to.has_no_scale () && cnt_per_nominal == 5)
        {
          new_row = 1;
        }

      if (col > 5)
        throw runtime_error ("measurement::cairo_print_5_meas_table more than 5 columns");

      old_nominal_value = nominal_value;
    }

  // senkrechte Linien zeichnen
  for (int k=0; k < (columns + 1); ++k)
    {
      if (k < 2 || k > 5)
        cairo_move_to (cr, c1 + k * col_width, top);
      else
        cairo_move_to (cr, c1 + k * col_width, top + header_height - col_height);

      cairo_line_to (cr, c1 + k * col_width, top + header_height + row * col_height);
      cairo_stroke (cr);

    }

  // X1.. X5
  for (int k=1; k <= 5; ++k)
    {
      char str[8];
      if (cnt_per_nominal>5)
        snprintf (str, 8, "Xr%i/%i", k, k+5 );
      else
        snprintf (str, 8, "Xr%i", k );

      cairo_centered_text (cr, c1 + col_width * (k + 0.5), top + header_height - 2 * col_height / 3, str);
      cairo_centered_text (cr, c1 + col_width * (k + 0.5), top + header_height - col_height / 3, "(a  )");
      cairo_centered_text (cr, c1 + col_width * (k + 0.5) + 0.15, top + header_height - col_height / 3 + 0.1, "s");
    }

  cairo_centered_text (cr, c1 + col_width/2, top + header_height - col_height / 2, "Xa");

  /*
  cairo_save (cr);
  cairo_move_to (cr, c1 + col_width/3, top + header_height - 0.2);
  cairo_rotate (cr, -M_PI/2);
  cairo_show_text (cr, "Nennwert");
  cairo_restore (cr);
  */

  cairo_centered_text (cr, c1 + width - 1.5 * col_width, top + header_height - col_height / 2, "u");

  //cairo_save (cr);
  //cairo_move_to (cr, c1 + width - 1.5 * col_width, top + header_height - 0.3);
  //cairo_rotate (cr, -M_PI/2);
  //cairo_print_text (cr, - (top + header_height - 0.3), c1 + width - 1.5 * col_width, "Messunsicherheit");
  //cairo_show_text (cr, "Messunsicherheit");
  //cairo_restore (cr);

  /*
  cairo_save (cr);
  cairo_rotate (cr, -M_PI/2);
  cairo_print_text (cr, - (top + header_height - 0.3), c1 + width - 0.6 * col_width, "Bewertung\nconfirmation");
  cairo_restore (cr);
  */

  //cairo_move_to (cr, c1 + 3 * col_width, top + header_height/3);
  //cairo_show_text (cr, "Bezugswert (Xr)");

  top += header_height + (row + 0.5) * col_height;
  return top;
}

// Ein Messwert pro Zeile
double measurement::cairo_print_1_meas_table (cairo_t *cr, double c1, double top, unsigned int first, unsigned int last, bool &values_below_max_deviation, bool &timing_violation)
{
  cairo_font_extents_t fe;
  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_font_extents (cr, &fe);

  double header_height = 2 * fe.height;
  double width = 18.0;
  int columns = 5;  //Nennwert, Bezugswert, Abweichung, Standardmessunsicherheit, Bewertung

  // brechnete Werte
  double col_width = width/columns;
  double col_height = 1.5 * fe.height;

  // draw table header
  cairo_move_to (cr, c1, top);
  cairo_line_to (cr, c1 + width, top);
  cairo_stroke (cr);
  cairo_move_to (cr, c1, top + header_height);
  cairo_line_to (cr, c1 + width, top + header_height);
  cairo_stroke (cr);

  // nominal_value muss monoton sein, das sollte dank ORDER BY in der
  // SELECT Anweisung der Fall sein

  double old_nominal_value = 0;
  int row = 0;
  double y;

  double mean = 0;
  for (unsigned int k = first; k <= last ; ++k)
    mean += measurement_items[k]->indicated_value;
  mean = mean / measurement_items.size ();

  cout << "first=" << first << " last=" << last << " mean=" << mean <<  endl;

  for (unsigned int k = first; k <= last ; ++k)
    {

      double nominal_value = measurement_items[k]->nominal_value;
      // Typ IIC und IIF verwenden arithmetisches Mittel
      if (to.has_no_scale () && ! to.has_fixed_trigger ())
        nominal_value = mean;

      cairo_set_source_rgb (cr, 0, 0, 0);
      cout << " nominal_value=" << nominal_value
           << " old_nominal_value=" << old_nominal_value
           << " row=" << row
           << " indicated_value = " << measurement_items[k]->indicated_value << endl;

      row++;

      y = top + header_height + row * col_height;
      cairo_move_to (cr, c1, y);
      cairo_line_to (cr, c1 + width, y);
      cairo_stroke (cr);

      // erste Spalte mit nominal_value
      char str[40];
      snprintf (str, 40, "%.2f Nm", nominal_value);
      cairo_centered_text (cr, c1 + col_width/2, y - col_height / 2, str);

      // Gesamte Standardmessunsicherheit
      snprintf (str, 40, "%.2f Nm", abs (total_uncertainty () * nominal_value));
      // snprintf (str, 40, "%.2f %%", total_uncertainty () * 100);
      cairo_centered_text (cr, c1 + 3.5 * col_width, y - col_height / 2, str);

      // Bewertung generieren
      double accuracy = to.accuracy;
      if (accuracy == 0)
        accuracy = to.get_accuracy_from_DIN ();
      bool okay = measurement_items[k]->is_in (accuracy, nominal_value);

      if (! okay)
        {
          cairo_set_source_rgb (cr, 1, 0, 0);
          values_below_max_deviation = 0;
        }

      // rise_time überprüfen
      double rise_time = measurement_items[k]->rise_time;
      double min_rise_time;
      double max_rise_time;
      to.get_timing_from_DIN (measurement_items[k]->nominal_value, min_rise_time, max_rise_time);
      bool rise_time_okay = (rise_time >= min_rise_time && (rise_time<= max_rise_time || max_rise_time == 0));
      if (! rise_time_okay)
        timing_violation = 1;

      //cout << "min_rise_time=" << min_rise_time << " max_rise_time=" << max_rise_time << " rise_time=" << rise_time << endl;

      // eigentliche Messwerte einfüllen
      snprintf (str, 40, "%.2f Nm%s", measurement_items[k]->indicated_value, (rise_time_okay) ? "": "*");
      cairo_centered_text (cr, c1 + col_width * 1.5, y - col_height / 2, str);

      //  Abweichung
      snprintf (str, 40, "%.2f %%", measurement_items[k]->rel_deviation (nominal_value) * 100);
      cairo_centered_text (cr, c1 + col_width * 2.5, y - col_height / 2, str);

      // Bewertung
      strncpy (str, (okay)? "pass" : "fail", 40);
      cairo_centered_text (cr, c1 + col_width * 4.5, y - col_height / 2, str);
      cairo_set_source_rgb (cr, 0, 0, 0);
    }

  // senkrechte Linien zeichnen
  for (int k=0; k < (columns + 1); ++k)
    {
      cairo_move_to (cr, c1 + k * col_width, top);
      cairo_line_to (cr, c1 + k * col_width, top + header_height + row * col_height);
      cairo_stroke (cr);
    }

  if (to.has_no_scale () && ! to.has_fixed_trigger ())
    cairo_centered_text (cr, c1 + 0.5 * col_width, top + header_height/2, "arith. Mittel (Xa)");
  else
    cairo_centered_text (cr, c1 + 0.5 * col_width, top + header_height/2, "Nennwert (Xa)");

  cairo_centered_text (cr, c1 + 1.5 * col_width, top + header_height/2, "Bezugswert (Xr)");
  cairo_centered_text (cr, c1 + 2.5 * col_width, top + header_height/2, "Abweichung");
  cairo_centered_text (cr, c1 + 3.5 * col_width, top + header_height/2, "u");
  cairo_centered_text (cr, c1 + 4.5 * col_width, top + header_height/2, "Bewertung");

  top += header_height + (row + 1) * col_height;
  return top;
}


double measurement::cairo_print_header (cairo_t *cr, double c1, double c2, double top)
{
  //source, font size, face and so on has to be initialized
  top = cairo_print_two_columns (cr, c1, c2, top, "Start der Kalibrierung", "start of calibration", start_time);
  top = cairo_print_two_columns (cr, c1, c2, top, "Ende der Kalibrierung", "stop of calibration", end_time);

  ostringstream os_temp;
  os_temp << temperature << " °C";
  top = cairo_print_two_columns (cr, c1, c2, top, "Temperatur", "temperature", os_temp.str ());

  ostringstream os_humidity;
  os_humidity << humidity << " %rH";
  top = cairo_print_two_columns (cr, c1, c2, top, "Feuchtigkeit", "humidity", os_humidity.str ());

  // Legende
  top = cairo_print_two_columns (cr, c1, c2, top + 0.2, "Verwendete Formelzeichen", "Used formular symbols", "");
  top = cairo_print_legend (cr, c1, c2/2, top, "u", "Gesamte Standardmessunsicherheit", "Total uncertainty of measurement");
  top = cairo_print_legend (cr, c1, c2/2, top, "w", "Gesamte relative Standardmessunsicherheit", "Total relative uncertainty of measurement");
  top = cairo_print_legend (cr, c1, c2/2, top, "Xr", "Mit Hilfe des Messgeräts bestimmter Bezugswert", "FIXME: english translation");
  top = cairo_print_legend (cr, c1, c2/2, top, "Xa", "Anzeige-, Einstell-, Nenn- oder Rechenwert", "FIXME: english translation");
  top = cairo_print_legend (cr, c1, c2/2, top, "as", "Relative Abweichung", "Relative deviation");
  return top;
}

double measurement::cairo_print (cairo_t *cr, double c1, double c2, double top, bool &values_below_max_deviation, bool &timing_violation)
{
  // don't need c2 here
  (void) c2;
  // Messwerte
  // search a sign change in nominal_value
  bool last_positive;
  bool first_run = 1;
  int start = 0;
  for (unsigned int k=0; k < measurement_items.size (); ++k)
    {
      bool pos = measurement_items[k]->nominal_value > 0;

      if (!first_run && pos != last_positive)
        {
          if (to.has_no_scale ()) //10 Messwerte
            top = cairo_print_1_meas_table (cr,  c1, top, start, k - 1, values_below_max_deviation, timing_violation);
          else
            top = cairo_print_5_meas_table (cr,  c1, top, start, k - 1, values_below_max_deviation, timing_violation);
          start = k;
        }
      if ((k+1) == measurement_items.size ())
        {
          if (to.has_no_scale ())
            top = cairo_print_1_meas_table (cr,  c1, top, start, measurement_items.size () - 1, values_below_max_deviation, timing_violation);
          else
            top = cairo_print_5_meas_table (cr,  c1, top, start, measurement_items.size () - 1, values_below_max_deviation, timing_violation);
        }
      first_run = 0;
      last_positive = pos;
    }

  if (timing_violation)
    {
      cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      top = cairo_print_text (cr, c1, top, "*) Die Mindestzeit um das Drehoment von 80% des Zielwerts bis auf den Zielwert zu\n"
                              "erhöhen, konnte nicht eingehalten werden. Siehe DIN EN ISO 6789-1:2015 Kapitel 6.2.4");

      cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
      top = cairo_print_text (cr, c1, top + 0.2, "FIXME: Satz oben in englisch...\n"
                              "FIXME: Satz oben in englisch...");
    }

  {
    cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    ostringstream uncertainty_str;
    uncertainty_str.precision (3);
    uncertainty_str << "Das Intervall der maximalen relativen erweiterten Messunsicherheit aus Messgerät\n"
                    "und Anwender von " << 100 * total_uncertainty () << "% ist kleiner als ein Viertel der höchstzulässigen Abweichung des\n"
                    "Drehmoment-Schraubwerkzeugs.";
    top = cairo_print_text (cr, c1, top + 0.5, uncertainty_str.str ());
  }

  {
    cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
    ostringstream uncertainty_str;
    uncertainty_str.precision (3);
    uncertainty_str << "FIXME: in english...\n"
                    "und Anwender von " << 100 * total_uncertainty () << "% ist kleiner als ein Viertel der höchstzulässigen Abweichung des\n"
                    "Drehmoment-Schraubwerkzeugs.";
    top = cairo_print_text (cr, c1, top + 0.2, uncertainty_str.str ());
  }
  return top;
}

double measurement::cairo_print_conformity (cairo_t *cr, double c1, double top, bool &values_below_max_deviation)
{
  top = cairo_print_two_columns (cr, c1, 0, top, "Konformitätsaussage", "Conformity", "");
  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  if (values_below_max_deviation)
    {
      cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      top = cairo_print_text (cr, c1, top, "Messwerte liegen innerhalb der zulässigen Abweichungen nach ISO 6789-1:2015.\n"
                              "Lorem ipsum sum\n"
                              "Lorem ipsum sum");
      cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
      top = cairo_print_text (cr, c1, top + 0.5, "FIXME: auf englisch Messwerte liegen innerhalb ....\n"
                              "Lorem ipsum sum\n"
                              "Lorem ipsum sum");
    }
  else
    {
      cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      top = cairo_print_text (cr, c1, top, "Messwerte liegen außerhalb der zulässigen Abweichung nach ISO 6789-1:2015.\n"
                              "Lorem ipsum sum\n"
                              "Lorem ipsum sum");
      cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
      top = cairo_print_text (cr, c1, top + 0.2, "FIXME: auf englisch Messwerte liegen außerhalb...\n"
                              "Lorem ipsum sum\n"
                              "Lorem ipsum sum");
    }

  return top;
}

ostream& operator<< (ostream& os, const test_person &tp)
{
  os << "test_person:" << endl;
  os << "  id          = " << tp.id << endl;
  os << "  name        = " << tp.name << endl;
  os << "  supervisor  = " << tp.supervisor << endl;
  os << "  uncertainty = " << tp.uncertainty << endl;
  return os;
}

ostream& operator<< (ostream& os, const test_object &to)
{
  os << "test_object:" << endl;
  os << "  id                = " << to.id << endl;
  os << "  active            = " << to.active << endl;
  os << "  test_equipment_nr = " << to.equipment_number << endl;
  os << "  serial_number     = " << to.serial_number << endl;
  os << "  manufacturer      = " << to.manufacturer << endl;
  os << "  model             = " << to.model << endl;
  os << "  DIN_type          = " << to.DIN_type << endl;
  os << "  DIN_class         = " << to.DIN_class << endl;
  os << "  dir_of_rotation   = " << to.dir_of_rotation << endl;
  os << "  lever_length      = " << to.lever_length << endl;
  os << "  min_torque        = " << to.min_torque << endl;
  os << "  max_torque        = " << to.max_torque << endl;
  os << "  resolution        = " << to.resolution << endl;
  os << "  attachments       = " << to.attachments << endl;
  os << "  accuracy          = " << to.accuracy << endl;
  return os;
}

bool test_object::equal (const test_object &to)
{
  return     id == to.id
             && active == to.active
             &&  ! equipment_number.compare (to.equipment_number)
             &&  ! serial_number.compare (to.serial_number)
             &&  ! manufacturer.compare (to.manufacturer)
             &&  ! model.compare (to.model)
             &&  ! DIN_type.compare (to.DIN_type)
             &&  ! DIN_class.compare (to.DIN_class)
             &&  dir_of_rotation == to.dir_of_rotation
             &&  min_torque == to.min_torque
             &&  max_torque == to.max_torque
             &&  resolution == to.resolution
             &&  accuracy == to.accuracy;
}

ostream& operator<< (ostream& os, const torque_tester &tt)
{
  os << "torque_tester:" << endl;

  os << "  id                         = " << tt.id << endl;
  os << "  serial_number              = " << tt.serial_number << endl;
  os << "  manufacturer               = " << tt.manufacturer << endl;
  os << "  model                      = " << tt.model << endl;
  os << "  next_calibration_date      = " << tt.next_calibration_date << endl;
  os << "  calibration_date           = " << tt.calibration_date << endl;
  os << "  calibration_number         = " << tt.calibration_number << endl;
  os << "  max_torque                 = " << tt.max_torque << endl;
  os << "  resolution                 = " << tt.resolution << endl;
  os << "  uncertainty_of_measurement = " << tt.uncertainty_of_measurement << endl;
  return os;
}

ostream& operator<< (ostream& os, const measurement &mm)
{
  os << "measurement:" << endl;
  os << "  id               = " << mm.id << endl;
  os << "  test_person.id   = " << mm.tp.id << endl;
  os << "  test_object.id   = " << mm.to.id << endl;
  os << "  torque_tester.id = " << mm.tt.id << endl;
  os << "  start_time       = " << mm.start_time << endl;
  os << "  end_time         = " << mm.end_time << endl;
  os << "  temperature      = " << mm.temperature << endl;
  os << "  humidity         = " << mm.humidity << endl;

  // output all measurement_items
  for (unsigned int k=0; k<mm.measurement_items.size (); ++k)
    os << *mm.measurement_items[k];
  return os;
}

ostream& operator<< (ostream& os, const measurement_item &mi)
{
  os << "  measurement_item:" << endl;
  os << "    ts              = " << mi.ts << endl;
  os << "    nominal_value   = " << mi.nominal_value << endl;
  os << "    indicated_value = " << mi.indicated_value << endl;
  os << "    rise_time       = " << mi.rise_time << endl;
  return os;
}

// find test_objects with LIKE using serial
void search_test_objects (sqlite3 *db, enum test_object_search_field field, string search_str, vector<test_object> &vto)
{
  sqlite3_stmt *pStmt;
  int rc = -1;

  if (field == SERIAL)
    rc = sqlite3_prepare_v2 (db, "SELECT ID FROM test_object WHERE serial_number LIKE ?1 AND active == 1", -1, &pStmt, NULL);
  else if (field == EQUIPMENTNR)
    rc = sqlite3_prepare_v2 (db, "SELECT ID FROM test_object WHERE equipment_number LIKE ?1 AND active == 1", -1, &pStmt, NULL);
  else if (field == MANUFACTURER)
    rc = sqlite3_prepare_v2 (db, "SELECT ID FROM test_object WHERE manufacturer LIKE ?1 AND active == 1", -1, &pStmt, NULL);
  else if (field == MODEL)
    rc = sqlite3_prepare_v2 (db, "SELECT ID FROM test_object WHERE MODEL LIKE ?1 AND active == 1", -1, &pStmt, NULL);

  if (rc == SQLITE_OK)
    {
      rc = sqlite3_bind_text (pStmt, 1, search_str.c_str (), -1, SQLITE_STATIC);
      if (rc == SQLITE_OK)
        {
          do
            {
              rc = sqlite3_step (pStmt);
              if (rc == SQLITE_ROW)
                {
                  int id = sqlite3_column_int (pStmt, 0);
                  //cout << "search_test_object_serial id=" << id << endl;
                  test_object tmp;
                  tmp.load_with_id (db, id);
                  vto.push_back (tmp);
                }
            }
          while (rc != SQLITE_DONE);
          sqlite3_finalize(pStmt);
        }
      else
        fprintf(stderr, "SQL error from sqlite3_bind_text: %i = %s\n", rc, sqlite3_errmsg(db));
    }
  else
    fprintf(stderr, "SQL error from sqlite3_prepare_v2: %i = %s\n", rc, sqlite3_errmsg(db));

}

void set_test_object_active (sqlite3 *db, int id, bool active)
{
  cout << "set_test_object_active id=" << id << " active=" << active << endl;

  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2 (db, "UPDATE test_object SET active = ?1 WHERE ID=?2;", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      sqlite3_bind_int (pStmt, 1, active);
      sqlite3_bind_int (pStmt, 2, id);
      rc = sqlite3_step (pStmt);
      sqlite3_finalize(pStmt);
      if (rc != SQLITE_DONE)
        {
          fprintf(stderr, "set_test_object_active sqlite3_step failed %i = %s\n", rc, sqlite3_errmsg(db));
          throw runtime_error ("set_test_object_active sqlite3_step failed");
        }
    }
  else
    {
      fprintf(stderr, "set_test_object_active sqlite3_prepare_v2 failed %i = %s\n", rc, sqlite3_errmsg(db));
      throw runtime_error ("set_test_object_active sqlite3_prepare_v2 failed");
    }
}

void set_test_object_equipment_number (sqlite3 *db, int id, string equipment_number)
{
  cout << "set_test_object_equipment_number id=" << id << " equipment_number=" << equipment_number << endl;

  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2 (db, "UPDATE test_object SET equipment_number = ?1 WHERE ID=?2;", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      sqlite3_bind_text (pStmt, 1, equipment_number.c_str (), -1, SQLITE_STATIC);
      sqlite3_bind_int (pStmt, 2, id);
      rc = sqlite3_step (pStmt);
      sqlite3_finalize(pStmt);
      if (rc != SQLITE_DONE)
        {
          fprintf(stderr, "set_test_object_equipment_number sqlite3_step failed %i = %s\n", rc, sqlite3_errmsg(db));
          throw runtime_error ("set_test_object_equipment_number sqlite3_step failed");
        }
    }
  else
    {
      fprintf(stderr, "set_test_object_equipment_number sqlite3_prepare_v2 failed %i = %s\n", rc, sqlite3_errmsg(db));
      throw runtime_error ("set_test_object_equipment_number sqlite3_prepare_v2 failed");
    }
}

bool get_test_object_active (sqlite3 *db, int id)
{
  cout << "get_test_object_active id=" << id << endl;
  bool ret;
  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2 (db, "SELECT active from test_object WHERE ID=?1;", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      sqlite3_bind_int (pStmt, 1, id);
      rc = sqlite3_step (pStmt);
      if (rc == SQLITE_ROW)
        ret = sqlite3_column_int (pStmt, 0);
      sqlite3_finalize(pStmt);
      if (rc == SQLITE_DONE)
        throw runtime_error ("No test_object with given id");
    }
  else
    {
      fprintf(stderr, "get_test_object_active sqlite3_prepare_v2 failed %i = %s\n", rc, sqlite3_errmsg(db));
      throw runtime_error ("get_test_object_active sqlite3_prepare_v2 failed");
    }
  cout << "get_test_object_active returns " << ret << endl;
  return ret;
}

// return 0 if none found
int search_active_adjacent_test_object (sqlite3 *db, int id)
{
  vector<test_object> vto;
  search_test_objects (db, SERIAL, "%", vto);
  cout << "search_active_adjacent_test_object vto.size()=" << vto.size () << endl;

  int min_dist = INT_MAX;
  int adj_id = 0;
  for (unsigned int k=0; k < vto.size (); ++k)
    {
      int dist = abs (id - vto[k].id);
      cout << "id=" << vto[k].id << " active=" << vto[k].active << " dist=" << dist << endl;
      if (dist < min_dist)
        {
          min_dist = dist;
          adj_id = vto[k].id;
        }
    }
  cout << "search_active_adjacent_test_object returns " << adj_id << endl;;
  return adj_id;
}

// check if there are measurements which references this test_object
bool test_object_has_measurement (sqlite3 *db, int id)
{
  cout << "test_object_has_measurement id=" << id << endl;
  bool ret=false;
  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2 (db, "SELECT id from measurement where test_object_id == ?1;", -1, &pStmt, NULL);
  if (rc == SQLITE_OK)
    {
      sqlite3_bind_int (pStmt, 1, id);
      rc = sqlite3_step (pStmt);
      if (rc == SQLITE_ROW)
        ret = true;
      sqlite3_finalize(pStmt);
    }
  else
    {
      fprintf(stderr, "test_object_has_measurement sqlite3_prepare_v2 failed %i = %s\n", rc, sqlite3_errmsg(db));
      throw runtime_error ("test_object_has_measurement sqlite3_prepare_v2 failed");
    }
  cout << "test_object_has_measurement returns " << ret << endl;
  return ret;
}

void search_test_persons (sqlite3 *db, enum test_person_search_field field, string s, vector<test_person> &vtp)
{
  cout << "search_test_persons field=" << field << " search_string=" << s << endl;
  sqlite3_stmt *pStmt;
  int rc = -1;

  if (field == NAME)
    rc = sqlite3_prepare_v2 (db, "SELECT id FROM test_person WHERE name LIKE ?1", -1, &pStmt, NULL);
  else if (field == SUPERVISOR)
    rc = sqlite3_prepare_v2 (db, "SELECT id FROM test_person WHERE supervisor LIKE ?1", -1, &pStmt, NULL);

  if (rc == SQLITE_OK)
    {
      rc = sqlite3_bind_text (pStmt, 1, s.c_str (), -1, SQLITE_STATIC);
      if (rc == SQLITE_OK)
        {
          do
            {
              rc = sqlite3_step (pStmt);
              if (rc == SQLITE_ROW)
                {
                  int id = sqlite3_column_int (pStmt, 0);
                  test_person tmp;
                  tmp.load_with_id (db, id);
                  vtp.push_back (tmp);
                }
            }
          while (rc != SQLITE_DONE);
          sqlite3_finalize(pStmt);
        }
      else
        fprintf(stderr, "SQL error from sqlite3_bind_text: %i = %s\n", rc, sqlite3_errmsg(db));
    }
  else
    fprintf(stderr, "SQL error from sqlite3_prepare_v2: %i = %s\n", rc, sqlite3_errmsg(db));

  cout << "search_test_persons returns " << vtp.size () << " records" << endl;
}
