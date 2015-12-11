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

#include "ttt.h"

//C'Tor
ttt::ttt (cb_display_double *cb_ind, cb_display_double *cb_nom, cb_display_double *cb_pt, cb_display_string *cb_i, cb_display_string_double *cb_s, cb_display_string *cb_r, string database_fn)
  :pttt(0),
   db(0),
   cb_indicated_torque(cb_ind),
   cb_nominal_torque(cb_nom),
   cb_peak_torque(cb_pt),
   cb_instruction(cb_i),
   cb_step(cb_s),
   cb_result(cb_r),
   confirmation(0),
   current_step(-1),
   sequencer_is_running(0),
   measurement_method(-1)
{
  print_indicated_torque(0.0);
  print_nominal_torque(0.0);
  print_peak_torque(0.0);
  print_instruction("");
  print_step ("idle", 0);
  print_result ("");

  int rc = sqlite3_open(database_fn.c_str (), &db);
  if( rc )
    {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      throw runtime_error ("Can't open sqlite database");
    }
  sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
}

ttt::~ttt ()
{
  cout << "ttt d'Tor" << endl;

  if (sequencer_is_running)
    stop_sequencer ();

  // delete all steps
  for (vector<step*>::iterator it = steps.begin() ; it != steps.end(); ++it)
    delete (*it);

  disconnect_measurement_input ();
  disconnect_TTT ();
  sqlite3_close (db);
}

ostream& operator<< (ostream& os, const ttt& d)
{
  os << "# TTT serial           = " << d.pttt->get_serial () << endl;
  os << "# TTT cal_date         = " << d.pttt->get_cal_date () << endl;
  return os;
}

void ttt::connect_TTT ()
{
  if (! pttt)
    pttt = new ttt_device ();
  else
    throw runtime_error ("TTT already connected");
  load_torque_tester ();
}

void ttt::connect_TTT (string serial)
{
  if (! pttt)
    pttt = new ttt_device (serial);
  else
    throw runtime_error ("TTT already connected");
}

void ttt::disconnect_TTT ()
{
  if (pttt)
    delete pttt;
}

void ttt::connect_measurement_input (string fn)
{
  if (measurement_input.is_open ())
    cerr << "ERROR: measurement_input already connected" << endl;
  else
    {
      measurement_input.open (fn.c_str ());
    }
  load_torque_tester ();
}

void ttt::disconnect_measurement_input ()
{
  if (measurement_input.is_open ())
    measurement_input.close ();
}

string ttt::get_time_for_filename ()
{
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  // FIXME: check if : is allowed in filenames on OSX and windoze
  // http://stackoverflow.com/questions/3038351/check-whether-a-string-is-a-valid-filename-with-qt
  // http://www.boost.org/doc/libs/1_43_0/libs/filesystem/doc/portability_guide.htm
  strftime (buffer, 80, "%Y-%m-%d_%H_%M_%S", timeinfo);
  return buffer;
}

/*!
 * run has to be called periodically.
 * This reads torque data from a connected TTT device or measurement_input
 * and writes to measurement_output and switches the steps.
 */
bool ttt::run ()
{
  // should be called faster than 21ms (900Hz Mode, packet_len = 19)
  // if called slower, samples may be missing

  if (current_step >= steps.size () && sequencer_is_running)
    {
      // we have reached the end
      // stop_sequencer
      stop_sequencer ();

      // save measurement
      meas.end_time = get_localtime();
      meas.save (db);

      // create report for DIN 6789 TypI and Typ II
      if (measurement_method == 1)
        {
          ostringstream os;

          // FIXME: model kann z.B. 730N/2 sein
          //
          // http://stackoverflow.com/questions/3038351/check-whether-a-string-is-a-valid-filename-with-qt
          // http://www.boost.org/doc/libs/1_43_0/libs/filesystem/doc/portability_guide.htm

          os << get_time_for_filename ();
          //os << "_" << meas.to.serial_number;
          //os << "_" << meas.to.manufacturer;
          //os << "_" << meas.to.model;
          os << "_DIN6789.pdf";

          report_filename = os.str ();
          report_result res = DIN6789_report (report_filename, meas.id);

          if (res.values_below_max_deviation && !res.timing_violation)
            print_result (gettext ("Kalibrierung innerhalb Toleranz"));
          else if (res.values_below_max_deviation && res.timing_violation)
            print_result (gettext ("#Kalibrierung innerhalb Toleranz jedoch Mindestzeit nicht eingehalten"));
          else
            print_result (gettext ("*Kalibrierung außerhalb Toleranz"));

          print_step ( string (gettext ("Kalibrierschein:")) + " " + report_filename, 1);
        }
      else if (measurement_method == 0)
        {
          report_filename = get_time_for_filename () + "_quick_check.pdf";
          bool res = create_quick_test_report (db, meas.id, report_filename.c_str ());

          if (res)
            print_result (gettext ("Schnellkalibrierung innerhalb Toleranz"));
          else
            print_result (gettext ("*Schnellkalibrierung außerhalb Toleranz"));
          print_step ( string (gettext ("Kalibrierschein:")) + " " + report_filename, 1);
        }
      else
        print_step ("finished", 1);
    }

  if (sequencer_is_running)
    {
      enum out_cmd cmd = NO_CMD;
      // read torque measurements
      if (pttt)
        {
          //read from hardware
          vector<double> tmp = pttt->poll_measurement ();
          unsigned int len = tmp.size ();
          if (len > 0)
            {
              // append to torque measurements
              for (unsigned int k=0; k < len; ++k)
                {
                  measurement_output << tmp[k]  << "\t" << confirmation << endl;
                  cmd = sequencer_inout (tmp[k], confirmation);
                }
              print_indicated_torque(tmp[len - 1]);
            }
        }
      else if (measurement_input.is_open ())
        {
          // read from measurement_input
          // simulate a connected hardware with TTT_SPS sampling rate
          static bool first_run = true;
          static struct timeval t1;

          if (first_run)
            {
              first_run = false;
              gettimeofday (&t1, NULL);
            }
          else
            {
              struct timeval t2;
              gettimeofday (&t2, NULL);
              double diff = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1.0e6;
              t1 = t2;

              int num_samples = diff * TTT_SPS;
              //cout << "diff = " << diff << " num_samples = " << num_samples << endl;

              double torque;
              bool tmp_confirmation;
              for (int k=0; k<num_samples; ++k)
                {
                  measurement_input >> torque;
                  measurement_input >> tmp_confirmation;

                  if (! measurement_input.eof ())
                    {
                      //cout << "torque = " << torque << " conf = " << tmp_confirmation << endl;
                      cmd = sequencer_inout (torque, tmp_confirmation);
                      measurement_output << torque << "\t" << tmp_confirmation << endl;
                    }
                  else
                    {
                      cerr << "ERROR: EOF measurement_input" << endl;
                      stop_sequencer ();
                      break;
                    }
                }
              print_indicated_torque(torque);
            }
        }
      else
        cerr << "ERROR: No source for torque measurements specified" << endl;

      step* pstep = steps[current_step];

      if (pttt && (cmd == CMD_TARA ))
        {
          //cout << "**************** TARA **************************" << endl;
          pttt->tare ();
          //cout << "**************** FINISHED TARA **************************" << endl;
          cmd = pstep->inout (0, 1);
        }

      meas_step *pmeas = dynamic_cast<meas_step*>(pstep);
      if (pmeas)
        {
          print_nominal_torque (pmeas->get_nominal_value ());
          print_peak_torque (pmeas->get_peak_torque());
          if (pmeas->is_finished ())
            {
              //cout << "max_torque = " << pmeas->peak_torque() << endl;
              double rise_time = -1;
              // is it a peak_click_step?
              peak_click_step *pclick = dynamic_cast<peak_click_step*>(pmeas);
              if (pclick)
                rise_time = pclick->get_rise_time ();

              // add result to database
              meas.add_measurement_item (get_localtime (), pmeas->get_nominal_value (), pmeas->get_peak_torque (), rise_time);
            }
        }

      preload_step *ppreload = dynamic_cast<preload_step*>(pstep);
      if (ppreload)
        {
          print_nominal_torque (ppreload->get_nominal_value ());
        }

      // refresh instruction and step description
      print_instruction (pstep->instruction ());

      // show active step and progress
      print_step (steps[current_step]->description (), (1.0 * current_step)/steps.size ());

      if (pstep->is_finished ())
        current_step++;
    }

  return sequencer_is_running;
}

void ttt::set_confirmation ()
{
  confirmation = true;
}

void ttt::add_step (step *s)
{
  //cout << "ttt::add_step" << endl;
  steps.push_back (s);
}

void ttt::clear_steps()
{
  cout << "ttt::clear_steps" << endl;
  steps.clear ();
  current_step=0;
}

// add complete DIN ISO 6789 sequences
void ttt::add_DIN6789_steps (bool repeat_on_timing_violation)
{
  int runs;
  int sign;

  // 0 = beide, 1 = nur rechtsdrehend, 2 = nur linksdrehend
  switch (meas.to.dir_of_rotation)
    {
    case 0: // both directions
      runs = 2; // 2 Durchläufe
      sign = 1; // zuerst rechtsdrehend
      break;

    case 1: // nur rechtsdrehend
      runs = 1; // nur ein Durchlauf
      sign = 1; // rechtsdrehend
      break;

    case 2: // nur linksdrehend
      runs = 1; // nur ein Durchlauf
      sign = -1; // linksdrehend
      break;
    default:
      throw runtime_error ("Unknown dir_of_rotation");
    }

  cout << "ttt::add_DIN6789_steps " << meas.to.get_type_class () << " runs=" << runs << " sign=" << sign << endl;

  for (int k=0; k<runs; ++k)
    {
      double min_torque = sign * meas.to.min_torque;
      double max_torque = sign * meas.to.max_torque;

      // check for same sign
      if ((min_torque * max_torque) <= 0)
        throw out_of_range("ttt::add_DIN6789_steps: min_torque and max_torque must have the same sign");

      // three torque tester preload cycles
      for (int k = 0; k < 3; ++k)
        add_step (new preload_torque_tester_step(max_torque));

      add_step (new tare_torque_tester_step());

      vector<double> torque_list;

      if (    meas.to.is_type (1)
              || (meas.to.is_type (2) && ! meas.to.has_no_scale()))
        {
          // min, 60% * max, 100% * max
          torque_list.push_back (min_torque);
          torque_list.push_back (0.6 * max_torque);
          torque_list.push_back (max_torque);
        }
      else // has_no_scale ()
        {
          if (meas.to.has_fixed_trigger ())
            torque_list.push_back (max_torque);
          else
            torque_list.push_back (min_torque);
        }

      for (unsigned int i=0; i < torque_list.size (); ++i)
        {
          //cout << "torque_list["<<i<<"]=" << torque_list[i] << endl;

          for (int k = 0; k < 3; ++k)
            {
              // 80% muss überschritten werden, damit der Schritt weitergeschaltet wird
              if (meas.to.has_no_scale ())
                add_step (new preload_test_object_step(max_torque));
              else
                add_step (new preload_test_object_step(torque_list[i]));
            }

          if (test_object_is_type (2))
            {
              double min_t, max_t;
              test_object::get_timing_from_DIN (meas.to.get_type_class (), torque_list[i], min_t, max_t);

              // Set max_t to 10s if not further specified
              if (max_t == 0)
                max_t = 10;

              // See DIN EN ISO 6789-1:2015 6.6.1 table 7
              int count_click_measurements = 5;
              if (meas.to.has_no_scale ())
                count_click_measurements = 10;

              double peak_level = 0.5;
              if (pttt)
                peak_level = pttt->get_peak_level ();

              for (int k = 0; k < count_click_measurements; ++k)
                add_step (new peak_click_step(torque_list[i], min_t, max_t, repeat_on_timing_violation, 0.6, peak_level));
            }
          else // typ 1
            {
              // Tara Prüfling
              add_step (new tare_test_object_step());

              for (int k = 0; k < 5; ++k)
                add_step (new peak_meas_step(torque_list[i]));
            }
        }
      sign = -sign;
    }
}

void ttt::start_sequencer (double temperature, double humidity)
{
  if (sequencer_is_running)
    throw runtime_error ("Sequencer is already running. Please stop it first");

  if (! measurement_output.is_open ())
    {
      ostringstream os;
      os << "./logfiles/";
      os << get_time_for_filename ();
      os << "_" << meas.to.serial_number;
      os << "_" << meas.to.manufacturer;
      os << "_" << meas.to.model;

      // are we connected to a real TTT?
      if (pttt)
        os << "_" << pttt->get_serial ();

      os << ".log";
      meas.raw_data_filename = os.str ();

      // FIXME: check if ./logfiles exists, create it if not?
      // open and write header
      measurement_output.open (meas.raw_data_filename.c_str ());
      if (pttt)
        {
          measurement_output << "# TTT serial           = " << pttt->get_serial () << endl;
          measurement_output << "# TTT cal_date         = " << pttt->get_cal_date () << endl;
        }
    }

  // init measurement
  meas.clear_measurement_item ();
  meas.temperature = temperature;
  meas.humidity = humidity;
  meas.start_time = get_localtime();

  if (pttt)
    pttt->start ();

  current_step = 0;
  print_peak_torque (0.0);
  print_indicated_torque (0.0);
  print_nominal_torque (0.0);
  sequencer_is_running = true;
}

void ttt::start_sequencer_single_peak (double temperature, double humidity, double nominal_value)
{
  if (sequencer_is_running)
    throw runtime_error ("Sequencer is already running. Please stop it first");

  clear_steps ();
  // add_step (new tare_step());
  add_step (new peak_meas_step(nominal_value));
  measurement_method = 0;
  start_sequencer (temperature, humidity);
}

void ttt::start_sequencer_DIN6789 (double temperature, double humidity, bool repeat_on_timing_violation)
{
  if (sequencer_is_running)
    throw runtime_error ("Sequencer is already running. Please stop it first");

  clear_steps ();
  // min_torque and max_torque_resolution from database
  // resolution from DIN678

  add_DIN6789_steps (repeat_on_timing_violation);

  measurement_method = 1;
  start_sequencer (temperature, humidity);
}

void ttt::start_sequencer_ASME (double temperature, double humidity)
{
  if (sequencer_is_running)
    throw runtime_error ("Sequencer is already running. Please stop it first");

  clear_steps ();
  measurement_method = 2;

  cout << "ttt::start_sequencer_ASME" << " temperature=" << temperature << " humidity=" << humidity << endl;
  throw runtime_error ("FIXME: ASME not yet implemented");
}

void ttt::stop_sequencer ()
{
  if (measurement_output.is_open ())
    measurement_output.close ();

  sequencer_is_running = false;

  // stop measuring
  if (pttt)
    pttt->stop ();
}

out_cmd ttt::sequencer_inout (double torque, bool confirmation)
{
  step *pstep = steps[current_step];
  return pstep->inout (torque, confirmation);
}

//! Loop over steps and list detected peaks
// only for debugging
void ttt::print_result ()
{
  int len = steps.size ();
  for (int k=0; k<len; ++k)
    {
      cout << "step " << k+1 << "/" << len << " "
           << steps[k]->description ();

      meas_step *pmeas = dynamic_cast<meas_step*>(steps[k]);
      if (pmeas)
        {
          cout << " nominal=" << pmeas->get_nominal_value ();
          cout << " peak=" << pmeas->get_peak_torque ();

          // is it a peak_click_step?
          peak_click_step *pclick = dynamic_cast<peak_click_step*>(pmeas);
          if (pclick)
            cout << " rise_time=" << pclick->get_rise_time ();
        }
      cout << endl;
    }
}

report_result ttt::DIN6789_report (string fn, int id)
{
  return create_DIN6789_report (db, id, fn.c_str ());
}

void ttt::load_torque_tester ()
{
  if (pttt)
    {
      // check if this is a new TTT or already in database
      int id = meas.tt.search_serial_and_next_cal_date (db, pttt->get_serial (), pttt->get_next_cal_date ());
      if (id > 0)
        {
          cout << "ttt::load_torque_tester found torque_tester with id = " << id << endl;
          meas.tt.load_with_id (db, id);
        }
      else
        {
          // create new entry
          meas.tt.serial_number         = pttt->get_serial ();
          meas.tt.manufacturer          = "Alluris GmbH & Co. KG";
          meas.tt.model                 = "TTT";
          meas.tt.next_calibration_date = pttt->get_next_cal_date ();
          meas.tt.calibration_date      = pttt->get_cal_date ();
          meas.tt.calibration_number    = pttt->get_cal_number ();
          meas.tt.max_torque            = pttt->get_max_torque ();
          meas.tt.uncertainty_of_measurement = pttt->get_uncertainty ();
          meas.tt.resolution            = pttt->get_resolution ();
          meas.tt.save (db);
          cout << "ttt::load_torque_tester created new torque_tester with id = " << meas.tt.id << endl;
        }
    }
  else
    {
      // load torque_tester with id 1 in simulation mode
      meas.tt.load_with_id (db, 1);
    }
}
