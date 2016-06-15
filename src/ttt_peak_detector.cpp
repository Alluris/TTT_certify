#include "ttt_peak_detector.h"

ttt_peak_detector::ttt_peak_detector ()
  : thres_start (0),
    thres_stop (0),
    thres_peak1_rel (0.9)
{
  cout << "ttt_peak_detector::ttt_peak_detector ()" << endl;
  clear ();
}

bool ttt_peak_detector::update (double v)
{
  bool retval = false;
  /* Algorithmus / Idee
   * Start der Peakdetektion wenn > thres_start (z.B. 2% von M-Max)
   * Detektion des 1. Peaks wenn Wert < thres_peak1_rel * cumsum
   * Kommt noch ein zweiter Peak bei dem zwischen Minimum und Maximum ein delta von > (1-thres_peak1_rel)/3 * Peak1 liegt,
   * wird auch das Minimum und der zweite Peak detektiert
   * Ende Detektion wenn Wert < thres_stop (z.B. 1% von M_max)
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
    }
  else if (state == 1 && v < thres_peak1_rel * cummax)
    {
      state = 2;
      ps.push_back (peakset ());
      ps.back().peak1_x = cummax_i;
      ps.back().peak1_y = cummax;

      ps.back().min_after_peak1_x = -1;
      ps.back().min_after_peak1_y = 0;

      ps.back().peak2_x = -1;
      ps.back().peak2_y = 0;

      cummin = cummax;
    }
  else if (state == 2 && v > cummin + (1-thres_peak1_rel)/3 * ps.back().peak1_y)
    {
      state = 3;
      cummax = v;
      ps.back().min_after_peak1_x = cummin_i;
      ps.back().min_after_peak1_y = cummin;
    }
  else if (state >= 2 && v < thres_stop)
    {
      if (state == 3)
        {
          ps.back().peak2_x = cummax_i;
          ps.back().peak2_y = cummax;
        }
      state = 0;
      cummax = 0;
      num_peaks++;
      retval = true;
    }
  v_cnt++;
  return retval;
}