/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

class step for TTT (tool torque tester)

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

#ifndef STEP_H
#define STEP_H

#define TEST_DEBUG_COUT

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sys/time.h>
#include <libintl.h>

using namespace std;

enum out_cmd
{
  NO_CMD = 0,
  CMD_TARA = 1,
  RESET_CONFIRMATION = 2
};

class step
{
protected:
  int int_step;
  vector <double> v_torque;
  vector <double> v_time;

  bool finished;

  struct timeval t1;

  double max_abs_torque ();
  double max_torque ();
  double min_torque ();

public:
  step();
  virtual ~step();

  // Input vector for state machine
  // return value are command which have to be executed
  virtual out_cmd inout (double torque, bool confirmation);

  bool is_finished ();

  virtual string instruction () = 0;
  virtual string description () = 0;
};

class preload_step: public step
{
protected:
  double nominal;     // has to be reached
  double stop_thres;  // torque has to fall bellow this threshold to detect the end of the step

public:
  preload_step (double nominal, double stop_threshold_factor);
  virtual out_cmd inout (double torque, bool confirmation);

  double get_nominal_value ()
  {
    return nominal;
  }
  virtual string instruction () = 0;
  virtual string description () = 0;
};

class preload_torque_tester_step: public preload_step
{
private:

public:
  preload_torque_tester_step (double nominal, double stop_threshold_factor);
  string instruction ();
  string description ();
};

class preload_test_object_step: public preload_step
{
private:

public:
  preload_test_object_step (double nominal, double stop_threshold_factor);
  enum out_cmd inout (double torque, bool confirmation);
  string instruction ();
  string description ();
};

/*!
 * \brief Tare torque measuring device (TTT)
*/
class tare_torque_tester_step: public step
{
private:

public:
  tare_torque_tester_step ();
  virtual out_cmd inout (double torque, bool confirmation);
  string instruction ();
  string description ();
};

/*!
 * \brief Tare test object
 */
class tare_test_object_step: public step
{
private:

public:
  tare_test_object_step ();
  virtual out_cmd inout (double torque, bool confirmation);
  string instruction ();
  string description ();
};

class meas_step: public step
{
protected:
  double nominal;            // target peak torque
  double start_peak_torque;  // threshold which has to be exceeded to start the peak detection
  // typically 60% from nominal value
public:
  meas_step (double nominal, double start_peak_torque_factor);
  double get_nominal_value ()
  {
    return nominal;
  }

  virtual out_cmd inout (double torque, bool confirmation);
  virtual double get_peak_torque () = 0;
  virtual void reset ()
  {
    int_step = 0;
    finished = 0;
    v_torque.clear ();
    v_time.clear ();
  }
};

class peak_meas_step: public meas_step
{
private:
  double stop_peak_torque;   // torque has to fall bellow this threshold to detect the end of the step
  double delay_start;

public:
  peak_meas_step (double nominal, double start_peak_torque_factor,
                  double stop_peak_torque_factor);
  virtual out_cmd inout (double torque, bool confirmation);
  string instruction ();
  string description ();
  double get_peak_torque ();
};

class peak_click_step: public meas_step
{
private:
  double first_peak;
  double peak_trigger2_factor;
  double peak_trigger2_threshold;  // torque has to fall bellow this threshold to start detection of the second peak
  double min_time;
  double max_time;
  bool repeat_timing;
  double wait_t;

  double torque_80_time;
  double first_peak_time;
  double rise_time;

public:
  peak_click_step (double nominal, double min_t, double max_t, bool repeat_on_timing_violation,
                   double start_peak_torque_factor, double _peak_trigger2_factor);
  virtual out_cmd inout (double torque, bool confirmation);
  string instruction ();
  string description ();
  double get_peak_torque ();
  double get_rise_time ();
};

#endif
