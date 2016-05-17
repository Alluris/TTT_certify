/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

TTT (tool torque tester) interface class

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

#ifndef TTT_H
#define TTT_H

#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <libintl.h>
#include "ttt_device.h"
#include "step.h"
#include "sqlite_interface.h"
#include "cairo_drawing_functions.h"
#include "measurement_table.h"

using namespace std;

typedef void(cb_display_double)(double value);
typedef void(cb_display_string)(string s);
typedef void(cb_display_string_double)(string s, double value);

#define TTT_SPS 900

class ttt
{

private:

  ttt_device *pttt;
  sqlite3 *db;

  ifstream measurement_input;
  ofstream measurement_output;

  double start_peak_torque_factor;
  double stop_peak_torque_factor;

  measurement_table *m_table;

  //! display for measured torque
  cb_display_double *cb_indicated_torque;
  //! display for nominal troque
  cb_display_double *cb_nominal_torque;
  //! display for measured peak torque
  cb_display_double *cb_peak_torque;
  //! display for user instructions
  cb_display_string *cb_instruction;
  //! display for step as string and progress 0..1
  cb_display_string_double *cb_step;
  //! display for result
  cb_display_string *cb_result;

  void print_indicated_torque (double v)
  {
    if (cb_indicated_torque)
      cb_indicated_torque (v);
  }

  void print_nominal_torque (double v)
  {
    if (cb_nominal_torque)
      cb_nominal_torque (v);
  }

  void print_peak_torque (double v)
  {
    if (cb_peak_torque)
      cb_peak_torque (v);
  }

  void print_instruction (string v)
  {
    if (cb_instruction)
      cb_instruction (v);
  }

  void print_step (string s, double v)
  {
    if (cb_step)
      cb_step (s, v);
  }

  void print_result (string s)
  {
    if (cb_result)
      cb_result (s);
  }

  // step sequencer
  vector<step *> steps;
  bool confirmation;

  unsigned int current_step;
  bool sequencer_is_running;

  string report_filename;
  string get_time_for_filename (); //returns localtime for usage in output filename

  measurement meas;

  enum e_report_style
  {
    QUICK_CHECK_REPORT,
    ISO6789_REPORT,
    ISO6789_LIKE_REPORT_WITH_REPEATS
  } report_style;

public:


  ttt (cb_display_double *cb_ind,
       cb_display_double *cb_nom,
       cb_display_double *cb_pt,
       cb_display_string *cb_i,
       cb_display_string_double *cb_s,
       cb_display_string *cb_r,
       string database_fn,
       double start_peak_torque_factor,
       double stop_peak_torque_factor,
       measurement_table *mt=0);
  ~ttt ();

  //************* datasource *************
  //! connect with first available TTT
  void connect_TTT ();
  //! connect with TTT with given serial number
  void connect_TTT (string serial);
  void disconnect_TTT ();

  /*!
   * Debugging tool:
   * Read torque measurement and confirmation from file instead of TTT device
   */
  void connect_measurement_input (string fn);
  void disconnect_measurement_input ();

  //************* datasink *************
  /*!
   * opens a file to write torque measurmeent data with 900Hz
   * Lines starting with # are comments/header
  */
  void connect_measurement_output (string fn);
  void disconnect_measurement_output ();

  //******** steps *******************
  void add_step (step *s);

  //! delete all steps
  void clear_steps ();

  // komplette Sequenzen hinzufügen
  void add_ISO6789_steps (bool repeat_on_timing_violation);

  bool run ();

  //! set confirmation/acknowledge for steps which needs user feedback
  void set_confirmation();

  void start_sequencer (double temperature, double humidity);
  void start_sequencer_quick_check (double temperature, double humidity, double nominal_value);
  void start_sequencer_ISO6789 (double temperature, double humidity, bool repeat_on_timing_violation, bool repeat_on_tolerance_violation);

  void stop_sequencer ();
  enum out_cmd sequencer_inout (double torque, bool confirmation);

  void print_result ();
  report_result ISO6789_report (string fn, int id, bool repeat_on_tolerance_violation);

  friend ostream& operator<<(ostream& os, const ttt& d);

  //********* DATABASE INSERT ****************/
  int new_test_person (string name, string supervisor, double uncertainty)
  {
    meas.tp.id = -1;
    meas.tp.name = name;
    meas.tp.supervisor = supervisor;
    meas.tp.uncertainty = uncertainty;
    meas.tp.save (db);
    return meas.tp.id;
  }

  int new_test_object  (string equipment_number,
                        string serial_number,
                        string manufacturer,
                        string model,
                        string type,
                        int dir_of_rotation,
                        double lever_length,
                        double min_torque,
                        double max_torque,
                        double resolution,
                        string attachments,
                        double accuracy)
  {
    meas.to.id = -1;
    meas.to.active = true;
    meas.to.equipment_number = equipment_number;
    meas.to.serial_number    = serial_number;
    meas.to.manufacturer     = manufacturer;
    meas.to.model            = model;

    if (! type.compare (0, 2, "II"))
      {
        meas.to.DIN_type = "II";
        meas.to.DIN_class = type.substr (2, 1);
      }
    else
      {
        meas.to.DIN_type = "I";
        meas.to.DIN_class = type.substr (1, 1);
      }

    meas.to.dir_of_rotation = dir_of_rotation;
    meas.to.lever_length    = lever_length;
    meas.to.min_torque      = min_torque;
    meas.to.max_torque      = max_torque;
    meas.to.resolution      = resolution;
    meas.to.attachments     = attachments;
    meas.to.accuracy        = accuracy;
    meas.to.save (db);
    return meas.to.id;
  }

  /*
    double cairo_print_test_object (cairo_t *cr)
    {
      return meas.tt.cairo_print (cr, 0, 0, 0);
    }
  */
  //********* DATABASE LOOKUPS **************/
  void load_test_person (int id)
  {
    meas.tp.load_with_id (db, id);
  }

  void search_test_person_name (string s, vector<test_person> &vtp)
  {
    search_test_persons (db, NAME, subst_wildcards (s), vtp);
  }

  void search_test_person_supervisor (string s, vector<test_person> &vtp)
  {
    search_test_persons (db, SUPERVISOR, subst_wildcards (s), vtp);
  }

  void load_test_object (int id)
  {
    meas.to.load_with_id (db, id);
  }

  // set id inactive and return next adjacent test_object or 0 if there is none
  int delete_test_object (int id)
  {
    set_test_object_active (db, id, false);
    set_test_object_equipment_number (db, id, meas.to.equipment_number + "(deleted on " + get_localtime () +")");
    int adj_id = search_active_adjacent_test_object (db, id);
    if (! adj_id)
      meas.to.id = 0;
    return adj_id;
  }

  void load_torque_tester ();

  //******* test person **************/

  string get_test_person_name ()
  {
    return meas.tp.name;
  }

  string get_test_person_supervisor ()
  {
    return meas.tp.supervisor;
  }

  double get_test_person_uncertainty ()
  {
    return meas.tp.uncertainty;
  }

  //******* test object **************/
  string get_test_object_equipment_nr ()
  {
    return meas.to.equipment_number;
  }

  string get_test_object_serial ()
  {
    return meas.to.serial_number;
  }

  string get_test_object_manufacturer()
  {
    return meas.to.manufacturer;
  }

  string get_test_object_model()
  {
    return meas.to.model;
  }

  // kombiniert aus typ und Klasse
  string get_test_object_type ()
  {
    return meas.to.get_type_class ();
  }

  bool test_object_is_type (int id)
  {
    return meas.to.is_type (id);
  }

  bool test_object_is_type_class (string tc)
  {
    return meas.to.is_type_class (tc);
  }

  bool test_object_is_editable ()
  {
    return (! test_object_has_measurement (db, meas.to.id));
  }

  bool test_object_is_is_screwdriver ()
  {
    return meas.to.is_screwdriver ();
  }

  int get_test_object_dir_of_rotation()
  {
    return meas.to.dir_of_rotation;
  }

  double get_test_object_min_torque ()
  {
    return meas.to.min_torque;
  }

  double get_test_object_max_torque ()
  {
    return meas.to.max_torque;
  }

  double get_test_object_resolution ()
  {
    return meas.to.resolution;
  }

  double get_test_object_lever_length ()
  {
    return meas.to.lever_length;
  }

  double get_test_object_accuracy ()
  {
    return meas.to.accuracy;
  }

  string get_test_object_attachments ()
  {
    return meas.to.attachments;
  }

  double get_test_object_accuracy_from_DIN ()
  {
    return meas.to.get_accuracy_from_DIN ();
  }

  void print_test_object ()
  {
    cout << meas.to;
  }

  //******* torque_tester **************/
  int get_torque_tester_id ()
  {
    return meas.tt.id;
  }

  string get_torque_tester_serial ()
  {
    return meas.tt.serial_number;
  }

  string get_torque_tester_manufacturer()
  {
    return meas.tt.manufacturer;
  }

  string get_torque_tester_model()
  {
    return meas.tt.model;
  }

  string get_torque_tester_next_cal_date()
  {
    return meas.tt.next_calibration_date;
  }

  string get_torque_tester_cal_date()
  {
    return meas.tt.calibration_date;
  }

  string get_torque_tester_cal_number()
  {
    return meas.tt.calibration_number;
  }

  double get_torque_tester_max_torque ()
  {
    return meas.tt.max_torque;
  }

  double get_torque_tester_resolution ()
  {
    return meas.tt.resolution;
  }

  double get_torque_tester_uncertainty_of_measurement ()
  {
    return meas.tt.uncertainty_of_measurement;
  }

  double get_torque_tester_peak_level ()
  {
    double peak_level = 0.5;
    if (pttt)
      peak_level = pttt->get_peak_level ();
    return peak_level;
  }
};

#endif
