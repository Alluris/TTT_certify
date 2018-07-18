/*

Copyright (C) 2016 Alluris GmbH & Co. KG <weber@alluris.de>

standalone peak detector

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

#ifndef PEAK_DET_H
#define PEAK_DET_H

#include <iostream>
#include <cstdio>
#include <vector>
#include <stdexcept>

using namespace std;

struct peakset
{
  int start_x;

  int peak1_x;
  double peak1_y;

  int min_after_peak1_x;
  double min_after_peak1_y;

  int peak2_x;
  double peak2_y;

  int stop_x;
};

class ttt_peak_detector
{
private:

  double cummax;
  double cummin;
  int cummax_i;
  int cummin_i;
  unsigned int num_peaks;
  int state;
  int v_cnt;

  // Thresholds
  double thres_start;
  double thres_stop;
  double thres_peak1_rel;

  int stop_delay_samples;

  vector <peakset> ps;
public:
  ttt_peak_detector ();

  void set_thresholds (double start, double stop, double peak1)
  {
    thres_start = start;
    thres_stop = stop;
    thres_peak1_rel = peak1;
    printf ("set_thresholds: thres_start=%.2f thres_stop=%.2f thres_peak1_rel=%.2f\n", thres_start, thres_stop, thres_peak1_rel);
  }

  void clear ()
  {
    cummax = 0;
    cummin = 0;
    cummax_i = 0;
    cummin_i = 0;
    num_peaks = 0;
    state = 0;
    v_cnt = 0;
    ps.clear ();
  }

  // return true if there is a new peakset available
  bool update (double v);

  void print_stats ()
  {
    cout << "Processed " << v_cnt << " values, found " << num_peaks << " peaksets" << endl;
    printf ("Nr.  start peak1x peak1y minPeak1x minPeak1y peak2x peak2y stop\n");
    for (unsigned int k=0; k < num_peaks; ++k)
      {
        printf ("#%2u %6i  %6i    %5.1f %6i %5.1f %6i %5.1f %6i\n",
                k,
                ps[k].start_x,
                ps[k].peak1_x, ps[k].peak1_y,
                ps[k].min_after_peak1_x, ps[k].min_after_peak1_y,
                ps[k].peak2_x, ps[k].peak2_y,
                ps[k].stop_x);
      }
  }

  int get_num_peaksets ()
  {
    return num_peaks;
  }

  peakset get_peakset (unsigned int num)
  {
    if (num < num_peaks)
      return ps[num];
    else
      throw std::out_of_range ("No peakset with given num");
  }

  peakset get_last_peakset ()
  {
    if (num_peaks > 0)
      return ps[num_peaks - 1];
    else
      throw std::out_of_range ("No peakset available");
  }

};

#endif
