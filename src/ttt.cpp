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

#ifdef _WIN32
#include <Shellapi.h>  //for ShellExecute
#endif

bool isnalnum (char c)
{
  return ! std::isalnum (c);
}

//C'Tor
ttt::ttt (cb_display_double *cb_ind,
          cb_display_double *cb_nom,
          cb_display_double *cb_pt,
          cb_display_string *cb_i,
          cb_display_string_double *cb_s,
          cb_display_string *cb_r,
          string database_fn,
          double start_peak,
          double stop_peak,
          measurement_table *mt)
  :pttt(0),
   db(0),
   start_peak_torque_factor (start_peak),
   stop_peak_torque_factor (stop_peak),
   m_table(mt),
   cb_indicated_torque(cb_ind),
   cb_nominal_torque(cb_nom),
   cb_peak_torque(cb_pt),
   cb_instruction(cb_i),
   cb_step(cb_s),
   cb_result(cb_r),
   confirmation(0),
   current_step(-1),
   sequencer_is_running(0),
   report_style (QUICK_CHECK_REPORT)
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

  clear_steps ();

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

      // create report for DIN EN ISO 6789 TypI and Typ II
      if (report_style == ISO6789_REPORT || report_style == ISO6789_LIKE_REPORT_WITH_REPEATS)
        {
          ostringstream os;

          // FIXME: model kann z.B. 730N/2 sein
          //
          // http://stackoverflow.com/questions/3038351/check-whether-a-string-is-a-valid-filename-with-qt
          // http://www.boost.org/doc/libs/1_43_0/libs/filesystem/doc/portability_guide.htm

          os << get_time_for_filename ();
          os << "_" << meas.to.serial_number;
          os << "_" << meas.to.manufacturer;
          os << "_" << meas.to.model;

          report_filename = os.str ();
          std::replace_if(report_filename.begin(), report_filename.end(), isnalnum, '_');

          if (report_style == ISO6789_REPORT)
            report_filename += "_ISO6789.pdf";
          else
            report_filename += "_like_ISO6789.pdf";

          report_result res = create_ISO6789_report (db, meas.id, report_filename.c_str (), report_style == ISO6789_LIKE_REPORT_WITH_REPEATS);

          if (res.values_below_max_deviation && !res.timing_violation)
            print_result (gettext ("Kalibrierung innerhalb Toleranz"));
          else if (res.values_below_max_deviation && res.timing_violation)
            print_result (gettext ("#Kalibrierung innerhalb Toleranz jedoch Mindestzeit nicht eingehalten"));
          else
            print_result (gettext ("*Kalibrierung außerhalb Toleranz"));

          // open created pdf

#ifdef _WIN32
          ShellExecute (0, 0, report_filename.c_str (), 0, 0, SW_SHOW );
#elif __APPLE__
          char call[256];
          snprintf (call, 256, "open %s", report_filename.c_str ());
          system (call);
#else
          char call[256];
          snprintf (call, 256, "xdg-open %s", report_filename.c_str ());
          system (call);
#endif
          //print_step ( string (gettext ("Kalibrierschein:")) + " " + report_filename, 1);
        }
      else if (report_style == QUICK_CHECK_REPORT) //single peak
        {
          // Meeting from 14.01.2016: No Report for Quick Check
          //report_filename = get_time_for_filename () + "_quick_check.pdf";
          //bool res = create_quick_test_report (db, meas.id, report_filename.c_str ());

          bool res = meas.quick_check_okay ();

          if (res)
            print_result (gettext ("Schnellkalibrierung innerhalb Toleranz"));
          else
            print_result (gettext ("*Schnellkalibrierung außerhalb Toleranz"));
        }
      //else
      // print_step ("finished", 1);

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

              // create measurement_item
              measurement_item *p = new measurement_item;
              p->ts = get_localtime ();
              p->nominal_value = pmeas->get_nominal_value ();
              p->indicated_value = pmeas->get_peak_torque ();
              p->rise_time = rise_time;

              double accuracy = meas.to.accuracy;
              if (accuracy == 0)
                accuracy = meas.to.get_accuracy_from_DIN ();

              cout << "ttt::run:" << __LINE__ << " accuracy=" << accuracy
                   << " peak_torque=" << pmeas->get_peak_torque () << " is_in=" << p->is_in (accuracy) << endl;

              // color of measurement_table cell background
              Fl_Color cell_color = FL_WHITE;

              // Typ IIC and IIF use mean as nominal value
              // so it's impossible to have a live "good/bad" information
              bool use_mean_as_nominal_value = meas.to.has_no_scale () && ! meas.to.has_fixed_trigger ();
              if (! use_mean_as_nominal_value && ! p->is_in (accuracy))
                cell_color = FL_RED;

              bool overwrite_measurement =   (! p->is_in (accuracy))
                                             && (report_style == ISO6789_LIKE_REPORT_WITH_REPEATS);
              // add result to measurement table
              if (m_table)
                m_table->add_measurement (pmeas->get_peak_torque (), overwrite_measurement, cell_color);

              if (overwrite_measurement)
                {
                  //aktueller Schritt zurücksetzen (interner Counter und Messwerte löschen usw.)
                  pmeas->reset ();
                }
              else
                {
                  // add result to database
                  meas.add_measurement_item (p);
                }
            }
        }
      // FIXME: should the old measurement be visible until next measurement or not?
      //else
      //  print_peak_torque (0.0);

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

  // delete all steps
  for (vector<step*>::iterator it = steps.begin() ; it != steps.end(); ++it)
    delete (*it);

  steps.clear ();
  current_step=0;
}

// add complete DIN ISO 6789 sequences
void ttt::add_ISO6789_steps (bool repeat_on_timing_violation)
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

  cout << "ttt::add_ISO6789_steps " << meas.to.get_type_class ()
       << " runs=" << runs
       << " sign=" << sign << endl;

  for (int k=0; k<runs; ++k)
    {
      double min_torque = sign * meas.to.min_torque;
      double max_torque = sign * meas.to.max_torque;

      // check for same sign
      if ((min_torque * max_torque) < 0)
        throw out_of_range("ttt::add_ISO6789_steps: min_torque and max_torque must have the same sign");

#ifdef ISO6789_1
      // DIN EN ISO 6789-1: 6.4 b)
      // three torque tester preload cycles
      for (int k = 0; k < 3; ++k)
        add_step (new preload_torque_tester_step(max_torque, 0.1));
#elif defined (ISO6789)
      // DIN EN ISO 6789: chapter 6.1
      // no explicit preload
#else
#error No ISO 6789 variant defined
#endif

      add_step (new tare_torque_tester_step());

      vector<double> torque_list;

      if (    meas.to.is_type (1)
              || (meas.to.is_type (2) && ! meas.to.has_no_scale()))
        {
#ifdef ISO6789_1
          // min
          torque_list.push_back (min_torque);
#elif defined (ISO6789)
          // 20% * max
          torque_list.push_back (0.2 * max_torque);
#else
#error No ISO 6789 variant defined
#endif
          // 60% * max, 100% * max
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

#ifdef ISO6789
      // DIN EN ISO  6789  : 6.3 Kalibrierbedingungen
      // c.) Typ I  ... eine Vorbelastung mit dem Höchstwert ...
      // d.) Typ II ... 5 Auslösungen ohne Messung...
      if (meas.to.is_type (1))
        add_step (new preload_test_object_step(max_torque, 0.1));
      else if (meas.to.is_type (2))
        for (int k = 0; k < 5; ++k)
          add_step (new preload_test_object_step(max_torque, 0.1));

      // DIN EN ISO 6789: 6.3.1 c)
      // Nur einmalig in Funktionsrichtung das Werkzeug tarieren
      if (meas.to.is_type (1))
        add_step (new tare_test_object_step());
#endif

      for (unsigned int i=0; i < torque_list.size (); ++i)
        {
          //cout << "torque_list["<<i<<"]=" << torque_list[i] << endl;

#ifdef ISO6789_1
          // DIN EN ISO 6789-1: Anhang C
          for (int k = 0; k < 3; ++k)
            {
              if (meas.to.has_no_scale ())
                add_step (new preload_test_object_step(max_torque, 0.1));
              else
                add_step (new preload_test_object_step(torque_list[i], 0.1));
            }
#endif
          if (test_object_is_type (2))
            {
              double min_t, max_t;
              test_object::get_timing_from_DIN (meas.to.get_type_class (), torque_list[i], min_t, max_t);

              // Set max_t to 10s if not further specified
              if (max_t == 0)
                max_t = 10;

              int count_click_measurements = 5;
#ifdef ISO6789_1
              // See DIN EN ISO 6789-1:2015 6.6.1 table 7
              if (meas.to.has_no_scale ())
                count_click_measurements = 10;
#elif defined (ISO6789)
              // DIN EN ISO 6789 6.4
              if (meas.to.has_no_scale () && ! meas.to.has_fixed_trigger ())
                count_click_measurements = 10;
#else
#error No ISO 6789 variant defined
#endif

              // get peak level from test_object table
              double peak_level = meas.to.peak_trigger2_factor;
              // Use setting from TTT if peak_level <= 0
              if (peak_level <= 0)
                peak_level = get_torque_tester_peak_level ();

              for (int k = 0; k < count_click_measurements; ++k)
                add_step (new peak_click_step(torque_list[i], min_t, max_t, repeat_on_timing_violation, start_peak_torque_factor, peak_level));
            }
          else // typ 1
            {

#ifdef ISO6789_1
              // DIN EN ISO 6789-1: Anhang C
              // Bei 6789-1 wird bei jedem Wert das Werkzeug tariert
              add_step (new tare_test_object_step());
#endif

              for (int k = 0; k < 5; ++k)
                add_step (new peak_meas_step(torque_list[i], start_peak_torque_factor, stop_peak_torque_factor));
            }
        }
      sign = -sign;
    }
}

void ttt::start_sequencer (double temperature, double humidity)
{
  if (sequencer_is_running)
    throw runtime_error ("Sequencer is already running. Please stop it first");

  if (! meas.to.id)
    throw runtime_error ("ttt::start_sequencer: No valid test object selected");

  if (! measurement_output.is_open ())
    {
      string tmp = meas.to.serial_number + "_" + meas.to.manufacturer + "_" + meas.to.model;
      std::replace_if(tmp.begin(), tmp.end(), isnalnum, '_');

      ostringstream os;
      os << "./logfiles/" << get_time_for_filename () << "_" << tmp;

      // are we connected to a real TTT?
      if (pttt)
        {
          tmp = pttt->get_serial ();
          std::replace_if(tmp.begin(), tmp.end(), isnalnum, '_');
          os << "_" << tmp;
        }

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
  meas.clear_measurement_items ();
  meas.temperature = temperature;
  meas.humidity = humidity;
  meas.start_time = get_localtime();

  switch (report_style)
    {
    case QUICK_CHECK_REPORT:
      meas.norm = "quick-check";
      break;
    case ISO6789_REPORT:
      meas.norm = "ISO6789";
      break;
    case ISO6789_LIKE_REPORT_WITH_REPEATS:
      meas.norm = "ISO6789 with repeats";
      break;
    default:
      meas.norm = "undefined";
    }

  // configure measurement_table
  if (m_table)
    {
      m_table->clear ();
      unsigned int k;
      int cols = 0, col_cnt = 0, rows = 0;
      double old_nom = 0;

      // loop over all steps and find measurement_steps
      // (ignore tara, preload and so on)

      // new row for every new nominal_value
      // new col if same nominal_value

      for (k=0; k < steps.size (); ++k)
        {
          meas_step *pmeas = dynamic_cast<meas_step*>(steps[k]);
          if (pmeas)
            {
              double nom = pmeas->get_nominal_value ();
              cout << "configure measurement_table nom=" << nom << " old_nom=" << old_nom << endl;
              if (nom != old_nom)
                {
                  m_table->add_nominal_value (nom);
                  rows++;
                  col_cnt = 1;
                }
              else
                {
                  col_cnt++;
                }

              if (col_cnt > cols)
                cols = col_cnt;
              old_nom = nom;
              // is it a peak_click_step?
              //peak_click_step *pclick = dynamic_cast<peak_click_step*>(pmeas);
              //if (pclick)
              //rise_time = pclick->get_rise_time ();

            }
        }

      cout << "cfg m_table rows=" << rows << " cols=" << cols << endl;
      m_table->rows (rows);
      m_table->cols (cols);
    }


  if (pttt)
    pttt->start ();

  current_step = 0;
  print_peak_torque (0.0);
  print_indicated_torque (0.0);
  print_nominal_torque (0.0);
  sequencer_is_running = true;
}

void ttt::start_sequencer_quick_check (double temperature, double humidity, double nominal_value)
{
  if (sequencer_is_running)
    throw runtime_error ("Sequencer is already running. Please stop it first");

  clear_steps ();

  add_step (new tare_torque_tester_step());

  // 5 Messungen ohne Tara und ohne rise_time Überwachung
  // add_step (new tare_step());

  if (test_object_is_type (2))
    {
      double peak_level = meas.to.peak_trigger2_factor;
      if (peak_level <= 0)
        peak_level = get_torque_tester_peak_level ();

      for (int k = 0; k < 5; ++k)
        add_step (new peak_click_step(nominal_value, 0, 10, false, start_peak_torque_factor, peak_level));
    }
  else // Typ 1
    {
      for (int k = 0; k < 5; ++k)
        add_step (new peak_meas_step(nominal_value, start_peak_torque_factor, stop_peak_torque_factor));
    }

  report_style = QUICK_CHECK_REPORT;
  start_sequencer (temperature, humidity);
}

void ttt::start_sequencer_ISO6789 (double temperature, double humidity, bool repeat_on_timing_violation, bool repeat_on_tolerance_violation)
{
  if (sequencer_is_running)
    throw runtime_error ("Sequencer is already running. Please stop it first");

  clear_steps ();
  // min_torque and max_torque_resolution from database
  // resolution from DIN678

  add_ISO6789_steps (repeat_on_timing_violation);

  if (! repeat_on_tolerance_violation)
    report_style = ISO6789_REPORT;
  else
    report_style = ISO6789_LIKE_REPORT_WITH_REPEATS;

  start_sequencer (temperature, humidity);
}

void ttt::stop_sequencer ()
{
  cout << "ttt::stop_sequencer ()" << endl;
  if (measurement_output.is_open ())
    measurement_output.close ();

  sequencer_is_running = false;

  // stop measuring
  if (pttt)
    pttt->stop ();
}

out_cmd ttt::sequencer_inout (double torque, bool confirmation)
{
  out_cmd ret;
  step *pstep = steps[current_step];
  ret = pstep->inout (torque, confirmation);
  if (ret == RESET_CONFIRMATION)
    this->confirmation = false;
  return ret;
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

void ttt::load_torque_tester ()
{
  if (pttt)
    {
      // check if this is a new TTT (using serial number),
      // or if the next calibration date has changed (because it has been calibrated)
      int id = meas.tt.search_serial_and_next_cal_date (db, pttt->get_serial (), pttt->get_cal_date (), pttt->get_next_cal_date ());
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
          meas.tt.model                 = pttt->get_model ();
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
      // load torque_tester with id in simulation mode
      // Id 1 => TTT-300C1 with 10 Nm
      // Id 2 => TTT-300B2 with  2 Nm
      // see fill_database_debug.sql
      meas.tt.load_with_id (db, 1);
    }
}
