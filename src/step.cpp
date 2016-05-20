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

#include <stdio.h>
#include "step.h"

step::step ()
  : int_step(0), finished(0)
{
  //cout << "step c'tor" << endl;
}

step::~step()
{
  //cout << "step d'tor" << endl;
}

out_cmd step::inout (double torque, bool confirmation)
{
  // store currrent rorque
  v_torque.push_back (torque);

  if (v_time.empty ())
    gettimeofday (&t1, NULL);

  struct timeval t2;
  gettimeofday (&t2, NULL);
  v_time.push_back ((t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1.0e6);

  (void) confirmation;
  //cout << "step::input torque = " <<  torque << " confirmation = " << confirmation << endl;
  return NO_CMD;
}

bool step::is_finished ()
{
  return this->finished;
}

double step::max_abs_torque ()
{
  double ma = max_torque ();
  double mi = min_torque ();
  double ret = ma;
  if (abs (mi) > ma)
    ret = mi;

  //cout << " step::max_abs_torque=" << ret << " size=" << v_torque.size() << endl;
  return ret;
}

double step::max_torque ()
{
  if (v_torque.size ())
    return *max_element (v_torque.begin (), v_torque.end ());
  else
    return 0;
}

double step::min_torque ()
{
  if (v_torque.size ())
    return *min_element (v_torque.begin (), v_torque.end ());
  else
    return 0;
}

//************************ preload_step ********************************************

preload_step::preload_step (double nominal, double stop_threshold_factor)
  : step ()
{
  this->nominal    = nominal;
  this->stop_thres = nominal * stop_threshold_factor;
}

enum out_cmd preload_step::inout (double torque, bool confirmation)
{
  step::inout (torque, confirmation);

  if ((int_step == 0) &&
      (  (nominal > 0 && torque >= nominal)
         || (nominal < 0 && torque <= nominal) ))
    {
      int_step++;
    }
  else if ((int_step == 1) &&
           (  (stop_thres < 0 && torque > stop_thres)
              || (stop_thres > 0 && torque < stop_thres) ))
    {
      int_step++;
      finished = true;
    }
  return NO_CMD;
}

//************************ preload_torque_tester_step ********************************************

preload_torque_tester_step::preload_torque_tester_step (double nominal, double stop_threshold_factor)
  : preload_step (nominal, stop_threshold_factor)
{
#ifdef TEST_DEBUG_COUT
  cout << "c'tor preload_torque_tester_step";
  cout << " nominal=" << nominal;
  cout << " stop_threshold_factor=" << stop_threshold_factor << endl;
#endif
}

string preload_torque_tester_step::instruction ()
{
  ostringstream oss;
  switch (int_step)
    {
    case 0:
      oss << gettext ("+Messgerät mit Nominalwert vorbelasten");
      break;
    case 1:
      oss << gettext ("Messgerät entlasten");
      break;
    case 2:
      break;
    default:
      break;
    }
  return oss.str();
}

string preload_torque_tester_step::description ()
{
  return gettext ("Vorbereitung des Messgeräts");
}

//************************ preload_test_object_step ********************************************

preload_test_object_step::preload_test_object_step (double nominal, double stop_threshold_factor)
  : preload_step (nominal, stop_threshold_factor)
{
#ifdef TEST_DEBUG_COUT
  cout << "c'tor preload_test_object_step";
  cout << " nominal=" << nominal;
  cout << " stop_threshold_factor=" << stop_threshold_factor << endl;
#endif
}

// bei 80% Weiterschalten
enum out_cmd preload_test_object_step::inout (double torque, bool confirmation)
{
  step::inout (torque, confirmation);

  if ((int_step == 0) &&
      (  (nominal > 0 && torque >= 0.8 * nominal)
         || (nominal < 0 && torque <= 0.8 * nominal) ))
    {
      int_step++;
    }
  else if ((int_step == 1) &&
           (  (stop_thres < 0 && torque > stop_thres)
              || (stop_thres > 0 && torque < stop_thres) ))
    {
      int_step++;
      finished = true;
    }
  return NO_CMD;
}

string preload_test_object_step::instruction ()
{
  ostringstream oss;
  switch (int_step)
    {
    case 0:
      oss << gettext ("+Drehmoment-Schraubwerkzeug mit Nominalwert vorbelasten");
      break;
    case 1:
      oss << gettext ("Drehmoment-Schraubwerkzeug entlasten");
      break;
    case 2:
      break;
    default:
      break;
    }
  return oss.str();
}

string preload_test_object_step::description ()
{
  return gettext ("Vorbereitung des Drehmoment-Schraubwerkzeug");
}

//************************ tare_torque_tester_step ********************************************

tare_torque_tester_step::tare_torque_tester_step ()
  : step()
{
#ifdef TEST_DEBUG_COUT
  cout << "c'tor tare_torque_tester_step" << endl;
#endif
}

enum out_cmd tare_torque_tester_step::inout (double torque, bool confirmation)
{
  step::inout (torque, confirmation);
  enum out_cmd ret = NO_CMD;

  // wait 5s until starting tare on the device so that the user can
  // completely release the device
//cout << "tare_torque_tester_step::inout v_time.back ()=" << v_time.back () << endl;
  if (int_step == 0 && v_time.back () > 5)
    {
      int_step = 1;
    }
  else if (int_step == 1)
    {
      ret = CMD_TARA;
      if (confirmation)
        int_step++;
    }
  else if (int_step == 2)
    {
      int_step++;
      finished = true;
    }
  return ret;
}

string tare_torque_tester_step::instruction ()
{
  ostringstream oss;
  switch (int_step)
    {
    case 0:
      oss << gettext ("Drehmoment-Schraubwerkzeug entfernen und Messgerät für Tara nicht bewegen");
      break;
    case 1:
      oss << gettext ("Messgerät nicht bewegen, Tara läuft");
      break;
    case 2:
      break;
    default:
      break;
    }
  return oss.str();
}

string tare_torque_tester_step::description ()
{
  return gettext ("Nullstellung des Messgeräts");
}

//************************ tare_test_object_step ********************************************

tare_test_object_step::tare_test_object_step ()
  : step()
{
#ifdef TEST_DEBUG_COUT
  cout << "c'tor tare_test_object_step" << endl;
#endif
}

enum out_cmd tare_test_object_step::inout (double torque, bool confirmation)
{
  step::inout (torque, confirmation);

  if (int_step == 0 && v_time.back () > 5)
    {
      int_step++;
    }
  else if (int_step == 1 && confirmation)
    {
      int_step++;
      return RESET_CONFIRMATION;
    }
  else if (int_step == 2)
    {
      int_step++;
      finished = true;
    }
  return NO_CMD;
}

string tare_test_object_step::instruction ()
{
  ostringstream oss;
  switch (int_step)
    {
    case 0:
      oss << gettext ("Drehmoment-Schraubwerkzeug aus dem Messgerät entnehmen und 5s warten");
      break;
    case 1:
      oss << gettext ("Drehmoment-Schraubwerkzeug tarieren, wieder in das Messgerät einsetzen und die Schaltfläche 'Bestätigung' drücken");
      break;
    case 2:
      break;
    default:
      break;
    }
  return oss.str();
}

string tare_test_object_step::description ()
{
  return gettext ("Nullstellung des Drehmoment-Schraubwerkzeug");
}

//************************ meas_step ********************************************

meas_step::meas_step (double nominal, double start_peak_torque_factor)
  : step ()
{
  this->nominal = nominal;
  this->start_peak_torque = nominal * start_peak_torque_factor;
}

out_cmd meas_step::inout (double torque, bool confirmation)
{
  return step::inout (torque, confirmation);
}

//************************ peak_meas_step ********************************************

peak_meas_step::peak_meas_step (double nominal, double start_peak_torque_factor, double stop_peak_torque_factor)
  : meas_step (nominal, start_peak_torque_factor)
{
  this->stop_peak_torque = nominal * stop_peak_torque_factor;

#ifdef TEST_DEBUG_COUT
  cout << "c'tor peak_meas_step";
  cout << " nominal=" << nominal;
  cout << " start_peak_torque_factor=" << start_peak_torque_factor;
  cout << " stop_peak_torque=" << stop_peak_torque << endl;
#endif
}

enum out_cmd peak_meas_step::inout (double torque, bool confirmation)
{
  meas_step::inout (torque, confirmation);
  /*
  cout << "peak_meas_step::inout"
       << " torque=" << torque
       << " start_peak_torque=" << start_peak_torque
       << " stop_peak_torque=" << stop_peak_torque << endl;
  */
  if ((int_step == 0) &&
      ( (start_peak_torque > 0 && torque >= start_peak_torque)
        || (start_peak_torque < 0 && torque <= start_peak_torque) ))
    {
      int_step++;
    }
  else if ((int_step == 1) &&
           ( (stop_peak_torque > 0 && torque < stop_peak_torque)
             || (stop_peak_torque < 0 && torque > stop_peak_torque) ))
    {
      int_step++;
      delay_start = v_time.back ();
    }
  else if ((int_step == 2) &&
           (v_time.back () - delay_start) > 0.5)
    {
      int_step++;
      finished = true;
    }
  return NO_CMD;
}

string peak_meas_step::instruction ()
{
  ostringstream oss;
  switch (int_step)
    {
    case 0:
      oss << gettext ("+Drehmoment-Schraubwerkzeug mit Nominalwert belasten");
      break;
    case 1:
      oss << gettext ("Drehmoment-Schraubwerkzeug mit Nominalwert belasten und anschließend entlasten");
      break;
    default:
      break;
    }
  return oss.str();
}

string peak_meas_step::description ()
{
  return gettext ("Peakmessung Typ I");
}

double peak_meas_step::get_peak_torque ()
{
  return max_abs_torque ();
}

//************************ peak_click_step ********************************************

peak_click_step::peak_click_step (double nominal,
                                  double min_t,
                                  double max_t,
                                  bool repeat_on_timing_violation,
                                  double start_peak_torque_factor,
                                  double _peak_trigger2_factor)
  : meas_step (nominal, start_peak_torque_factor),
    first_peak (0),
    peak_trigger2_factor (_peak_trigger2_factor),
    peak_trigger2_threshold (0),
    min_time (min_t),
    max_time (max_t),
    repeat_timing (repeat_on_timing_violation),
    wait_t (0),
    torque_80_time (0),
    first_peak_time (0),
    rise_time (0)
{
#ifdef TEST_DEBUG_COUT
  cout << "c'tor peak_click_step";
  cout << " nominal=" << nominal;
  cout << " min_time=" << min_time;
  cout << " max_time=" << max_time;
  cout << " repeat_timing=" << repeat_timing;
  cout << " start_peak_torque_factor=" << start_peak_torque_factor;
  cout << " peak_trigger2_factor=" << peak_trigger2_factor << endl;
#endif
}

out_cmd peak_click_step::inout (double torque, bool confirmation)
{
  meas_step::inout (torque, confirmation);

  /*
    cout << "torque=" << torque;
    cout << " nominal=" << nominal;
    cout << " start_peak_torque=" << start_peak_torque;
    cout << " peak_trigger2_threshold=" << peak_trigger2_threshold;
    cout << endl;
  */

  if ((int_step == 0) &&
      ( (start_peak_torque > 0 && torque >= start_peak_torque)
        || (start_peak_torque < 0 && torque <= start_peak_torque)))
    {
      int_step = 1;
    }
  else if (int_step == 1) // start schwelle überschritten
    {
      if (   (nominal > 0 && ((torque * peak_trigger2_factor) > peak_trigger2_threshold))
             || (nominal < 0 && ((torque * peak_trigger2_factor) < peak_trigger2_threshold)))
        {
          //cout << "update peak_trigger2_threshold" << endl;
          peak_trigger2_threshold = torque * peak_trigger2_factor;
        }

      if ( (nominal > 0 && torque < peak_trigger2_threshold)
           || (nominal < 0 && torque > peak_trigger2_threshold))
        {
          this->first_peak = max_abs_torque ();
          /*
          cout << "peak_click_step::inout peak_triggered."
               << " torque=" << torque
               << " first_peak=" << first_peak
               << " peak_trigger2_threshold=" << peak_trigger2_threshold
               << endl;
          */
          int_step = 2;
        }
    }
  else if (int_step == 2) // peak_trigger2_threshold unterschritten
    {

      if (torque_80_time == 0 && first_peak_time == 0)
        {
          double sign = (nominal > 0)? 1 : -1;
          // Anstiegszeit überprüfen
          // von 80% * first_peak bis first_peak
          unsigned int k = 0;
          while (k < v_torque.size () && (sign * v_torque[k]) < (sign * 0.8 * first_peak))
            k++;

          torque_80_time = v_time[k];

          // first_peak_index suchen
          while (k < v_torque.size () && (sign * v_torque[k]) < (sign * first_peak))
            k++;

          first_peak_time = v_time[k];

          rise_time = first_peak_time - torque_80_time;
          //cout << "peak_click_step::inout rise_time=" << rise_time << endl;

          if (repeat_timing)
            if (rise_time < min_time)  //too fast
              int_step = 4;
          if (rise_time > max_time)  // too slow
            int_step = 5;
        }
      else
        {
          // Die Variante hier wartet, bis torque 2s lang unter der peak_trigger2_threshold Schwelle war
          // (ignoriert wenn die Kraft später nochmal ansteigt)
          if (   (nominal > 0 && torque < peak_trigger2_threshold)
                 || (nominal < 0 && torque > peak_trigger2_threshold))
            {
              wait_t += v_time[v_time.size()-1] - v_time[v_time.size()-2];
            }

          if (wait_t > 2.0)
            {
              int_step++;
              finished = true;

              /*
              cout << "peak_click_step::input first_peak=" << first_peak
                   << " max_abs_torque=" << max_abs_torque ()
                   << " nominal=" << nominal << endl;
              */
            }
        }

    }
  else if (int_step == 4 || int_step == 5) // rise-time violation
    {
      wait_t += v_time[v_time.size()-1] - v_time[v_time.size()-2];

      if (wait_t > 3.0)
        {
          wait_t = 0;
          first_peak = 0;
          peak_trigger2_threshold = 0;
          torque_80_time = 0;
          first_peak_time = 0;
          rise_time = 0;
          int_step = 0;
          v_torque.clear ();
          v_time.clear ();
        }
    }

  return NO_CMD;
}

string peak_click_step::instruction ()
{
  ostringstream oss;
  char buf[100];
  switch (int_step)
    {
    case 0:
    case 1:
      oss << gettext ("+Drehmoment-Schraubwerkzeug belasten bis es auslöst");
      break;
    case 2:
      oss << gettext ("Drehmoment-Schraubwerkzeug entlasten");
      break;
    case 3:
      break;
    case 4:
      //cout << "rise_time=" << rise_time << endl;
      snprintf (buf, 100, gettext ("*Drehmomentanstieg war zu schnell (%.2fs < %.2fs). Wiederholung"), rise_time, min_time);
      oss << buf;
      break;
    case 5:
      snprintf (buf, 100, gettext ("*Drehmomentanstieg war zu langsam (%.2fs > %.2fs). Wiederholung"), rise_time, max_time);
      oss << buf;
      break;
    default:
      break;
    }
  return oss.str();
}

string peak_click_step::description ()
{
  return gettext ("Peakmessung Typ II");
}

double peak_click_step::get_peak_torque ()
{
  return this->first_peak;
}

double peak_click_step::get_rise_time ()
{
  return rise_time;
}
