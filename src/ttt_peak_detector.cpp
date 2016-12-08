#include "ttt_peak_detector.h"

ttt_peak_detector::ttt_peak_detector ()
  : thres_start (0),
    thres_stop (0),
    thres_peak1_rel (0.9),
    stop_delay_samples (100)
{
  cout << "ttt_peak_detector::ttt_peak_detector ()" << endl;
  clear ();
}

bool ttt_peak_detector::update (double v)
{
  bool retval = false;
  static int timer = 0;
  /* Algorithmus / Idee
   * Start der Peakdetektion wenn > thres_start (z.B. 2% von M-Max)
   * Detektion des 1. Peaks wenn Wert < thres_peak1_rel * cumsum
   * Kommt noch ein zweiter Peak bei dem zwischen Minimum und Maximum ein delta von > (1-thres_peak1_rel)/3 * Peak1 liegt,
   * wird auch das Minimum und der zweite Peak detektiert
   * Ende Detektion wenn Wert für 0.2s < thres_stop (z.B. 1% von M_max) bleibt
  */

  // cummax aktualisieren
  if (v > cummax)
    {
      cummax = v;
      cummax_i = v_cnt;
    }

  // cummin aktualisieren
  if (v < cummin)
    {
      cummin = v;
      cummin_i = v_cnt;
    }

  if (state == 0 && v > thres_start)
    {
      state = 1;
      ps.push_back (peakset ());
      ps.back().start_x = v_cnt;
      ps.back().min_after_peak1_x = -1;
      ps.back().min_after_peak1_y = 0;
      ps.back().peak2_x = -1;
      ps.back().peak2_y = 0;
      ps.back().stop_x = -1;
    }
  else if (state == 1 && v < thres_peak1_rel * cummax)
    {
      state = 2;
      ps.back().peak1_x = cummax_i;
      ps.back().peak1_y = cummax;
      cummin = cummax;
    }
  else if (state == 2 && v > cummin + (1-thres_peak1_rel)/3 * ps.back().peak1_y)
    {
      state = 3;
      cummax = v;
      ps.back().min_after_peak1_x = cummin_i;
      ps.back().min_after_peak1_y = cummin;
    }
  else if ((state == 2 || state == 3) && v < thres_stop)
    {
      if (state == 3)
        {
          ps.back().peak2_x = cummax_i;
          ps.back().peak2_y = cummax;
        }
      state = 4;
    }
  else if (state == 4)
    {
      //printf ("timer=%i\n", timer);
      if (v > thres_stop)
        timer = 0;
      else
        timer++;

      if (timer > stop_delay_samples)
        {
          state = 0;
          timer = 0;
          ps.back().stop_x = v_cnt;
          cummax = 0;
          num_peaks++;
          retval = true;
        }
    }
  v_cnt++;
  
  static int old_state = 0;
  static int old_v_cnt = 0;
  if ((v_cnt - old_v_cnt) > 900 || state != old_state)
    {
      printf ("v_cnt=%6i, thres_start=%5.2f, thres_peak1_rel=%5.2f, cummax=%5.2f, cummin=%5.2f, thres_stop=%5.2f, state=%i\n", v_cnt, thres_start, thres_peak1_rel, cummax, cummin, thres_stop, state);
      old_v_cnt = v_cnt;
      old_state = state;
    }
  return retval;
}
